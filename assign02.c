// Must declare the main assembly entry point before use.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "assign02.pio.h"

#define IS_RGBW true  // Will use RGBW format
#define NUM_PIXELS 1  // There is 1 WS2812 device in the chain
#define WS2812_PIN 28 // The GPIO pin that the WS2812 connected to
#define MAXIMUM 36
int wrongAns;
int rightAns;
int total;
int accuracy;
char input_array[20];
void stats();

// Declare the main assembly code entry point.
void main_asm();

typedef struct equivalentcode
{
    char word;
    char *morse;
} equivalentcode;

equivalentcode alphanumeric[MAXIMUM];

/**
 * @brief This function initialises the watch dog timer
 *
 */
void asm_watch_update()
{
    watchdog_update();
}

/**
 * @brief This function has inputted 36 letters and numbers. It also has the equivalent morse-code.
 *
 */
void input_morse()
{
    alphanumeric[0].word = 'A';
    alphanumeric[1].word = 'B';
    alphanumeric[2].word = 'C';
    alphanumeric[3].word = 'D';
    alphanumeric[4].word = 'E';
    alphanumeric[5].word = 'F';
    alphanumeric[6].word = 'G';
    alphanumeric[7].word = 'H';
    alphanumeric[8].word = 'I';
    alphanumeric[9].word = 'J';
    alphanumeric[10].word = 'K';
    alphanumeric[11].word = 'L';
    alphanumeric[12].word = 'M';
    alphanumeric[13].word = 'N';
    alphanumeric[14].word = 'O';
    alphanumeric[15].word = 'P';
    alphanumeric[16].word = 'Q';
    alphanumeric[17].word = 'R';
    alphanumeric[18].word = 'S';
    alphanumeric[19].word = 'T';
    alphanumeric[20].word = 'U';
    alphanumeric[21].word = 'V';
    alphanumeric[22].word = 'W';
    alphanumeric[23].word = 'X';
    alphanumeric[24].word = 'Y';
    alphanumeric[25].word = 'Z';
    alphanumeric[26].word = '0';
    alphanumeric[27].word = '1';
    alphanumeric[28].word = '2';
    alphanumeric[29].word = '3';
    alphanumeric[30].word = '4';
    alphanumeric[31].word = '5';
    alphanumeric[32].word = '6';
    alphanumeric[33].word = '7';
    alphanumeric[34].word = '8';
    alphanumeric[35].word = '9';

    alphanumeric[0].morse = ".-";
    alphanumeric[1].morse = "-...";
    alphanumeric[2].morse = "-.-.";
    alphanumeric[3].morse = "-..";
    alphanumeric[4].morse = ".";
    alphanumeric[5].morse = "..-.";
    alphanumeric[6].morse = "--.";
    alphanumeric[7].morse = "....";
    alphanumeric[8].morse = "..";
    alphanumeric[9].morse = ".---";
    alphanumeric[10].morse = "-.-";
    alphanumeric[11].morse = ".-..";
    alphanumeric[12].morse = "--";
    alphanumeric[13].morse = "-.";
    alphanumeric[14].morse = "---";
    alphanumeric[15].morse = ".--.";
    alphanumeric[16].morse = "--.-";
    alphanumeric[17].morse = ".-.";
    alphanumeric[18].morse = "...";
    alphanumeric[19].morse = "-";
    alphanumeric[20].morse = "..-";
    alphanumeric[21].morse = "...-";
    alphanumeric[22].morse = ".--";
    alphanumeric[23].morse = "-..-";
    alphanumeric[24].morse = "-.--";
    alphanumeric[25].morse = "--..";
    alphanumeric[26].morse = "-----";
    alphanumeric[27].morse = ".----";
    alphanumeric[28].morse = "..---";
    alphanumeric[29].morse = "...--";
    alphanumeric[30].morse = "....-";
    alphanumeric[31].morse = ".....";
    alphanumeric[32].morse = "-....";
    alphanumeric[33].morse = "--...";
    alphanumeric[34].morse = "---..";
    alphanumeric[35].morse = "----.";
}

// input_morse();
/**
 * @brief Wrapper function used to call the underlying PIO
 *        function that pushes the 32-bit RGB colour value
 *        out to the LED serially using the PIO0 block. The
 *        function does not return until all of the data has
 *        been written out.
 *
 * @param pixel_grb The 32-bit colour value generated by urgb_u32()
 */
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

/**
 * @brief
 *
 * @param r
 * @param g
 * @param b
 * @return uint32_t
 */

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void set_rgb_off()
{
    put_pixel(urgb_u32(0x00, 0x00, 0x00));
}

void set_rgb_red()
{
    put_pixel(urgb_u32(0x7F, 0x00, 0x00));
}

void set_rgb_yellow()
{
    put_pixel(urgb_u32(0x0F, 0x0F, 0x0F));
}

void set_rgb_blue()
{
    put_pixel(urgb_u32(0x00, 0x00, 0x7F));
}
void set_rgb_orange()
{
    put_pixel(urgb_u32(0x7F, 0x40, 0x00));
}

void set_rgb_green()
{
    put_pixel(urgb_u32(0x00, 0x7F, 0x00));
}

/**
 * @brief This function displays the welcome message when the game starts.
 *
 */
void Welcome_StartMessage()
{
    printf("+---------------------------------------------------------+\n");
    printf(":                    Group 8 ASSIGNMENT2                  :\n");
    printf("+---------------------------------------------------------+\n");
    printf(":      #       #######     #    ######   #      #         :\n");
    printf(":      #       #          # #   #    #   # #    #         :\n");
    printf(":      #       #         #   #  #    #   #  #   #         :\n");
    printf(":      #       #######  ######  ######   #   #  #         :\n");
    printf(":      #       #        #    #  #  #     #    # #         :\n");
    printf(":      #       #        #    #  #   #    #     ##         :\n");
    printf(":      ####### #######  #    #  #    #   #      #         :\n");
    printf(":                                                         :\n");
    printf(":    #        #    #######    ######    #####   ######    :\n");
    printf(":    # #    # #   #       #   #     #  #     #  #         :\n");
    printf(":    #   # #  #   #       #   #     #  #        #         :\n");
    printf(":    #    #   #   #       #   ######    #####   #####     :\n");
    printf(":    #        #   #       #   #  #           #  #         :\n");
    printf(":    #        #   #       #   #   #    #     #  #         :\n");
    printf(":    #        #    ########   #    #   ######   ######    :\n");
    printf(":                                                         :\n");
    printf(":             ######   #######   #####     ######         :\n");
    printf(":           ##        #       #  #    #    #              :\n");
    printf(":          ##         #       #  #     #   #              :\n");
    printf(":          ##         #       #  #     #   #####          :\n");
    printf(":          ##         #       #  #     #   #              :\n");
    printf(":           ##        #       #  #    #    #              :\n");
    printf(":            #######   ########  ####      ######         :\n");
    printf("+---------------------------------------------------------+\n");
    printf(":       USE GP21 TO ENTER A SEQUENCE TO BEGIN             :\n");
    printf(":       "
           "-----"
           " for level 01 - CHARS (EASY)             :\n");
    printf(":       "
           ".----"
           " for level 02 - CHARS (HARD)             :\n");
}

int var;
int life = 3;
int streak = 0;

/**
 * @brief This function takes input the lives left and displays the equivalent color on the pi-pico
 *
 * @param num
 */

void rgb_life(int num)
{
    if (num == 3)
    {
        set_rgb_green();
    }
    else if (num == 2)
    {
        set_rgb_yellow();
    }
    else if (num == 1)
    {
        set_rgb_orange();
    }
    else if (num == 0)
    {
        set_rgb_red();
    }
    else
    {
        set_rgb_blue();
    }
}
int comp;
char compar[20];

/**
 * @brief This function implements the Level 1 and gives the character along with the equivalent morse code
 *
 */
void level1()
{
    var = rand() % 36;
    strcpy(compar, alphanumeric[var].morse);
    printf("+-----------------------------------------------------------+\n");
    printf(":                    ENTERING LEVEL 1                       :\n");
    printf("+-----------------------------------------------------------+\n");
    printf("                       INSTRUCTIONS                          \n");
    printf(" You will need to enter the morse code of the character given\n");
    printf("        you are also given the morse code                    \n");
    printf("+-----------------------------------------------------------+\n");
    printf("                  Given character    :  %c                   \n", alphanumeric[var].word);
    printf("              The equivalent morse code : %s                 \n", alphanumeric[var].morse);
    printf("+-----------------------------------------------------------+\n");
    main_asm();
    printf("%s\n", input_array);
    comp = strcmp(input_array, compar);
    if (comp == 0)
    {

        streak++;
        printf("You wrote the correct sequence. YOUR STREAK : %d\n", streak);
        if (life < 3)
        {
            life++;
            rgb_life(life);
        }
        if (streak < 5)
        {
            printf("Number of lives left                        : %d\n", life);
            level1();
        }
        else
        {
            printf("\n\n\n\n\n");
            printf("+-----------------------------------------------------------+\n");
            printf(":                    ENTERING LEVEL 2                       :\n");
            printf("+-----------------------------------------------------------+\n");
            printf("\n\n\n\n\n");
            streak = 0;
            life = 3;
            set_rgb_green();
            level2();
        }
    }
    else
    {
        life--;
        printf("Number of lives left                        : %d\n", life);
        rgb_life(life);
        streak = 0;
        printf("You wrote the wrong sequence. YOUR STREAK : %d\n", streak);
        if (life == 0)
        {

            printf("+-----------------------------------------------------------+\n");
            printf(":         GAME OVER, WAIT FOR 8 SECONDS TO RESTART          :\n");
            printf("+-----------------------------------------------------------+\n");
            Welcome_StartMessage();
        }
        else
        {
            printf("+-----------------------------------------------------------+\n");
            printf(":             WRONG ANSWER, TRY AGAIN                       :\n");
            printf("+-----------------------------------------------------------+\n");

            level1();
        }
    }
}

/**
 * @brief This function implements the stats and shows the character to be inputted to the user. If the user wins, the statistics are shown
 *
 */
void level2()
{
    var = rand() % 36;
    strcpy(compar, alphanumeric[var].morse);
    printf("+-----------------------------------------------------------+\n");
    printf(":                    ENTERING LEVEL 2                       :\n");
    printf("+-----------------------------------------------------------+\n");
    printf("                       INSTRUCTIONS                          \n");
    printf(" You will need to enter the morse code of the character given\n");
    printf("        you are also given the morse code                    \n");
    printf("+-----------------------------------------------------------+\n");
    printf("                  Given character    :  %c                   \n", alphanumeric[var].word);
    printf("+-----------------------------------------------------------+\n");
    main_asm();
    printf("%s\n", input_array);
    comp = strcmp(input_array, compar);
    if (comp == 0)
    {
        streak++;
        printf("You wrote the correct sequence. YOUR STREAK : %d\n", streak);
        if (life < 3)
        {
            life++;
            printf("Number of lives left                        : %d\n", life);
            rgb_life(life);
        }
        if (streak < 5)
        {
            level2();
        }
        else
        {
            printf("+-----------------------------------------------------------+\n");
            printf(":                    YOU WON !!!                            :\n");
            printf("+                    YOUR STATS:                            +\n");
            stats();
        }
    }
    else
    {

        life--;
        printf("Number of lives left                        : %d\n", life);
        rgb_life(life);
        streak = 0;
        printf("You wrote the wrong sequence. YOUR STREAK : %d\n", streak);
        if (life == 0)
        {
            printf("+-----------------------------------------------------------+\n");
            printf(":       GAME OVER, WAIT FOR 8 SECONDS TO RESTART            :\n");
            printf("+-----------------------------------------------------------+\n");
        }
        else
        {
            printf("+-----------------------------------------------------------+\n");
            printf(":             WRONG ANSWER, TRY AGAIN                       :\n");
            printf("+-----------------------------------------------------------+\n");

            level2();
        }
    }
}

/**
 * @brief This function prints the stats of the user. It shows the attempts, right answers, wrong answers and the accuracy
 *
 */
void stats()
{
    printf("+-----------------------------------------------------------+\n");
    printf(":                        STATS                              :\n");
    printf("+-----------------------------------------------------------+\n");
    total = wrongAns + rightAns;
    printf("\n NUMBER OF ATTEMPTS    :           %d                      \n", total);
    printf("\n NUMBER OF RIGHT ANSWER:           %d                      \n", rightAns);
    printf("\n NUMBER OF WRONG ANSWER:           %d                      \n", wrongAns);
    accuracy = (rightAns / (wrongAns + rightAns)) * 100;
    printf("\n YOUR ACCURACY         :           %d                      \n", accuracy);
    printf("+-----------------------------------------------------------+\n");
}

/**
 * @brief This function checks which level is inputted by user
 *
 */

void start()
{

    main_asm();
    // cur_i = 0;
    if (strcmp(input_array, "-----") == 0)
    {
        set_rgb_green();
        level1();
    }
    else if (strcmp(input_array, ".----") == 0)
    {
        set_rgb_green();
        level2();
    }
    else
    {
        printf("Wrong input\n");
        return;
    }
}

/**
 * @brief This function takes the input from the user through the assembly
 *
 * @param sel
 * @param index
 */
void asm_arr(int sel, int index)
{
    if (sel == 1)
    {
        input_array[index] = '.';
    }
    else if (sel == 2)
    {
        input_array[index] = '-';
    }
    else if (sel == 3)
    {
        input_array[index] = '\0';
    }
}
// void asm_arr(int let,int ind)
/**
 * @brief Wrapper to allow the assembly code to call the gpio_init()
 *        SDK function.
 *
 * @param pin       The GPIO pin number to initialise.
 */
void asm_gpio_init(int pin)
{
    gpio_init(pin);
}

/**
 * @brief Wrapper to allow the assembly code to call the gpio_set_dir()
 *        SDK function.
 *
 * @param pin       The GPIO pin number of which to set the direction.
 * @param dir       Specify the direction that the pin should be set to (0=input/1=output).
 */
void asm_gpio_set_dir(int pin, int dir)
{
    gpio_set_dir(pin, dir);
}

int asm_gpio_get(int pin)
{
    return gpio_get(pin);
}

/**
 * @brief Wrapper to allow the assembly code to call the gpio_put()
 *        SDK function.
 *
 * @param pin       The GPIO pin number to write to.
 * @param value     Specify the value that the pin should be set to (0/1).
 */
void asm_gpio_put(int pin, int value)
{
    gpio_put(pin, value);
}

/**
 * @brief Wrapper to allow the assembly code to easily setup interrupt
 * detection for falling edge events for a specific GPIO pin.
 *
 * @param pin The GPIO pin number to setup the interrupts on.
 */
void asm_gpio_set_irq_f(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
}
void asm_gpio_set_irq_r(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
}

/**
 * @brief This function is the main entry point of the program
 *
 * @return int
 */
int main()
{
    // Initialise all STDIO as we will be using the GPIOs
    stdio_init_all();
    Welcome_StartMessage();
    // Initialise the PIO interface with the WS2812 code
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    set_rgb_blue();
    watchdog_enable(8000000, 1);

    input_morse();

    for (int i = 0; i < 5; i++)
    {

        start();
    }

    return 0;
}
