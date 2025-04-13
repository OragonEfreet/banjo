#include <banjo/log.h>
#include <banjo/pixel.h>

#include <stdio.h>

void display_value(bj_pixel_mode mode, uint8_t red, uint8_t green, uint8_t blue) {
    const uint32_t val = bj_pixel_value(mode, red, green, blue);

    char binarystr[33] = {0};
    for(size_t b = 0 ; b < 32 ; ++b) {
        binarystr[31-b] = '0' + ((val >> b) & 0x01);
    }

    bj_info("R:%d, G:%d, B:%d -[0x%08x]--> %ld\t0x%08x\t0b%s",
        red, green, blue, mode, 0, val, binarystr
    );
}

int main(void) {

    display_value(BJ_PIXEL_MODE_RGB565, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_RGB565, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_RGB565, 0, 0, 255);
    display_value(BJ_PIXEL_MODE_XRGB1555, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_XRGB1555, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_XRGB1555, 0, 0, 255);
    display_value(BJ_PIXEL_MODE_XRGB8888, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_XRGB8888, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_XRGB8888, 0, 0, 255);




    return 0;
}
