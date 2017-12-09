/*******************************************************************************
 * Stefan Bylund 2017
 *
 * A layer 2 drawing performance demo program for Sinclair ZX Spectrum Next.
 *
 * zcc +zx [-subtype=sna] -vn -SO3 -startup=30 -clib=sdcc_iy
 *   --max-allocs-per-node200000 -L<zxnext_layer2>/lib/sdcc_iy -lzxnext_layer2
 *   -I<zxnext_layer2>/include zxnext_perf_demo.c -o zxnext_perf_demo -create-app
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
#pragma output REGISTER_SP = -1
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

static void test_clear_screen_many(void);

static void test_load_screen_many(void);

static void test_draw_pixel_many(void);

static void test_draw_line_many(void);

static void test_draw_rect_many(void);

static void test_fill_rect_many(void);

static void test_draw_text_many(void);

static void test_blit_many(void);

static void test_blit_transparent_many(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t test_number = 0;

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

    // Put Z80 in 14 MHz turbo mode.

    z80_outp(REGISTER_NUMBER_PORT, PERIPHERAL_2_REGISTER);
    peripheral_2_settings = z80_inp(REGISTER_VALUE_PORT);

    z80_outp(REGISTER_NUMBER_PORT, PERIPHERAL_2_REGISTER);
    z80_outp(REGISTER_VALUE_PORT, 0x80 | peripheral_2_settings);

    z80_outp(REGISTER_NUMBER_PORT, TURBO_MODE_REGISTER);
    z80_outp(REGISTER_VALUE_PORT, 2);
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
    layer2_configure(true, false, false, 0);
}

static void select_test(void)
{
    switch (test_number)
    {
        case 0:
            test_clear_screen_many();
            break;
        case 1:
            test_load_screen_many();
            break;
        case 2:
            test_draw_pixel_many();
            break;
        case 3:
            test_draw_line_many();
            break;
        case 4:
            test_draw_rect_many();
            break;
        case 5:
            test_fill_rect_many();
            break;
        case 6:
            test_draw_text_many();
            break;
        case 7:
            test_blit_many();
            break;
        case 8:
            test_blit_transparent_many();
            break;
        default:
            break;
    }

    test_number = (test_number + 1) % 9;
}

static void test_clear_screen_many(void)
{
    while (!in_inkey())
    {
        layer2_clear_screen(rand() % 256, NULL);
    }
}

static void test_load_screen_many(void)
{
    bool toggle = true;

    while (!in_inkey())
    {
        layer2_load_screen(toggle ? "screen1.nxi" : "screen2.nxi", NULL, false);
        toggle = !toggle;
        // A short delay so we have time to actually see the screen.
        z80_delay_ms(500);
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

static void test_draw_line_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_draw_line(rand() % 256, rand() % 192, rand() % 256, rand() % 192, rand() % 256, NULL);
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

static void test_fill_rect_many(void)
{
    layer2_clear_screen(0, NULL);

    while (!in_inkey())
    {
        layer2_fill_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
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

static void test_blit_many(void)
{
    layer2_clear_screen(0xFF, NULL);

    while (!in_inkey())
    {
        layer2_blit(rand() % 256, rand() % 192, sprite, 16, 16, NULL);
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
}
