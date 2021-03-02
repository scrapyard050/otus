/// @file
/// Конец записи центрального каталога

#include "global.h"


struct eocdr {
        uint16_t disk_nbr;        /* Number of this disk. */
        uint16_t cd_start_disk;   /* Nbr. of disk with start of the CD. */
        uint16_t disk_cd_entries; /* Nbr. of CD entries on this disk. */
        uint16_t cd_entries;      /* Nbr. of Central Directory entries. */
        uint32_t cd_size;         /* Central Directory size in bytes. */
        uint32_t cd_offset;       /* Central Directory file offset. */
        uint16_t comment_len;     /* Archive comment length. */
        const uint8_t *comment;   /* Archive comment. */
};



// Размер конца записи центрального каталога
#define EOCDR_BASE_SZ 22
// Сигнатура для определения содержит ли файл zip
#define EOCDR_SIGNATURE 0x06054b50

/// @brief Поиск конца записи центрального каталога
/// @param[out] eocdr_info структура куда будут помещены данные если в файле содержится zip
/// @param[in] src содержимое файла
/// @param[in] src_len размер файла
///
bool find_eocdr( struct eocdr *eocdr_info, const uint8_t *src, size_t src_len )
{
    for (size_t comment_len = 0; comment_len <= UINT16_MAX; comment_len++)
    {
        if (src_len < EOCDR_BASE_SZ + comment_len)
        {
            break;
        }
        
        const uint8_t *p = &src[src_len - EOCDR_BASE_SZ - comment_len];
        // смещаемся в файле для поиска сигнатуры zip
        uint32_t signature = extract_uint32_le( (p ) );
        if (signature == EOCDR_SIGNATURE)
        {
            eocdr_info->disk_nbr =  extract_uint16_le(p - sizeof(uint16_t));
            eocdr_info->cd_start_disk = extract_uint16_le(p - sizeof(uint16_t) - sizeof(uint16_t)  );
            eocdr_info->disk_cd_entries = extract_uint16_le(p - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) );
            eocdr_info->cd_entries = extract_uint16_le (p - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t));
            eocdr_info->cd_size = extract_uint32_le(p - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint32_t) );
            eocdr_info->cd_offset = extract_uint32_le(p - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint32_t) - sizeof(uint32_t) );
            eocdr_info->comment_len = extract_uint16_le(p - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint16_t) - sizeof(uint32_t) - sizeof(uint32_t) - sizeof(uint16_t));
            eocdr_info->comment = p;
            return true;
        }
    }
    return false;
}

