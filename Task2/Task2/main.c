#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "eocdr.h"


#define COUNT_READ 1
#define FILE_METADATA_SEEK 24
#define FILE_NAME_SEEK 12

#define CFH_SIGNATURE 0x02014b50

/// @brief Получаем размер файла
/// @param[in] file файл для чтения данных
///
size_t Prepare( FILE *file )
{
    size_t origin_pos = ftell(file);
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, origin_pos, SEEK_SET);
    return file_size;
}

/// @brief Поиск zip
/// @param[in] file файл для чтения
/// @param[in] offset смещение eocdr
/// @return true если конец записи центрального каталога найден
///
bool DetectZip(FILE *file, size_t *offset )
{
    size_t file_size = Prepare(file);
    size_t signature_eocdr = 0;
    fseek(file, file_size -EOCDR_BASE_SZ, SEEK_SET);
    fread(&signature_eocdr, sizeof(uint32_t), COUNT_READ, file);
    if( signature_eocdr == EOCDR_SIGNATURE)
    {
        *offset = ftell(file) - sizeof(uint32_t);
        return true;
    }

   return false;
}

/// @brief Получение eocdr
/// @param[in] file файл для чтения
/// @param[in] offset смещение eocdr
/// @param[out] record информация о eocdr
///
void ReadEocdrData(FILE *file, size_t offset, eocdr *record )
{
    fseek(file, offset + sizeof(uint32_t), SEEK_SET);
    fread(record, sizeof(eocdr), COUNT_READ, file);
}

/// @brief Отображает файлы в архиве
/// @param[in] file файл на чтение
/// @param[in] offset_eocdr смещение на структуру eocdr
///
void ListFiles(FILE *file, size_t offset_eocdr)
{
    eocdr record;
    ReadEocdrData(file, offset_eocdr, &record);
    
    fseek(file, offset_eocdr - record.cd_size, SEEK_SET);
    for (uint16_t count = 0; count < record.cd_entries; ++count)
    {
        uint32_t signature;
        fread(&signature, sizeof(uint32_t), 1, file);
        if (signature != CFH_SIGNATURE)
        {
            fprintf(stderr, "%s", "Not found central dir");
            exit(NOT_FOUND_CENTRAL_DIR);
        }

      
        fseek(file, FILE_METADATA_SEEK, SEEK_CUR);

        uint16_t name_len, extra_len, comment_len;
        fread(&name_len, sizeof(uint16_t), 1, file);
        fread(&extra_len, sizeof(uint16_t), 1, file);
        fread(&comment_len, sizeof(uint16_t), 1, file);

  
        fseek(file, FILE_NAME_SEEK, SEEK_CUR);
        char name[name_len];
        fread(&name, name_len, 1, file);
    
        
        for (int fileName = 0; fileName < name_len; ++fileName)
        {
            printf("%c", name[fileName]);
        }
        printf("\n");
        
    
        fseek(file, extra_len+comment_len, SEEK_CUR);
    }

    
}

int main(int argc, char * argv[] )
{
    // проверка входных параметров
    if( argc != 2)
    {
        fprintf(stderr, "%s", "Invalid input data");
        exit(INVALID_PARAM);
    }
    
    // читаем файл
    FILE* file;
    if( ( file = fopen(argv[1], "rb") ) == NULL )
    {
        fprintf(stderr, "%s", " Error read file");
        exit(ERROR_READ_FILE);
    }
    
    size_t offset_eocdr = 0;
    if( !DetectZip(file, &offset_eocdr) )
    {
        fprintf(stderr, "%s", " Zip file not detected");
        exit(ZIP_NOT_FOUND);
    }
    
    ListFiles(file, offset_eocdr);
    
    fclose(file);
    
    return 0;
}
