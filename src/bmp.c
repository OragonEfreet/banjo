#include "bmp.h"

void bmp_read_header(const u8* buffer, bmp_header* header, bj_error* p_error) {

    // Read signature
#ifdef BANJO_PEDANTIC
    if (*(u16*)buffer != BMP_SIGNATURE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u16);

    header->file_size = *(u32*)buffer;  buffer += sizeof(u32);
    buffer += sizeof(u32); // Skip reserved
    header->data_offset = *(u32*)buffer;  buffer += sizeof(u32);
}

void bmp_read_info_header(const u8* buffer, bmp_info_header* info_header, bj_error* p_error) {

#ifdef BANJO_PEDANTIC
    if (*(u32*)buffer != BMP_INFO_HEADER_SIZE) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u32);

    info_header->width  = *(u32*)buffer;  buffer += sizeof(u32);
    info_header->height = *(u32*)buffer;  buffer += sizeof(u32);

#ifdef BANJO_PEDANTIC
    if (*(u16*)buffer != 0x01) { // Planes
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT);
        return;
    }
#endif
    buffer += sizeof(u16);

    


}


    /* u32 bmp_data_offset = 0; */
    /* fread(&bmp_data_offset, sizeof(u32), 1, bmp_file); */
    /* if(bmp_data_offset == 0) { */
    /*     bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT); */
    /*     fclose(bmp_file); */
    /*     return p_framebuffer; */
    /* } */

    /* u32 bmp_infoheader_size = 0; */
    /* fread(&bmp_infoheader_size, sizeof(u32), 1, bmp_file); */
    /* if(bmp_infoheader_size != 40) { */
    /*     bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT); */
    /*     fclose(bmp_file); */
    /*     return p_framebuffer; */
    /* } */

    /* u32 bmp_infoheader_width = 0; */
    /* fread(&bmp_infoheader_width, sizeof(u32), 1, bmp_file); */
    /* if(bmp_infoheader_width == 0) { */
    /*     bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT); */
    /*     fclose(bmp_file); */
    /*     return p_framebuffer; */
    /* } */

    /* u32 bmp_infoheader_height = 0; */
    /* fread(&bmp_infoheader_height, sizeof(u32), 1, bmp_file); */
    /* if(bmp_infoheader_height == 0) { */
    /*     bj_set_error(p_error, BJ_DOMAIN_IO, BJ_INVALID_FORMAT); */
    /*     fclose(bmp_file); */
    /*     return p_framebuffer; */
    /* } */
