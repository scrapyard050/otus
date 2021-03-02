/// @file
/// Центральный заголовок файла

#ifndef cfh_h
#define cfh_h

#include "global.h"

#define EXT_ATTR_DIR (1U << 4)
#define EXT_ATTR_ARC (1U << 5)

struct cfh {
        uint16_t made_by_ver;    /* Version made by. */
        uint16_t extract_ver;    /* Version needed to extract. */
        uint16_t gp_flag;        /* General purpose bit flag. */
        uint16_t method;         /* Compression method. */
        uint16_t mod_time;       /* Modification time. */
        uint16_t mod_date;       /* Modification date. */
        uint32_t crc32;          /* CRC-32 checksum. */
        uint32_t comp_size;      /* Compressed size. */
        uint32_t uncomp_size;    /* Uncompressed size. */
        uint16_t name_len;       /* Filename length. */
        uint16_t extra_len;      /* Extra data length. */
        uint16_t comment_len;    /* Comment length. */
        uint16_t disk_nbr_start; /* Disk nbr. where file begins. */
        uint16_t int_attrs;      /* Internal file attributes. */
        uint32_t ext_attrs;      /* External file attributes. */
        uint32_t lfh_offset;     /* Local File Header offset. */
        const uint8_t *name;     /* Filename. */
        const uint8_t *extra;    /* Extra data. */
        const uint8_t *comment;  /* File comment. */
};


// Размер центрального заголовка
#define CFH_BASE_SZ 46
// Сигнатура
#define CFH_SIGNATURE 0x02014b50 

static bool read_cfh(struct cfh *cfh, const uint8_t *src, size_t src_len, size_t offset)
{
    const uint8_t *p;
    uint32_t signature;

    if (offset > src_len || src_len - offset < CFH_BASE_SZ)
    {
        return false;
    }
    
    p = &src[offset];
    signature = READ32(p);
    if (signature != CFH_SIGNATURE)
    {
        return false;
    }
    
    cfh->made_by_ver = extract_uint16_le(p);
    cfh->extract_ver = extract_uint16_le(p);
    cfh->gp_flag = extract_uint16_le(p);
    cfh->method = extract_uint16_le(p);
    cfh->mod_time = extract_uint16_le(p);
    cfh->mod_date = extract_uint16_le(p);
    cfh->crc32 = extract_uint32_le(p);
    cfh->comp_size = extract_uint32_le(p);
    cfh->uncomp_size = extract_uint32_le(p);
    cfh->name_len = extract_uint16_le(p);
    cfh->extra_len = extract_uint16_le(p);
    cfh->comment_len = extract_uint16_le(p);
    cfh->disk_nbr_start = extract_uint16_le(p);
    cfh->int_attrs = extract_uint16_le(p);
    cfh->ext_attrs = extract_uint32_le(p);
    cfh->lfh_offset = extract_uint32_le(p);
    cfh->name = p;
    cfh->extra = cfh->name + cfh->name_len;
    cfh->comment = cfh->extra + cfh->extra_len;
    assert(p == &src[offset + CFH_BASE_SZ] && "All fields read.");
    if (src_len - offset - CFH_BASE_SZ < cfh->name_len + cfh->extra_len + cfh->comment_len)
    {
        return false;
    }
    return true;
}

#endif /* cfh_h */
