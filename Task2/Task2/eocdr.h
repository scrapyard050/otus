//
//  eocdr.h
//  Task2
//
//  Created by Антон Соколов on 25.02.2021.
//

//#ifndef eocdr_h
//#define eocdr_h

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



/* Size of the End of Central Directory Record, not including comment. */
#define EOCDR_BASE_SZ 22
#define EOCDR_SIGNATURE 0x06054b50  /* "PK\5\6" little-endian. */

 bool find_eocdr(struct eocdr *r, const uint8_t *src, size_t src_len)
{
        size_t comment_len;
        const uint8_t *p;
        uint32_t signature;

        for (comment_len = 0; comment_len <= UINT16_MAX; comment_len++) {
                if (src_len < EOCDR_BASE_SZ + comment_len) {
                        break;
                }

                p = &src[src_len - EOCDR_BASE_SZ - comment_len];
                signature = extract_uint32_le(p);

                if (signature == EOCDR_SIGNATURE) {
                        r->disk_nbr = extract_uint16_le(p);
                        r->cd_start_disk = extract_uint16_le(p);
                        r->disk_cd_entries = extract_uint16_le(p);
                        r->cd_entries = extract_uint16_le(p);
                        r->cd_size = extract_uint32_le(p);
                        r->cd_offset = extract_uint32_le(p);
                        r->comment_len = extract_uint16_le(p);
                        r->comment = p;
                        return true;
                }
        }

        return false;
}

//#endif /* eocdr_h */
