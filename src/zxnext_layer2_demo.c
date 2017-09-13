/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 screen demo program for Sinclair ZX Spectrum Next.
 *
 * zcc +zx -vn -SO3 -startup=30 -clib=sdcc_iy --max-allocs-per-node200000
 *   -L<zxnext_layer2>/lib/sdcc_iy -lzxnext_layer2 -I<zxnext_layer2>/include
 *   zxnext_layer2_demo.c -o zxnext_layer2_demo -create-app [-Cz"--sna"]
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

#pragma output CRT_ORG_CODE = 0x6164
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

static void flip_main_shadow_screen(void);

static void test_clear_screen(layer2_screen_t *screen);

static void test_clear_screen_many(void);

static void test_load_screen(layer2_screen_t *screen);

static void test_load_screen_many(void);

static void test_draw_pixel(layer2_screen_t *screen);

static void test_draw_pixel_many(void);

static void test_draw_line(layer2_screen_t *screen);

static void test_draw_line_many(void);

static void test_draw_rect(layer2_screen_t *screen);

static void test_draw_rect_many(void);

static void test_fill_rect(layer2_screen_t *screen);

static void test_fill_rect_many(void);

static void test_draw_text(layer2_screen_t *screen);

static void test_draw_text_many(void);

static void test_blit(layer2_screen_t *screen);

static void test_blit_many(void);

static void test_blit_transparent(layer2_screen_t *screen);

static void test_blit_transparent_many(void);

static void test_scroll_screen_horizontally(void);

static void test_scroll_screen_vertically(void);

static void test_scroll_screen_diagonally(void);

static void test_scroll_multi_screen_horizontally(void);

static void test_scroll_multi_screen_vertically(void);

static void test_layer2_over_ula(void);

static void test_ula_over_layer2(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

static layer2_screen_t shadow_screen = {SHADOW_SCREEN};

static layer2_screen_t off_screen = {OFF_SCREEN, 0, 1, 3};

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
            test_clear_screen(&shadow_screen);
            break;
        case 10:
            test_load_screen(&shadow_screen);
            break;
        case 11:
            test_draw_pixel(&shadow_screen);
            break;
        case 12:
            test_draw_line(&shadow_screen);
            break;
        case 13:
            test_draw_rect(&shadow_screen);
            break;
        case 14:
            test_fill_rect(&shadow_screen);
            break;
        case 15:
            test_draw_text(&shadow_screen);
            break;
        case 16:
            test_blit(&shadow_screen);
            break;
        case 17:
            test_blit_transparent(&shadow_screen);
            break;
        case 18:
            test_clear_screen(&off_screen);
            break;
        case 19:
            test_load_screen(&off_screen);
            break;
        case 20:
            test_draw_pixel(&off_screen);
            break;
        case 21:
            test_draw_line(&off_screen);
            break;
        case 22:
            test_draw_rect(&off_screen);
            break;
        case 23:
            test_fill_rect(&off_screen);
            break;
        case 24:
            test_draw_text(&off_screen);
            break;
        case 25:
            test_blit(&off_screen);
            break;
        case 26:
            test_blit_transparent(&off_screen);
            break;
        case 27:
            test_clear_screen_many();
            break;
        case 28:
            test_load_screen_many();
            break;
        case 29:
            test_draw_pixel_many();
            break;
        case 30:
            test_draw_line_many();
            break;
        case 31:
            test_draw_rect_many();
            break;
        case 32:
            test_fill_rect_many();
            break;
        case 33:
            test_draw_text_many();
            break;
        case 34:
            test_blit_many();
            break;
        case 35:
            test_blit_transparent_many();
            break;
        case 36:
            test_scroll_screen_horizontally();
            break;
        case 37:
            test_scroll_screen_vertically();
            break;
        case 38:
            test_scroll_screen_diagonally();
            break;
        case 39:
            test_scroll_multi_screen_horizontally();
            break;
        case 40:
            test_scroll_multi_screen_vertically();
            break;
        case 41:
            test_layer2_over_ula();
            break;
        case 42:
            test_ula_over_layer2();
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 43;
}

static void flip_main_shadow_screen(void)
{
    static bool flip = false;

    if (flip)
    {
        layer2_set_main_screen_ram_bank(8);
        layer2_set_shadow_screen_ram_bank(11);
    }
    else
    {
        layer2_set_main_screen_ram_bank(11);
        layer2_set_shadow_screen_ram_bank(8);
    }

    flip = !flip;
}

static void test_clear_screen(layer2_screen_t *screen)
{
    layer2_clear_screen(0xDB, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
    }
}

static void test_clear_screen_many(void)
{
    while (!in_inkey())
    {
        layer2_clear_screen(rand() % 256, NULL);
    }
}

static void test_load_screen(layer2_screen_t *screen)
{
    layer2_load_screen("screen1.nxi", screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_draw_pixel(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_pixel(0,   0,  0x03, screen);
    layer2_draw_pixel(127, 0,  0x03, screen);
    layer2_draw_pixel(255, 0,  0x03, screen);
    layer2_draw_pixel(0,   31, 0x03, screen);
    layer2_draw_pixel(127, 31, 0x03, screen);
    layer2_draw_pixel(255, 31, 0x03, screen);
    layer2_draw_pixel(0,   63, 0x03, screen);
    layer2_draw_pixel(127, 63, 0x03, screen);
    layer2_draw_pixel(255, 63, 0x03, screen);

    layer2_draw_pixel(0,   64,  0xE0, screen);
    layer2_draw_pixel(127, 64,  0xE0, screen);
    layer2_draw_pixel(255, 64,  0xE0, screen);
    layer2_draw_pixel(0,   95,  0xE0, screen);
    layer2_draw_pixel(127, 95,  0xE0, screen);
    layer2_draw_pixel(255, 95,  0xE0, screen);
    layer2_draw_pixel(0,   127, 0xE0, screen);
    layer2_draw_pixel(127, 127, 0xE0, screen);
    layer2_draw_pixel(255, 127, 0xE0, screen);

    layer2_draw_pixel(0,   128, 0x83, screen);
    layer2_draw_pixel(127, 128, 0x83, screen);
    layer2_draw_pixel(255, 128, 0x83, screen);
    layer2_draw_pixel(0,   159, 0x83, screen);
    layer2_draw_pixel(127, 159, 0x83, screen);
    layer2_draw_pixel(255, 159, 0x83, screen);
    layer2_draw_pixel(0,   191, 0x83, screen);
    layer2_draw_pixel(127, 191, 0x83, screen);
    layer2_draw_pixel(255, 191, 0x83, screen);

    // Outside screen.
    layer2_draw_pixel(255, 255, 0x83, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_draw_line(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_line(0,   0,   255, 0,   0x6F, screen);
    layer2_draw_line(0,   191, 255, 191, 0x6F, screen);
    layer2_draw_line(0,   0,   0,   191, 0x6F, screen);
    layer2_draw_line(255, 0,   255, 191, 0x6F, screen);

    layer2_draw_line(0,   0, 255, 191, 0xE0, screen);
    layer2_draw_line(255, 0, 0,   191, 0xE0, screen);

    // Outside screen.
    layer2_draw_line(127, 0, 255, 255, 0xE0, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_draw_rect(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_rect(0, 0, 256, 192, 0x6F, screen);

    layer2_draw_rect(118, 22,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 54,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 86,  20, 20, 0xE0, screen);
    layer2_draw_rect(118, 118, 20, 20, 0xE0, screen);
    layer2_draw_rect(118, 150, 20, 20, 0xE0, screen);
    layer2_draw_rect(118, 182, 20, 20, 0xE0, screen);

    // Clipped rectangles.
    layer2_draw_rect(246, 54,  20, 20, 0xEF, screen);
    layer2_draw_rect(246, 118, 20, 20, 0xEF, screen);
    layer2_draw_rect(246, 182, 20, 20, 0xEF, screen);

    // Outside screen.
    layer2_draw_rect(246, 246, 20, 20, 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_fill_rect(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_fill_rect(118, 22,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 54,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 86,  20, 20, 0xE0, screen);
    layer2_fill_rect(118, 118, 20, 20, 0xE0, screen);
    layer2_fill_rect(118, 150, 20, 20, 0xE0, screen);
    layer2_fill_rect(118, 182, 20, 20, 0xE0, screen);

    // Clipped rectangles.
    layer2_fill_rect(246, 54,  20, 20, 0xEF, screen);
    layer2_fill_rect(246, 118, 20, 20, 0xEF, screen);
    layer2_fill_rect(246, 182, 20, 20, 0xEF, screen);

    // Outside screen.
    layer2_fill_rect(246, 246, 20, 20, 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_draw_text(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_draw_text(3,  12, "Hello", 0x00, screen);
    layer2_set_font(NULL);
    layer2_draw_text(11, 12, "Hello", 0x00, screen);
    layer2_set_font((void *) 0x3D00);
    layer2_draw_text(19, 12, "Hello", 0x00, screen);

    // Clipped text.
    layer2_draw_text(3,  29, "Hello", 0xEF, screen);
    layer2_draw_text(11, 29, "Hello", 0xEF, screen);
    layer2_draw_text(19, 29, "Hello", 0xEF, screen);

    // Outside screen.
    layer2_draw_text(24, 32, "Hello", 0xEF, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_blit(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_blit(120, 24,  sprite, 16, 16, screen); // top
    layer2_blit(120, 56,  sprite, 16, 16, screen); // top + middle
    layer2_blit(120, 88,  sprite, 16, 16, screen); // middle
    layer2_blit(120, 120, sprite, 16, 16, screen); // middle + bottom
    layer2_blit(120, 152, sprite, 16, 16, screen); // bottom
    layer2_blit(120, 184, sprite, 16, 16, screen); // bottom clipped

    layer2_blit(64, 48, tall_sprite, 2, 96, screen); // top + middle + bottom

    // Clipped blits.
    layer2_blit(248, 56,  sprite, 16, 16, screen);
    layer2_blit(248, 120, sprite, 16, 16, screen);
    layer2_blit(248, 184, sprite, 16, 16, screen);

    // Outside screen.
    layer2_blit(248, 248, sprite, 16, 16, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_blit_transparent(layer2_screen_t *screen)
{
    layer2_fill_rect(0, 0,   256, 64, 0xFE, screen);
    layer2_fill_rect(0, 64,  256, 64, 0x7E, screen);
    layer2_fill_rect(0, 128, 256, 64, 0x9F, screen);

    layer2_blit_transparent(120, 24,  sprite, 16, 16, screen); // top
    layer2_blit_transparent(120, 56,  sprite, 16, 16, screen); // top + middle
    layer2_blit_transparent(120, 88,  sprite, 16, 16, screen); // middle
    layer2_blit_transparent(120, 120, sprite, 16, 16, screen); // middle + bottom
    layer2_blit_transparent(120, 152, sprite, 16, 16, screen); // bottom
    layer2_blit_transparent(120, 184, sprite, 16, 16, screen); // bottom clipped

    layer2_blit_transparent(64, 48, tall_sprite, 2, 96, screen); // top + middle + bottom

    // Clipped blits.
    layer2_blit_transparent(248, 56,  sprite, 16, 16, screen);
    layer2_blit_transparent(248, 120, sprite, 16, 16, screen);
    layer2_blit_transparent(248, 184, sprite, 16, 16, screen);

    // Outside screen.
    layer2_blit_transparent(248, 248, sprite, 16, 16, screen);

    if (IS_SHADOW_SCREEN(screen))
    {
        flip_main_shadow_screen();
    }
    else if (IS_OFF_SCREEN(screen))
    {
        layer2_copy_off_screen(screen);
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

static void test_scroll_screen_horizontally(void)
{
    uint8_t offset_x = 0;
    bool increment = true;

    layer2_load_screen("screen1.nxi", NULL);

    // 0, 1, 2, ..., 254, 255, 0, 255, 254, ..., 2, 1, 0, ...

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment)
        {
            offset_x++;
        }
        else
        {
            offset_x--;
        }

        layer2_set_offset_x(offset_x);

        if (offset_x == 0)
        {
            increment = !increment;
        }
    }

    layer2_set_offset_x(0);
}

static void test_scroll_screen_vertically(void)
{
    uint8_t offset_y = 0;
    bool increment = true;

    layer2_load_screen("screen1.nxi", NULL);

    // 0, 1, 2, ..., 190, 191, 0, 191, 190, ..., 2, 1, 0, ...

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment)
        {
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
        }

        layer2_set_offset_y(offset_y);

        if (offset_y == 0)
        {
            increment = !increment;
        }
    }

    layer2_set_offset_y(0);
}

static void test_scroll_screen_diagonally(void)
{
    uint8_t offset_x = 0;
    uint8_t offset_y = 0;
    bool increment_x = true;
    bool increment_y = true;

    layer2_load_screen("screen1.nxi", NULL);

    while (!in_inkey())
    {
        intrinsic_halt();

        if (increment_x)
        {
            offset_x++;
        }
        else
        {
            offset_x--;
        }

        if (increment_y)
        {
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
        }

        layer2_set_offset_x(offset_x);
        layer2_set_offset_y(offset_y);

        if (offset_x == 0)
        {
            increment_x = !increment_x;
        }

        if (offset_y == 0)
        {
            increment_y = !increment_y;
        }
    }

    layer2_set_offset_x(0);
    layer2_set_offset_y(0);
}

/*
 * Scroll horizontally between three screens numbered 0, 1 and 2 with the file
 * names wide1.nxi, wide2.nxi and wide3.nxi. The first screen file is loaded
 * into the main layer 2 screen. The screen file to be scrolled in is loaded
 * into an off-screen buffer. When the screen being scrolled in is completely
 * scrolled in, the next screen file to be scrolled in is loaded into the off-
 * screen buffer. When the last screen is completely scrolled in, the scroll
 * direction is reversed. The actual scrolling is performed in the vertical
 * blanking interval and is done by shifting the columns of the main screen by
 * one pixel and blitting the corresponding column from the off-screen buffer of
 * the screen being scrolled in.
 */
static void test_scroll_multi_screen_horizontally(void)
{
    static const char *screen_files[3] = {"wide1.nxi", "wide2.nxi", "wide3.nxi"};
    uint8_t next_screen_num = 0;
    uint8_t offset_x = 0;
    uint8_t fill_x;
    bool increment = true;

    layer2_load_screen(screen_files[next_screen_num], NULL);

    while (!in_inkey())
    {
        if (offset_x == 0)
        {
            next_screen_num = increment ? next_screen_num + 1 : next_screen_num - 1;
            layer2_load_screen(screen_files[next_screen_num], &off_screen);
        }

        intrinsic_halt();

        if (increment)
        {
            fill_x = offset_x;
            offset_x++;
        }
        else
        {
            offset_x--;
            fill_x = offset_x;
        }

        layer2_set_offset_x(offset_x);
        layer2_blit_off_screen_column(fill_x, &off_screen, fill_x);

        if (offset_x == 0)
        {
            if ((next_screen_num == 0) || (next_screen_num == 2))
            {
                increment = !increment;
            }
        }
    }

    layer2_set_offset_x(0);
}

/*
 * Scroll vertically between three screens numbered 0, 1 and 2 with the file
 * names tall1.nxi, tall2.nxi and tall3.nxi. The first screen file is loaded
 * into the main layer 2 screen. The screen file to be scrolled in is loaded
 * into an off-screen buffer. When the screen being scrolled in is completely
 * scrolled in, the next screen file to be scrolled in is loaded into the off-
 * screen buffer. When the last screen is completely scrolled in, the scroll
 * direction is reversed. The actual scrolling is performed in the vertical
 * blanking interval and is done by shifting the rows of the main screen by one
 * pixel and blitting the corresponding row from the off-screen buffer of the
 * screen being scrolled in.
 */
static void test_scroll_multi_screen_vertically(void)
{
    static const char *screen_files[3] = {"tall1.nxi", "tall2.nxi", "tall3.nxi"};
    uint8_t next_screen_num = 0;
    uint8_t offset_y = 0;
    uint8_t fill_y;
    bool increment = true;

    layer2_load_screen(screen_files[next_screen_num], NULL);

    while (!in_inkey())
    {
        if (offset_y == 0)
        {
            next_screen_num = increment ? next_screen_num + 1 : next_screen_num - 1;
            layer2_load_screen(screen_files[next_screen_num], &off_screen);
        }

        intrinsic_halt();

        if (increment)
        {
            fill_y = offset_y;
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
            fill_y = offset_y;
        }

        layer2_set_offset_y(offset_y);
        layer2_blit_off_screen_row(fill_y, &off_screen, fill_y);

        if (offset_y == 0)
        {
            if ((next_screen_num == 0) || (next_screen_num == 2))
            {
                increment = !increment;
            }
        }
    }

    layer2_set_offset_y(0);
}

static void test_layer2_over_ula(void)
{
    // By default, the layer 2 screen is over the ULA screen.
    // Draw a filled rectangle with the transparency colour in the middle
    // of the layer 2 screen so the underlying ULA screen shows through.

    zx_cls(INK_BLACK | PAPER_WHITE);
    printAt(12, 11, "ULA SCREEN");

    layer2_clear_screen(0x37, NULL);
    layer2_draw_text(5, 9, "LAYER 2 SCREEN", 0x88, NULL);
    layer2_fill_rect(64, 80, 128, 40, 0xE3, NULL);

    in_wait_key();
    // Reset to default settings.
    zx_cls(INK_BLACK | PAPER_WHITE);
}

static void test_ula_over_layer2(void)
{
    struct r_Rect8 rect = {8, 16, 10, 5};

    // Set the ULA screen to be over the layer 2 screen.
    // Draw a filled rectangle with the transparency colour in the middle
    // of the ULA screen so the underlying layer 2 screen shows through.

    // FIXME: I can only get ULA transparency to work when
    // the transparency colour is set to black (0x00)...

    zx_cls(INK_BLACK | PAPER_WHITE);
    printAt(5, 11, "ULA SCREEN");
    zx_cls_wc_attr(&rect, PAPER_BLACK);

    layer2_clear_screen(0x37, NULL);
    layer2_draw_text(12, 9, "LAYER 2 SCREEN", 0x88, NULL);

    layer2_set_layer_priorities(LAYER_PRIORITIES_S_U_L);
    layer2_set_global_transparency_color(0x00);

    in_wait_key();
    // Reset to default settings.
    zx_cls(INK_BLACK | PAPER_WHITE);
    layer2_set_layer_priorities(LAYER_PRIORITIES_S_L_U);
    layer2_set_global_transparency_color(0xE3);
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
