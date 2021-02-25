#include <string.h>
#include <assert.h>
#include <time.h>
#include "cfh.h"
#include "lfh.h"
#include "eocdr.h"

typedef size_t zipiter_t; /* Zip archive member iterator. */

typedef struct zip_t zip_t;
struct zip_t {
        uint16_t num_members;    /* Number of members. */
        const uint8_t *comment;  /* Zip file comment (not terminated). */
        uint16_t comment_len;    /* Zip file comment length. */
        zipiter_t members_begin; /* Iterator to the first member. */
        zipiter_t members_end;   /* Iterator to the end of members. */

        const uint8_t *src;
        size_t src_len;
};

typedef enum { ZIP_STORED = 0, ZIP_DEFLATED = 8 } method_t;
typedef struct zipmemb_t zipmemb_t;
struct zipmemb_t {
        const uint8_t *name;      /* Member name (not null terminated). */
        uint16_t name_len;        /* Member name length. */
        time_t mtime;             /* Modification time. */
        uint32_t comp_size;       /* Compressed size. */
        const uint8_t *comp_data; /* Compressed data. */
        method_t method;          /* Compression method. */
        uint32_t uncomp_size;     /* Uncompressed size. */
        uint32_t crc32;           /* CRC-32 checksum. */
        const uint8_t *comment;   /* Comment (not null terminated). */
        uint16_t comment_len;     /* Comment length. */
        bool is_dir;              /* Whether this is a directory. */
        zipiter_t next;           /* Iterator to the next member. */
};


time_t dos2ctime(uint16_t dos_date, uint16_t dos_time)
{
        struct tm tm = {0};

        tm.tm_sec = (dos_time & 0x1f) * 2;  /* Bits 0--4:  Secs divided by 2. */
        tm.tm_min = (dos_time >> 5) & 0x3f; /* Bits 5--10: Minute. */
        tm.tm_hour = (dos_time >> 11);      /* Bits 11-15: Hour (0--23). */

        tm.tm_mday = (dos_date & 0x1f);          /* Bits 0--4: Day (1--31). */
        tm.tm_mon = ((dos_date >> 5) & 0xf) - 1; /* Bits 5--8: Month (1--12). */
        tm.tm_year = (dos_date >> 9) + 80;       /* Bits 9--15: Year-1980. */

        tm.tm_isdst = -1;

        return mktime(&tm);
}

/* Get the Zip archive member through iterator it. */
zipmemb_t zip_member(const zip_t *zip, zipiter_t it)
{
        struct cfh cfh;
        struct lfh lfh;
        bool ok;
        zipmemb_t m;

        assert(it >= zip->members_begin && it < zip->members_end);

        ok = read_cfh(&cfh, zip->src, zip->src_len, it);
        assert(ok);

        ok = read_lfh(&lfh, zip->src, zip->src_len, cfh.lfh_offset);
        assert(ok);

        m.name = cfh.name;
        m.name_len = cfh.name_len;
        m.mtime = dos2ctime(cfh.mod_date, cfh.mod_time);
        m.comp_size = cfh.comp_size;
        m.comp_data = lfh.extra + lfh.extra_len;
        m.method = cfh.method;
        m.uncomp_size = cfh.uncomp_size;
        m.crc32 = cfh.crc32;
        m.comment = cfh.comment;
        m.comment_len = cfh.comment_len;
        m.is_dir = (cfh.ext_attrs & EXT_ATTR_DIR) != 0;

        m.next = it + CFH_BASE_SZ +
                 cfh.name_len + cfh.extra_len + cfh.comment_len;

        assert(m.next <= zip->members_end);

        return m;
}

bool zip_read(zip_t *zip, const uint8_t *src, size_t src_len)
{
        struct eocdr eocdrInfo;
        struct cfh cfh;
        struct lfh lfh;
        size_t i, offset;
        const uint8_t *comp_data;

        zip->src = src;
        zip->src_len = src_len;

        if (!find_eocdr(&eocdrInfo, src, src_len)) {
                return false;
        }

        if (eocdrInfo.disk_nbr != 0 || eocdrInfo.cd_start_disk != 0 ||
            eocdrInfo.disk_cd_entries != eocdrInfo.cd_entries) {
                return false; /* Cannot handle multi-volume archives. */
        }

        zip->num_members = eocdrInfo.cd_entries;
        zip->comment = eocdrInfo.comment;
        zip->comment_len = eocdrInfo.comment_len;

        offset = eocdrInfo.cd_offset;
        zip->members_begin = offset;

        /* Read the member info and do a few checks. */
        for (i = 0; i < eocdrInfo.cd_entries; i++) {
                if (!read_cfh(&cfh, src, src_len, offset)) {
                        return false;
                }

                if (cfh.gp_flag & 1) {
                        return false; /* The member is encrypted. */
                }
                if (cfh.method != ZIP_STORED && cfh.method != ZIP_DEFLATED) {
                        return false; /* Unsupported compression method. */
                }
                if (cfh.method == ZIP_STORED &&
                    cfh.uncomp_size != cfh.comp_size) {
                        return false;
                }
                if (cfh.disk_nbr_start != 0) {
                        return false; /* Cannot handle multi-volume archives. */
                }
                if (memchr(cfh.name, '\0', cfh.name_len) != NULL) {
                        return false; /* Bad filename. */
                }

                if (!read_lfh(&lfh, src, src_len, cfh.lfh_offset)) {
                        return false;
                }

                comp_data = lfh.extra + lfh.extra_len;
                if (cfh.comp_size > src_len - (size_t)(comp_data - src)) {
                        return false; /* Member data does not fit in src. */
                }

                offset += CFH_BASE_SZ + cfh.name_len + cfh.extra_len +
                          cfh.comment_len;
        }

        zip->members_end = offset;

        return true;
}
