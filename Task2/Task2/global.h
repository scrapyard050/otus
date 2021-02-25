/// @file
/// Глобальные функции

#ifndef global_h
#define global_h

#include <stdint.h>

// представление типа bool, поскольку stdbool.h отсутствует
typedef enum {false, true} bool;

/// @brief смещение на 2 байта little endian
/// @param[in] buffer  представление файла
/// @return смещенные данные в буффере
///
uint16_t extract_uint16_le(const uint8_t *buffer)
{
    return ((buffer[0] | (buffer[1] << 8)));
}

/// @brief смещение на 4 байта little endian
/// @param[in] buffer  представление файла
/// @return смещенные данные в буфере
///
uint32_t extract_uint32_le(const uint8_t *buffer)
{
    return (((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 0x10)) | (buffer[3] << 0x18));
}

#endif /* global_h */
