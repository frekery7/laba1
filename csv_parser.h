#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <stdio.h>
#include <stddef.h>

// read_field — Читает одно поле из CSV
// FILE *f — Файл, откуда считываем информацию
// char **out_buf — Место куда функция запишет прочитанную ячейку
// size_t *out_cap — Емкость ячейки
// 1 — поле + ','
// 2 — поле + '\n' или EOF
// 0 — конец файла без данных
// -1 — ошибка памяти
int read_field(FILE *f, char **out_buf, size_t *out_cap);

// read_csv_line — Читает одну строку из CSV
// FILE *f — Файл, откуда считываем информацию
// char ***out_fields — Место куда функция запишет массив прочитанных ячеек
// int *out_count — Место куда функция запишет количество ячеек
// 1 — успешное выполнение
// 0 — конец файла
// -1 — ошибка памяти
int read_csv_line(FILE *f, char ***out_fields, int *out_count);

#endif