#include <stdio.h>
#include <string.h>
#include "cp1251.h"
#include "koi8.h"
#include "iso_8859_5.h"

static unsigned int counter = 0;

/// Получаем размер байтов в файле
///
void Prepare(FILE *file, long *all_byets )
{
    fseek(file, 0L, SEEK_END);
    *all_byets = ftell(file);
    fseek(file, 0L, SEEK_SET);
}


// 3 итерация 38
void Decode( const char buffer[], const size_t *len_buffer, char result [] )
{
    for( size_t it = 0; it < *len_buffer;  it++)
    {
        for( size_t count = 0; count < sizeof(cp1251)/sizeof(cp1251[0]);  count++ )
        {
            if( buffer[it] == cp1251[count].hex_code )
            {
                unsigned int unicode = cp1251[count].unicode_code;
            
                // 1 байт для хранения символа
                if( unicode < 0x80)
                {
                    result[counter++] = unicode;
                    break;
                }
                // 2 байт для хранения символа
                else if( unicode < 0x800 )
                {
                    result[counter++] = ( unicode>>6 ) | 0xC0;
                    result[counter++] = ( unicode & 0x3F ) | 0x80;
                }
                // 3 байт для хранения символа
                else if( unicode < 0x10000 )
                {
                    result[ counter++ ] = ( unicode >> 12 ) | 0xe0;
                    result[ counter++ ] =  (unicode >> 6  & 0x3f) | 0x80;
                    result[ counter++ ] =  (unicode & 0x3f) | 0x80;
                }
                // 4 байт для хранения символа
                else if( unicode < 0x200000 )
                {
                    result[ counter++ ] = unicode >> 18 | 0xf0;
                    result[ counter++ ] = (unicode >> 12 & 0x3f) | 0x80;
                    result[ counter++ ] = (unicode >> 6 & 0x3f) | 0x80;
                    result[ counter++ ] = (unicode & 0x3f) | 0x80;
                }
            }
            
        }
    }
}

/// Чтение и перекодировка файла
///
int DecodeCp1251( FILE *file, const long *all_byets, char result[] )
{
    const size_t len_buffer = 256;
    char buffer[len_buffer];
    while (fgets(buffer, len_buffer - 1, file))
    {
        counter = 0;
        buffer[strcspn(buffer, "\n")] = 0;
        Decode(buffer, &len_buffer, result);
        printf("%s\n", result);
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
    
    // открываем файл и проверяем результат выполнения операции
    FILE *file = fopen(argv[2], "r");
    if( NULL == file)
    {
        fprintf(stderr, "%s", "Not found file for reading\n");
        return 2;
    }
    
    long all_bytes = 0;
    Prepare(file, &all_bytes);
    char result[all_bytes*2];
    all_bytes *= 2;
    DecodeCp1251(file, &all_bytes, result);
    
    fclose(file);
    return 0;
}


