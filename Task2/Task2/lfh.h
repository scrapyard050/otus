/// @file Локальный заголовок файла

#ifndef lfh_h
#define lfh_h
#include "global.h"

struct lfh {
        uint16_t extract_ver;
        uint16_t gp_flag;
        uint16_t method;
        uint16_t mod_time;
        uint16_t mod_date;
        uint32_t crc32;
        uint32_t comp_size;
        uint32_t uncomp_size;
        uint16_t name_len;
        uint16_t extra_len;
        const uint8_t *name;
        const uint8_t *extra;
};

// длина заголовка
#define LFH_BASE_SZ 30
// сигнатура
#define LFH_SIGNATURE 0x04034b50

static bool read_lfh( struct lfh *lfh, const uint8_t *src, size_t src_len, size_t offset )
{
    const uint8_t *p;
    uint32_t signature;

    if (offset > src_len || src_len - offset < LFH_BASE_SZ)
    {
        return false;
    }
    
    p = &src[offset];
    signature = extract_uint32_le(p);
    if (signature != LFH_SIGNATURE)
    {
        return false;
    }
    
    lfh->extract_ver = extract_uint16_le(p);
    lfh->gp_flag = extract_uint16_le(p);
    lfh->method = extract_uint16_le(p);
    lfh->mod_time = extract_uint16_le(p);
    lfh->mod_date = extract_uint16_le(p);
    lfh->crc32 = extract_uint32_le(p);
    lfh->comp_size = extract_uint32_le(p);
    lfh->uncomp_size = extract_uint32_le(p);
    lfh->name_len = extract_uint16_le(p);
    lfh->extra_len = extract_uint16_le(p);
    lfh->name = p;
    lfh->extra = lfh->name + lfh->name_len;
    assert(p == &src[offset + LFH_BASE_SZ] && "All fields read.");
    if (src_len - offset - LFH_BASE_SZ < lfh->name_len + lfh->extra_len)
    {
        return false;
    }

    return true;
}

#endif /* lfh_h */
