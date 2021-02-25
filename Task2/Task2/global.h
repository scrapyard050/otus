//
//  global.h
//  Task2
//
//  Created by Антон Соколов on 25.02.2021.
//


#ifndef global_h
#define global_h

#include <stdint.h>
typedef enum {false, true} bool;

uint16_t extract_uint16_le(const uint8_t *buffer)
{
    return ((buffer[0] | (buffer[1] << 8)));
}
uint32_t extract_uint32_le(const uint8_t *buffer)
{
    return (((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 0x10)) | (buffer[3] << 0x18));
}

#endif /* global_h */
