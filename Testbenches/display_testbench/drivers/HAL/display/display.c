/***************************************************************************//**
  @file     display.c
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/
//todo check what happens if overflow in PSIR tick counter
//todo add "DEVELOPMENT IF"  to check possible segmentation fault while accessing in indexed mode

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "../../MCAL/systick/systick.h"
#include "../../MCAL/gpio/gpio.h"
#include "display.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	SYSTICK_MS				    ( 1.0 / SYSTICK_ISR_FREQUENCY_HZ * 1000.0 )
#define MS2TICKS(ms)  			    ( (ms) / SYSTICK_MS )

#define MIN_REFRESH_MS              10
#define DECODER_MAX                 4
#define DISPLAY_SEGMENT_PIN_COUNT   8
#define DISPLAY_PIN_MASK            1
#define DPOINT_ENABLE_MASK          0x80
#define EMPTY_DISPLAY               0x00

/* pinout */
#define DISPLAY_SEGA_PIN            PORTNUM2PIN(PC,5)   //! (definir adecuadamente)
#define DISPLAY_SEGB_PIN            PORTNUM2PIN(PC,7)   //! (definir adecuadamente)
#define DISPLAY_SEGC_PIN            PORTNUM2PIN(PC,0)   //! (definir adecuadamente)
#define DISPLAY_SEGD_PIN            PORTNUM2PIN(PC,9)   //! (definir adecuadamente)
#define DISPLAY_SEGE_PIN            PORTNUM2PIN(PC,8)   //!  (definir adecuadamente)
#define DISPLAY_SEGF_PIN            PORTNUM2PIN(PC,1)   //! (definir adecuadamente)
#define DISPLAY_SEGG_PIN            PORTNUM2PIN(PB,19)  //! (definir adecuadamente)
#define DISPLAY_DPOINT_PIN          PORTNUM2PIN(PB,18)  //! (definir adecuadamente)
#define DISPLAY_DECODER_PIN_A       PORTNUM2PIN(PC,3)   //! decoder MSB (definir adecuadamente)
#define DISPLAY_DECODER_PIN_B       PORTNUM2PIN(PC,2)   //! decoder LSB (definir adecuadamente)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum{
    MAX_DISPLAY_BRIGHTNESS = 1,
    INT_DISPLAY_BRIGHTNESS = 3,
    MIN_DISPLAY_BRIGHTNESS = 5

}display_brightness_t;

typedef struct {
    display_brightness_t brightnessLevel;
    display_brightness_t brightnessCount;
    uint8_t character;
    bool isActiveLow;
    bool isEnabled;

} display_t;



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static const uint8_t segmentPinout[] = 
{
    DISPLAY_SEGA_PIN,
    DISPLAY_SEGB_PIN,
    DISPLAY_SEGC_PIN,
    DISPLAY_SEGD_PIN,
    DISPLAY_SEGE_PIN,
    DISPLAY_SEGF_PIN,
    DISPLAY_SEGG_PIN,
    DISPLAY_DPOINT_PIN

};

/* Decode table extracted from https://en.wikichip.org/wiki/seven-segment_display/representing_letters and modified*/

static const uint8_t seven_seg_digits_decode_gfedcba[78]= {
/*  -     .     /                                                           */
    0x40, 0x00, 0x00,    
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x00, 
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x00, 0x39, 0x00, 0x79, 0x71, 0x3D, 
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
    0x76, 0x30, 0x1E, 0x00, 0x38, 0x00, 0x00, 0x3F, 0x73, 0x00, 0x00, 0x6D, 
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
    0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
    0x00, 0x5F, 0x7C, 0x58, 0x5E, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
    0x00, 0x00, 0x54, 0x5C, 0x00, 0x67, 0x50, 0x00, 0x78, 0x1C, 0x00, 0x00, 
/*  x     y     z     */
    0x00, 0x6E, 0x00
};

static display_t displays[] = 
{

 {MAX_DISPLAY_BRIGHTNESS, MAX_DISPLAY_BRIGHTNESS, EMPTY_DISPLAY, true, true},
 {MAX_DISPLAY_BRIGHTNESS, MAX_DISPLAY_BRIGHTNESS, EMPTY_DISPLAY, true, true},
 {MAX_DISPLAY_BRIGHTNESS, MAX_DISPLAY_BRIGHTNESS, EMPTY_DISPLAY, true, true},
 {MAX_DISPLAY_BRIGHTNESS, MAX_DISPLAY_BRIGHTNESS, EMPTY_DISPLAY, true, true}

};


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void displayInit(void)
{
    // decoder pin init (GPIO)
    gpioWrite(DISPLAY_DECODER_PIN_A, LOW);
    gpioWrite(DISPLAY_DECODER_PIN_B, LOW);

    gpioMode(DISPLAY_DECODER_PIN_A, OUTPUT);
    gpioMode(DISPLAY_DECODER_PIN_B, OUTPUT);

    // 7-segment pin init (GPIO)
    for (uint8_t i = 0 ; i < sizeof(segmentPinout) ; i++)
    {
        gpioWrite(segmentPinout[i], HIGH);
        gpioMode(segmentPinout[i], OUTPUT);
    }

    // PISR init (SYSTICK)
    SysTick_Init(displayPISR); // periodic interruption registration

}

void displayWrite(display_id_t id, uint8_t chr)
{
    uint8_t decodedChar = decode7seg(chr);

    if(displays[id].character & DPOINT_ENABLE_MASK)
    {
        decodedChar |= DPOINT_ENABLE_MASK;
    }

    displays[id].character = decodedChar;   
}

void displayEnable(display_id_t id)
{
    displays[id].isEnabled = true;
}

void displayDisable(display_id_t id)
{
    displays[id].isEnabled = false;
}

void displaySetBrightness(display_id_t id, display_brightness_t brightness)
{
    displays[id].brightnessLevel = brightness;
}

void displayDotSet(display_id_t id)
{
    displays[id].character |= DPOINT_ENABLE_MASK;
}

void displayDotClear(display_id_t id)
{
    displays[id].character &= ~DPOINT_ENABLE_MASK;
}

void displayClear(display_id_t id)
{
    displays[id].character = EMPTY_DISPLAY;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void displayPISR(void)
{
  static uint32_t tickCount = 0;          // PISR period time control

    if ( tickCount == MS2TICKS(MIN_REFRESH_MS) )
    {
        for (uint8_t i = 0 ; i < DISPLAY_COUNT ; i++)
        {

            if (displays[i].brightnessCount == 0)
            {
                if (displays[i].isEnabled)
                {
                        uint8_t character = displays[i].character;

                    for (uint8_t j = 0 ; j < DISPLAY_SEGMENT_PIN_COUNT ; j++)
                    {
                        uint8_t out = character & DISPLAY_PIN_MASK;
                        gpioWrite(segmentPinout[j], displays[i].isActiveLow ? ~out : out );
                        character = character >> 1;
                    }

                    displays[i].brightnessCount = displays[i].brightnessLevel; // reload brightness counter

                    /* Clear current display before moving to the next*/

                    for (uint8_t j = 0 ; j < DISPLAY_SEGMENT_PIN_COUNT ; j++)
                    {
                        gpioWrite(segmentPinout[j], displays[i].isActiveLow ? HIGH : LOW);
                    }
                }

                else
                {
                    displays[i].brightnessCount--;
                }          

            }
      
        }

    tickCount = 0;    // reset counter
    }

    else
    {
        tickCount++;
    }


}

static uint8_t decode7seg(uint8_t chr)
{ /* assuming implementation uses ASCII */
    if (chr > (uint8_t)'z')
        return 0x00;
    return seven_seg_digits_decode_gfedcba[chr - '-'];
}

/******************************************************************************/
