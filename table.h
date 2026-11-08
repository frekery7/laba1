#ifndef TABLE_H
#define TABLE_H
#include <stdio.h>

typedef enum{
    TYPE_NULL = 0,
    TYPE_INT = 1,
    TYPE_DOUBLE = 2,
    TYPE_STRING = 3
} ColumnType;

typedef struct{
    char *name;      // имя из заголовка
    ColumnType type; // итоговый тип
} ColumnSchema;

typedef struct{
    FILE *file;           // файл CSV
    ColumnSchema *schema; // массив схем колонок
    int num_columns;      // число колонок
    long long num_rows;   // число строк с данными
} Table;

int read_header(Table *t);
int scan_schema(Table *t);
void table_free(Table *t);
const char *type_to_string(ColumnType t);
#endif
