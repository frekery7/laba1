#ifndef OUTPUT_H
#define OUTPUT_H

#include "table.h"
#include "stats.h"

void print_help(void);
void print_schema(Table *t);
void print_stats(Table *t, ColumnStats *stats);

#endif