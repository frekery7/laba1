#include "table.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Основная функция: Превращает тип в строчку
const char* type_to_string(ColumnType t) {
    switch (t) {
        case TYPE_INT: return "INT";
        case TYPE_DOUBLE: return "DOUBLE";
        case TYPE_STRING: return "STRING";
        case TYPE_NULL: return "NULL";
    }
    return "UNKNOWN";
}

// Вспомагательная функция: Проверяет строка валидный INT или нет(1 - Да, 0 - Нет)
static int is_valid_int(const char *str) {
    if (str == NULL || str[0] == '\0' ) return 0;
    errno = 0;
    char *end;
    strtoll(str, &end, 10);
    return (errno != ERANGE && *end == '\0');
}

// Вспомагательная функция: Проверяет строка валидный DOUBLE или нет(1 - Да, 0 - Нет)
static int is_valid_double(const char *str) {
    if (str == NULL || str[0] == '\0') return 0;
    const char *c = str;
    if (*c == '+' || *c == '-') c++;
    if (isdigit((unsigned char)*c) == 0) return 0;
    while (isdigit((unsigned char)*c)) c++;
    if (*c != '.') return 0;
    c++;
    if (isdigit((unsigned char)*c) == 0) return 0;
    while (isdigit((unsigned char)*c)) c++;
    return (*c == '\0');
}

// Вспомагательная функция: Определеят тип одной ячейки
static ColumnType detect_type(const char *str) {
    if (str == NULL || str[0] == '\0') return TYPE_NULL;
    if (is_valid_int(str)) return TYPE_INT;
    if (is_valid_double(str)) return TYPE_DOUBLE;
    return TYPE_STRING;
}

// Вспомагательная функция: Обновляет тип столбца используя новая тип и текущий
static ColumnType merge_type(ColumnType current, ColumnType new_type) {
    if (new_type == TYPE_NULL) return current;
    if (current == TYPE_NULL) return new_type;
    if (current == TYPE_STRING || new_type == TYPE_STRING) return TYPE_STRING;
    if (current == TYPE_DOUBLE || new_type == TYPE_DOUBLE) return TYPE_DOUBLE;
    return TYPE_INT;
}


// Основная функция: Читает все заголовки
int read_header(Table *t) {
    char **fields;
    int count;
    if (read_csv_line(t->file, &fields, &count) <= 0) return -1;

    t->num_columns = count;
    t->schema = calloc(count, sizeof(ColumnSchema));
    if (t->schema == NULL) {
        for (int i = 0; i < count; i++) free(fields[i]);
        free(fields);
        return -1;
    }
    for (int i = 0; i < count; i++) {
        t->schema[i].name = strdup(fields[i]);
        t->schema[i].type = TYPE_NULL;
        free(fields[i]);
    }
    free(fields);
    return 0;
}

// Основная функция: Считывает данные для schema
int scan_schema(Table *t) {
    long long rows = 0;
    while (1) {
        char **fields;
        int count;
        int res = read_csv_line(t->file, &fields, &count);
        if (res < 0) return -1;
        if (res == 0) break;
        rows++;

        if (count != t->num_columns) {
            for (int i = 0; i < count; i++) free(fields[i]);
            free(fields);
            return -2;
        }

        for (int i = 0; i < count; i++) {
            ColumnType ft = detect_type(fields[i]);
            t->schema[i].type = merge_type(t->schema[i].type, ft);
            free(fields[i]);
        }
        free(fields);
    }
    t->num_rows = rows;
    rewind(t->file);
    return 0;
}

// Основная функция: Освобождает память, выделенную для Table + закрывает файл 
void table_free(Table *t) {
    if (t->schema) {
        for (int i = 0; i < t->num_columns; i++) free(t->schema[i].name);
        free(t->schema);
    }
    if (t->file) fclose(t->file);
}