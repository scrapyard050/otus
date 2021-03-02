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
        uint32_t signature = READ32( (p ) );
        if (signature == EOCDR_SIGNATURE)
        {
            eocdr_info->disk_nbr =   READ16(p);
            eocdr_info->cd_start_disk = READ16(p);
            eocdr_info->disk_cd_entries = READ16(p);
            eocdr_info->cd_entries = READ16(p);
            eocdr_info->cd_size = READ32(p);
            eocdr_info->cd_offset = READ32(p);
            eocdr_info->comment_len = READ16(p);
            eocdr_info->comment = p;
            if (eocdr_info->comment_len == comment_len)
            {
                    return true;
            }
        }
    }
    return false;
}

