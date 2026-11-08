#define _GNU_SOURCE
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>

int read_field(FILE *f, char **out_buf, size_t *out_cap){
    int c; // текущий символ
    size_t len = 0; // длина текста в ячейке
    int has_data = 0; // флаг(были ли данные)

    // Создаем буфер при первом запуске
    if (*out_buf == NULL){
        *out_cap = 16;
        char *tmp = malloc(*out_cap);
        if (tmp == NULL) return -1;
        *out_buf = tmp;
    }

    // Читаем посимвольно до разделителя
    while ((c = fgetc(f)) != EOF){
        if (c == ',' || c == '\n') break;
        if (c == '\r') continue; // игнорим виндовый перенос

        // Расширяем буфер, если строка не влезает
        if (len + 1 >= *out_cap) {
            size_t new_cap = *out_cap * 2;
            char *tmp = realloc(*out_buf, new_cap);
            if (tmp == NULL) return -1;
            *out_buf = tmp;
            *out_cap = new_cap;
        }
        (*out_buf)[len++] = (char)c;
        has_data = 1;
    }

    // Конец файла без новых данных
    if (c == EOF && !has_data && len == 0) return 0;

    (*out_buf)[len] = '\0';

    // Обрезание пробелов и табов
    char *start = *out_buf;
    while (*start == ' ' || *start == '\t') start++;
    
    char *end = start + strlen(start);
    while (end > start && (end[-1] == ' ' || end[-1] == '\t')) end--;
    *end = '\0';

    if (start != *out_buf){
        memmove(*out_buf, start, (size_t)(end - start) + 1);
    }

    // 2 = конец строки, 1 = обычная ячейка
    if (c == '\n' || c == EOF) return 2;
    return 1;
}

int read_csv_line(FILE *f, char ***out_fields, int *out_count){
    char **fields = NULL; // массив ячеек
    int count = 0; // счетчик ячеек
    int capacity = 0; // емкость массива fields
    size_t field_cap = 0; // емкость одной ячейки
    char *field_buf = NULL; // буфер под одну ячейку
    int res; // для read_field

    // Собираем ячейки
    while ((res = read_field(f, &field_buf, &field_cap)) > 0){
        if (count >= capacity){
            int new_capacity = capacity == 0 ? 8 : capacity * 2;
            char **tmp = realloc(fields, new_capacity * sizeof(char*));
            if (tmp == NULL){
                for (int i = 0; i < count; i++) free(fields[i]);
                free(fields); free(field_buf);
                return -1;
            }
            capacity = new_capacity;
            fields = tmp;
        }

        // Копируем ячейку в массив
        fields[count] = strdup(field_buf);
        if (fields[count] == NULL) {
            for (int i = 0; i < count; i++) free(fields[i]);
            free(fields); free(field_buf);
            return -1;
        }
        count++;
        if (res == 2) break;
    }

    free(field_buf);

    if (res < 0) {
        for (int i = 0; i < count; i++) free(fields[i]);
        free(fields);
        return -1;
    }
    if (res == 0 && count == 0 && fields == NULL){
        *out_fields = NULL;
        *out_count = 0;
        return 0;
    }

    // Выходные данные
    *out_fields = fields;
    *out_count = count;
    return 1;
}
