
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <video/mipi_display.h>
#include <linux/gpio.h>

#include "fbtft.h"

#define DRVNAME "fb_ili9486"

#define DEFAULT_GAMMA "00 23 26 08 10 24 36 43 46 0C 18 18 23 2F\n" \
                         "00 23 26 08 10 24 36 43 46 0C 18 18 23 2F"

enum ili9486_command {
    FRAME_RATE = 0xB1,
    DISPLAY_FUNCTION_CONTROL = 0xB6,
    POWER_CONTROL_1 = 0xC0,
    POWER_CONTROL_2 = 0xC1,
    VCOM_CONTROL_1 = 0xC5,
    VCOM_CONTROL_2 = 0xC7,
    MEMORY_ACCESS_CONTROL = 0x36,
    PIXEL_FORMAT_SET = 0x3A,
    ENTRY_MODE_SET = 0xB7,
};

static int init_display(struct fbtft_par *par)
{
    write_reg(par, MIPI_DCS_EXIT_SLEEP_MODE);
    mdelay(120);

    write_reg(par, PIXEL_FORMAT_SET, 0x55);
    write_reg(par, DISPLAY_FUNCTION_CONTROL, 0x0A, 0x82, 0x27);
    write_reg(par, FRAME_RATE, 0x00, 0x1B);
    write_reg(par, POWER_CONTROL_1, 0x1B, 0x0A);
    write_reg(par, POWER_CONTROL_2, 0x02);
    write_reg(par, VCOM_CONTROL_1, 0x20, 0x27);
    write_reg(par, VCOM_CONTROL_2, 0x86);
    write_reg(par, MEMORY_ACCESS_CONTROL, 0x48);
    write_reg(par, MIPI_DCS_ENTER_NORMAL_MODE);
    write_reg(par, MIPI_DCS_SET_DISPLAY_ON);
    return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
    write_reg(par, MIPI_DCS_SET_COLUMN_ADDRESS, xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF);
    write_reg(par, MIPI_DCS_SET_PAGE_ADDRESS, ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF);
    write_reg(par, MIPI_DCS_WRITE_MEMORY_START);
}

static int set_var(struct fbtft_par *par)
{
    u8 madctl_par = 0x48;
    write_reg(par, MEMORY_ACCESS_CONTROL, madctl_par);
    return 0;
}

static int blank(struct fbtft_par *par, bool on)
{
    if (on)
        write_reg(par, MIPI_DCS_SET_DISPLAY_OFF);
    else
        write_reg(par, MIPI_DCS_SET_DISPLAY_ON);
    return 0;
}

static struct fbtft_display display = {
    .regwidth = 8,
    .width = 320,
    .height = 480,
    .gamma_num = 2,
    .gamma_len = 14,
    .gamma = DEFAULT_GAMMA,
    .fbtftops = {
        .init_display = init_display,
        .set_addr_win = set_addr_win,
        .set_var = set_var,
        .blank = blank,
    },
};

FBTFT_REGISTER_DRIVER(DRVNAME, "ilitek,ili9486", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9486");
MODULE_ALIAS("platform:ili9486");

MODULE_DESCRIPTION("FB driver for the ILI9486 LCD Controller");
MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");
