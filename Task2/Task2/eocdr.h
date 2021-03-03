#ifndef eocdr_h
#define eocdr_h

// Размер конца записи центрального каталога
#define EOCDR_BASE_SZ 22
// Сигнатура для определения содержит ли файл zip
#define EOCDR_SIGNATURE 0x06054b50

// stdbool отсутствует
typedef enum {false, true} bool;

// структура описывает конец записи центрального каталого
typedef struct {
    uint16_t disk_nbr;        /* Number of this disk. */
    uint16_t cd_start_disk;   /* Nbr. of disk with start of the CD. */
    uint16_t disk_cd_entries; /* Nbr. of CD entries on this disk. */
    uint16_t cd_entries;      /* Nbr. of Central Directory entries. */
    uint32_t cd_size;         /* Central Directory size in bytes. */
    uint32_t cd_offset;       /* Central Directory file offset. */
    uint16_t comment_len;     /* Archive comment length. */
} eocdr;


#endif /* eocdr_h */
