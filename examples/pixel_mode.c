////////////////////////////////////////////////////////////////////////////////
/// \example pixel_mode.c
/// Understanding pixel formats and RGB packing.
///
/// Pixel formats define how RGB color values are packed into integers.
/// Different formats trade off memory usage vs color precision. This example
/// demonstrates how the same RGB values produce different packed integers
/// depending on the pixel format.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/pixel.h>

void display_value(bj_pixel_mode mode, uint8_t red, uint8_t green, uint8_t blue) {
    // bj_get_pixel_value() converts 8-bit RGB components (0-255) into a packed
    // integer value according to the specified pixel format. The packing order
    // and bit depth vary by format.
    const uint32_t val = bj_get_pixel_value(mode, red, green, blue);

    // Convert the packed value to binary string for visualization of bit layout.
    char binarystr[33] = {0};
    for(size_t b = 0 ; b < 32 ; ++b) {
        binarystr[31-b] = '0' + ((val >> b) & 0x01);
    }

    bj_info("R:%d, G:%d, B:%d -[0x%08x]--> %ld\t0x%08x\t0b%s",
        red, green, blue, mode, 0, val, binarystr
    );
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // RGB565: 16-bit format with 5 bits red, 6 bits green, 5 bits blue.
    // Uses less memory (2 bytes per pixel) but lower color precision.
    // Green gets 6 bits because human eyes are most sensitive to green.
    display_value(BJ_PIXEL_MODE_RGB565, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_RGB565, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_RGB565, 0, 0, 255);

    // XRGB1555: 16-bit format with 1 unused bit, then 5 bits each for RGB.
    // Also 2 bytes per pixel but with equal precision across all channels.
    display_value(BJ_PIXEL_MODE_XRGB1555, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_XRGB1555, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_XRGB1555, 0, 0, 255);

    // XRGB8888: 32-bit format with 8 bits per channel (full precision).
    // Uses 4 bytes per pixel but retains all 8 bits of input color data.
    // The 'X' byte is unused padding for alignment.
    display_value(BJ_PIXEL_MODE_XRGB8888, 255, 0, 0);
    display_value(BJ_PIXEL_MODE_XRGB8888, 0, 255, 0);
    display_value(BJ_PIXEL_MODE_XRGB8888, 0, 0, 255);

    return 0;
}
