#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cp1251.h"
#include "koi8.h"
#include "iso_8859_5.h"

#define CP1251 "cp1251"
#define KOI8_R "koi8"
#define IOS_8859_5 "iso-8859-5"

// максимальная количество байт для перекодировки
// поскольку работаем только с таблицами символов на кириллице, то считаем что максимальное количество байт для коддирования 2
#define COUNT_BYTE 2
#define LEN_BUFFER 256
#define SUCCESS 0

static unsigned int counter = 0;

/// Получаем размер байтов в файле
///
void Prepare(FILE *file, long *all_byets )
{
    fseek(file, 0L, SEEK_END);
    *all_byets = ftell(file);
    fseek(file, 0L, SEEK_SET);
}

void DecodeOperation( const unsigned int* unicode, char result [] )
{
    // 1 байт для хранения символа
    if( *unicode < 0x80)
    {
        result[counter++] = *unicode;
        return;;
    }
    
    // 2 байт для хранения символа
    else if( *unicode < 0x800 )
    {
        result[counter++] = ( *unicode>>6 ) | 0xC0;
        result[counter++] = ( *unicode & 0x3F ) | 0x80;
        return;
    }
}

/// вызов функции для перекодирования из кодировки cp1251
///
void DecodeCp1251(const char* ch, char result [] )
{
    for( size_t count = 0; count < sizeof(cp1251)/sizeof(cp1251[0]);  count++ )
    {
        if( *ch == cp1251[count].hex_code )
        {
            unsigned int unicode = cp1251[count].unicode_code;
            DecodeOperation(&unicode, result);
        }
    }
}

/// вызов функции для перекодирования из кодировки koi8r
///
void DecodeKoi8(const char* ch, char result [] )
{
    for( size_t count = 0; count < sizeof(koi8r)/sizeof(koi8r[0]);  count++ )
    {
        if( *ch == koi8r[count].hex_code )
        {
            unsigned int unicode = koi8r[count].unicode_code;
            DecodeOperation(&unicode, result);
        }
    }
}

/// вызов функции для перекодирования из кодировки iso8859-5
///
void DecodeIso8859(const char* ch, char result [] )
{
    for( size_t count = 0; count < sizeof(iso88595)/sizeof(iso88595[0]);  count++ )
    {
        if( *ch == iso88595[count].hex_code )
        {
            unsigned int unicode = iso88595[count].unicode_code;
            DecodeOperation(&unicode, result);
        }
    }
}

/// Определение типа кодировка
///
void Decode( const char buffer[], const char* encoding, char result [] )
{
    for( size_t it = 0; it < LEN_BUFFER;  it++)
    {
        if( !strcmp(encoding, CP1251) )
        {
            DecodeCp1251(&buffer[it], result);
            continue;
        }
        
        if( !strcmp(encoding, KOI8_R) )
        {
            DecodeKoi8(&buffer[it], result);
            continue;
        }
        
        if( !strcmp(encoding, IOS_8859_5) )
        {
            DecodeIso8859(&buffer[it], result);
            continue;
        }
    }
}

/// Чтение  файла
///
int DecodeData( FILE *file_reading_descriptor, FILE *file_result_descriptor, const char *encoding )
{
    long all_bytes = 0;
    Prepare(file_reading_descriptor, &all_bytes);
    char result[all_bytes*COUNT_BYTE];
    all_bytes *= COUNT_BYTE;
    char buffer[LEN_BUFFER];
    while (fgets(buffer, LEN_BUFFER, file_reading_descriptor))
    {
        counter = 0;
        Decode(buffer,encoding, result);
        //printf("%s\n", result);
        fprintf(file_result_descriptor, "%s", result);
    }
    
    return 0;
}

int main(int argc, const char * argv[])
{
    // проверяем количество переданных аргументов
    if( argc < 3 )
    {
        fprintf(stderr, "%s", "Wrong number of input arguments\n");
        return 1;
    }
    
    // проверяем существует ли файл куда будет записываться результат
    // если файл существуем то удаляем его
    const char* resultFile = argv[3];
    if( !access(resultFile, F_OK) )
    {
        if( remove(resultFile) )
        {
            fprintf(stderr, "%s", "Unable to delete file\n");
            return 2;
        }
    }
    
    // открываем файл куда будет писаться результат и проверяем результат выполнения операции
    FILE *file_result_descriptor = fopen(resultFile, "w");
    if( NULL == file_result_descriptor)
    {
        fprintf(stderr, "%s", "Not found file for writing result\n");
        return 3;
    }
    
    // открываем файл и проверяем результат выполнения операции
    FILE *file_reading_descriptor = fopen(argv[2], "r");
    if( NULL == file_reading_descriptor)
    {
        fprintf(stderr, "%s", "Not found file for reading\n");
        return 4;
    }
    
    const char* encoding = argv[1];
    DecodeData(file_reading_descriptor, file_result_descriptor, encoding);
    
    fclose(file_reading_descriptor);
    fclose(file_result_descriptor);
    return 0;
}


