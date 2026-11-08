#ifndef STATS_H
#define STATS_H

#include "table.h"

typedef struct {
    //INT
    long long int_min, int_max;
    long double int_sum;
    long long int_count;
    long long int_positive, int_negative, int_even;

    //DOUBLE
    double double_min, double_max;
    long double double_sum;
    long long double_count;
    long long double_positive, double_negative;

    //NULL
    long long null_count;
} ColumnStats;

void stats_init(ColumnStats *s);
int collect_stats(Table *t, ColumnStats **out_stats);
void stats_free(ColumnStats *stats);

#endif