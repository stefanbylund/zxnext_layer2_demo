/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 screen demo program for Sinclair ZX Spectrum Next.
 ******************************************************************************/

#include <arch/zx.h>
#include <input.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zxnext_registers.h"
#include "zxnext_layer2.h"

// FIXME: CRT_ORG_CODE = 0x6164 doesn't work for snapshot images...
#pragma output CRT_ORG_CODE = 0x6200
#pragma output REGISTER_SP = 0xBFFF
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_FOPEN_MAX = -1

#define printAt(row, col, str) printf("\x16%c%c%s", (row), (col), (str))

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

static void init_hardware(void);

static void init_isr(void);

static void create_start_screen(void);

static void init_tests(void);

static void select_test(void);

static void test_clear_screen(off_screen_buffer_t *off_screen_buffer);

static void test_clear_screen_many(void);

static void test_load_screen(off_screen_buffer_t *off_screen_buffer);

static void test_load_screen_many(void);

static void test_draw_pixel(off_screen_buffer_t *off_screen_buffer);

static void test_draw_pixel_many(void);

static void test_draw_line(off_screen_buffer_t *off_screen_buffer);

static void test_draw_line_many(void);

static void test_draw_rect(off_screen_buffer_t *off_screen_buffer);

static void test_draw_rect_many(void);

static void test_fill_rect(off_screen_buffer_t *off_screen_buffer);

static void test_fill_rect_many(void);

static void test_draw_text(off_screen_buffer_t *off_screen_buffer);

static void test_draw_text_many(void);

static void test_blit(off_screen_buffer_t *off_screen_buffer);

static void test_blit_many(void);

static void test_blit_transparent(off_screen_buffer_t *off_screen_buffer);

static void test_blit_transparent_many(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

static off_screen_buffer_t off_screen_buffer = {0, 1, 3};

static uint8_t tall_sprite[192];

static const uint8_t sprite[] =
{
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xFB, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xF5, 0xF5, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFB, 0xF5, 0xA8, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0xFF, 0xFF, 0xFB, 0xA8, 0x44, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0x04, 0x04, 0x04, 0xFF, 0xFB, 0xA8, 0x44, 0xA8, 0xFB, 0xFF, 0x04, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0xA8, 0x44, 0x44, 0xFB, 0xFF, 0x04, 0xE3, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0x44, 0x44, 0xFB, 0xFF, 0x04, 0x4D, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0x44, 0x44, 0x44, 0xFA, 0x4D, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0xFB, 0x44, 0xFF, 0xF5, 0x44, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFF, 0x44, 0xF5, 0xA8, 0x04, 0xE3, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0xFA, 0x44, 0x04, 0xA8, 0x04, 0xE3, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x4D, 0x4D, 0x04, 0xE3, 0x04, 0xF5, 0x04, 0xE3,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x04, 0xE3, 0xE3, 0xE3, 0x04, 0xFA, 0x04,
    0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x04, 0x04
};

/*******************************************************************************
 * Functions
 ******************************************************************************/

static void init_hardware(void)
{
    uint8_t peripheral_2_settings;

    // Put Z80 in 7 MHz turbo mode.

    z80_outp(REGISTER_NUMBER_PORT, PERIPHERAL_2_REGISTER);
    peripheral_2_settings = z80_inp(REGISTER_VALUE_PORT);

    z80_outp(REGISTER_NUMBER_PORT, PERIPHERAL_2_REGISTER);
    z80_outp(REGISTER_VALUE_PORT, 0x80 | peripheral_2_settings);

    z80_outp(REGISTER_NUMBER_PORT, TURBO_MODE_REGISTER);
    z80_outp(REGISTER_VALUE_PORT, 1);
}

static void init_isr(void)
{
    // Set up IM2 interrupt service routine:
    // Put Z80 in IM2 mode with a 257-byte interrupt vector table located
    // at 0x6000 (before CRT_ORG_CODE) filled with 0x61 bytes. Install an
    // empty interrupt service routine at the interrupt service routine
    // entry at address 0x6161.

    intrinsic_di();
    im2_init((void *) 0x6000);
    memset((void *) 0x6000, 0x61, 257);
    z80_bpoke(0x6161, 0xFB);
    z80_bpoke(0x6162, 0xED);
    z80_bpoke(0x6163, 0x4D);
    intrinsic_ei();
}

static void create_start_screen(void)
{
    zx_border(INK_WHITE);
    zx_cls(INK_BLACK | PAPER_WHITE);

    printAt(7,  5, "Press any key to start");
    printAt(15, 1, "Press any key to switch screen");
}

static void init_tests(void)
{
    zx_border(INK_YELLOW);
    zx_cls(INK_BLACK | PAPER_WHITE);

    memset(tall_sprite, 0x27, 192);
    layer2_configure(true, false, false, 0);
}

static void select_test(void)
{
    switch (test_number)
    {
        case 0:
            test_clear_screen(NULL);
            break;
        case 1:
            test_load_screen(NULL);
            break;
        case 2:
            test_draw_pixel(NULL);
            break;
        case 3:
            test_draw_line(NULL);
            break;
        case 4:
            test_draw_rect(NULL);
            break;
        case 5:
            test_fill_rect(NULL);
            break;
        case 6:
            test_draw_text(NULL);
            break;
        case 7:
            test_blit(NULL);
            break;
        case 8:
            test_blit_transparent(NULL);
            break;
        case 9:
            test_clear_screen(&off_screen_buffer);
            break;
        case 10:
            test_load_screen(&off_screen_buffer);
            break;
        case 11:
            test_draw_pixel(&off_screen_buffer);
            break;
        case 12:
            test_draw_line(&off_screen_buffer);
            break;
        case 13:
            test_draw_rect(&off_screen_buffer);
            break;
        case 14:
            test_fill_rect(&off_screen_buffer);
            break;
        case 15:
            test_draw_text(&off_screen_buffer);
            break;
        case 16:
            test_blit(&off_screen_buffer);
            break;
        case 17:
            test_blit_transparent(&off_screen_buffer);
            break;
        case 18:
            test_clear_screen_many();
            break;
        case 19:
            test_load_screen_many();
            break;
        case 20:
            test_draw_pixel_many();
            break;
        case 21:
            test_draw_line_many();
            break;
        case 22:
            test_draw_rect_many();
            break;
        case 23:
            test_fill_rect_many();
            break;
        case 24:
            test_draw_text_many();
            break;
        case 25:
            test_blit_many();
            break;
        case 26:
            test_blit_transparent_many();
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 27;
}

static void test_clear_screen(off_screen_buffer_t *off_screen_buffer)
{
    layer2_clear_screen(0xDB, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_clear_screen_many(void)
{
    while (!in_inkey())
    {
        layer2_clear_screen(rand() % 256, NULL);
    }
}

static void test_load_screen(off_screen_buffer_t *off_screen_buffer)
{
    layer2_load_screen("screen1.nxi", off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_load_screen_many(void)
{
    bool toggle = true;

    while (!in_inkey())
    {
        layer2_load_screen(toggle ? "screen1.nxi" : "screen2.nxi", NULL);
        toggle = !toggle;
        // A short delay so we have time to actually see the screen.
        z80_delay_ms(500);
    }
}

static void test_draw_pixel(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_draw_pixel(0,   0,  0x03, off_screen_buffer);
    layer2_draw_pixel(127, 0,  0x03, off_screen_buffer);
    layer2_draw_pixel(255, 0,  0x03, off_screen_buffer);
    layer2_draw_pixel(0,   31, 0x03, off_screen_buffer);
    layer2_draw_pixel(127, 31, 0x03, off_screen_buffer);
    layer2_draw_pixel(255, 31, 0x03, off_screen_buffer);
    layer2_draw_pixel(0,   63, 0x03, off_screen_buffer);
    layer2_draw_pixel(127, 63, 0x03, off_screen_buffer);
    layer2_draw_pixel(255, 63, 0x03, off_screen_buffer);

    layer2_draw_pixel(0,   64,  0xE0, off_screen_buffer);
    layer2_draw_pixel(127, 64,  0xE0, off_screen_buffer);
    layer2_draw_pixel(255, 64,  0xE0, off_screen_buffer);
    layer2_draw_pixel(0,   95,  0xE0, off_screen_buffer);
    layer2_draw_pixel(127, 95,  0xE0, off_screen_buffer);
    layer2_draw_pixel(255, 95,  0xE0, off_screen_buffer);
    layer2_draw_pixel(0,   127, 0xE0, off_screen_buffer);
    layer2_draw_pixel(127, 127, 0xE0, off_screen_buffer);
    layer2_draw_pixel(255, 127, 0xE0, off_screen_buffer);

    layer2_draw_pixel(0,   128, 0x83, off_screen_buffer);
    layer2_draw_pixel(127, 128, 0x83, off_screen_buffer);
    layer2_draw_pixel(255, 128, 0x83, off_screen_buffer);
    layer2_draw_pixel(0,   159, 0x83, off_screen_buffer);
    layer2_draw_pixel(127, 159, 0x83, off_screen_buffer);
    layer2_draw_pixel(255, 159, 0x83, off_screen_buffer);
    layer2_draw_pixel(0,   191, 0x83, off_screen_buffer);
    layer2_draw_pixel(127, 191, 0x83, off_screen_buffer);
    layer2_draw_pixel(255, 191, 0x83, off_screen_buffer);

    // Outside screen.
    layer2_draw_pixel(255, 255, 0x83, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_draw_pixel_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_draw_pixel(rand() % 256, rand() % 192, rand() % 256, NULL);
    }
}

static void test_draw_line(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_draw_line(0,   0,   255, 0,   0x6F, off_screen_buffer);
    layer2_draw_line(0,   191, 255, 191, 0x6F, off_screen_buffer);
    layer2_draw_line(0,   0,   0,   191, 0x6F, off_screen_buffer);
    layer2_draw_line(255, 0,   255, 191, 0x6F, off_screen_buffer);

    layer2_draw_line(0,   0, 255, 191, 0xE0, off_screen_buffer);
    layer2_draw_line(255, 0, 0,   191, 0xE0, off_screen_buffer);

    // Outside screen.
    layer2_draw_line(127, 0, 255, 255, 0xE0, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_draw_line_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_draw_line(rand() % 256, rand() % 192, rand() % 256, rand() % 192, rand() % 256, NULL);
    }
}

static void test_draw_rect(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_draw_rect(0, 0, 256, 192, 0x6F, off_screen_buffer);

    layer2_draw_rect(118, 22,  20, 20, 0xE0, off_screen_buffer);
    layer2_draw_rect(118, 54,  20, 20, 0xE0, off_screen_buffer);
    layer2_draw_rect(118, 86,  20, 20, 0xE0, off_screen_buffer);
    layer2_draw_rect(118, 118, 20, 20, 0xE0, off_screen_buffer);
    layer2_draw_rect(118, 150, 20, 20, 0xE0, off_screen_buffer);
    layer2_draw_rect(118, 182, 20, 20, 0xE0, off_screen_buffer);

    // Clipped rectangles.
    layer2_draw_rect(246, 54,  20, 20, 0xEF, off_screen_buffer);
    layer2_draw_rect(246, 118, 20, 20, 0xEF, off_screen_buffer);
    layer2_draw_rect(246, 182, 20, 20, 0xEF, off_screen_buffer);

    // Outside screen.
    layer2_draw_rect(246, 246, 20, 20, 0xEF, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_draw_rect_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_draw_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
    }
}

static void test_fill_rect(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_fill_rect(118, 22,  20, 20, 0xE0, off_screen_buffer);
    layer2_fill_rect(118, 54,  20, 20, 0xE0, off_screen_buffer);
    layer2_fill_rect(118, 86,  20, 20, 0xE0, off_screen_buffer);
    layer2_fill_rect(118, 118, 20, 20, 0xE0, off_screen_buffer);
    layer2_fill_rect(118, 150, 20, 20, 0xE0, off_screen_buffer);
    layer2_fill_rect(118, 182, 20, 20, 0xE0, off_screen_buffer);

    // Clipped rectangles.
    layer2_fill_rect(246, 54,  20, 20, 0xEF, off_screen_buffer);
    layer2_fill_rect(246, 118, 20, 20, 0xEF, off_screen_buffer);
    layer2_fill_rect(246, 182, 20, 20, 0xEF, off_screen_buffer);

    // Outside screen.
    layer2_fill_rect(246, 246, 20, 20, 0xEF, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_fill_rect_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_fill_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
    }
}

static void test_draw_text(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_draw_text(3,  12, "Hello", 0x00, off_screen_buffer);
    layer2_set_font(NULL);
    layer2_draw_text(11, 12, "Hello", 0x00, off_screen_buffer);
    layer2_set_font((void *) 0x3D00);
    layer2_draw_text(19, 12, "Hello", 0x00, off_screen_buffer);

    // Clipped text.
    layer2_draw_text(3,  29, "Hello", 0xEF, off_screen_buffer);
    layer2_draw_text(11, 29, "Hello", 0xEF, off_screen_buffer);
    layer2_draw_text(19, 29, "Hello", 0xEF, off_screen_buffer);

    // Outside screen.
    layer2_draw_text(24, 32, "Hello", 0xEF, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_draw_text_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_draw_text(rand() % 24, rand() % 32, "Hello", rand() % 256, NULL);
    }
}

static void test_blit(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_blit(120, 24,  sprite, 16, 16, off_screen_buffer); // top
    layer2_blit(120, 56,  sprite, 16, 16, off_screen_buffer); // top + middle
    layer2_blit(120, 88,  sprite, 16, 16, off_screen_buffer); // middle
    layer2_blit(120, 120, sprite, 16, 16, off_screen_buffer); // middle + bottom
    layer2_blit(120, 152, sprite, 16, 16, off_screen_buffer); // bottom
    layer2_blit(120, 184, sprite, 16, 16, off_screen_buffer); // bottom clipped

    layer2_blit(64, 48, tall_sprite, 2, 96, off_screen_buffer); // top + middle + bottom

    // Clipped blits.
    layer2_blit(248, 56,  sprite, 16, 16, off_screen_buffer);
    layer2_blit(248, 120, sprite, 16, 16, off_screen_buffer);
    layer2_blit(248, 184, sprite, 16, 16, off_screen_buffer);

    // Outside screen.
    layer2_blit(248, 248, sprite, 16, 16, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_blit_many(void)
{
    layer2_clear_screen(0xFF, NULL);

    while (!in_inkey())
    {
        layer2_blit(rand() % 256, rand() % 192, sprite, 16, 16, NULL);
    }
}

static void test_blit_transparent(off_screen_buffer_t *off_screen_buffer)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, off_screen_buffer);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, off_screen_buffer);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, off_screen_buffer);

    layer2_blit_transparent(120, 24,  sprite, 16, 16, off_screen_buffer); // top
    layer2_blit_transparent(120, 56,  sprite, 16, 16, off_screen_buffer); // top + middle
    layer2_blit_transparent(120, 88,  sprite, 16, 16, off_screen_buffer); // middle
    layer2_blit_transparent(120, 120, sprite, 16, 16, off_screen_buffer); // middle + bottom
    layer2_blit_transparent(120, 152, sprite, 16, 16, off_screen_buffer); // bottom
    layer2_blit_transparent(120, 184, sprite, 16, 16, off_screen_buffer); // bottom clipped

    layer2_blit_transparent(64, 48, tall_sprite, 2, 96, off_screen_buffer); // top + middle + bottom

    // Clipped blits.
    layer2_blit_transparent(248, 56,  sprite, 16, 16, off_screen_buffer);
    layer2_blit_transparent(248, 120, sprite, 16, 16, off_screen_buffer);
    layer2_blit_transparent(248, 184, sprite, 16, 16, off_screen_buffer);

    // Outside screen.
    layer2_blit_transparent(248, 248, sprite, 16, 16, off_screen_buffer);

    if (off_screen_buffer != NULL)
    {
        layer2_copy_off_screen(off_screen_buffer);
    }
}

static void test_blit_transparent_many(void)
{
    layer2_clear_screen(0xFF, NULL);

    while (!in_inkey())
    {
        layer2_blit_transparent(rand() % 256, rand() % 192, sprite, 16, 16, NULL);
    }
}

int main(void)
{
    init_hardware();
    init_isr();

    create_start_screen();
    in_wait_key();
    init_tests();

    while (true)
    {
        if (in_inkey())
        {
            in_wait_nokey();
            select_test();
        }
    }

    //return 0;
}
