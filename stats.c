#include "stats.h"
#include "csv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <limits.h>

void stats_init(ColumnStats *s) {
    memset(s, 0, sizeof(*s));
    s->int_min = LLONG_MAX;
    s->int_max = LLONG_MIN;
    s->double_min = INFINITY;
    s->double_max = -INFINITY;
}

static void stats_update_int(ColumnStats *s, long long val) {
    if (val < s->int_min) s->int_min = val;
    if (val > s->int_max) s->int_max = val;
    s->int_sum += (long double)val;
    s->int_count++;
    if (val > 0) s->int_positive++;
    if (val < 0) s->int_negative++;
    if (val % 2 == 0) s->int_even++;
}

static void stats_update_double(ColumnStats *s, double val) {
    if (val < s->double_min) s->double_min = val;
    if (val > s->double_max) s->double_max = val;
    s->double_sum += val;
    s->double_count++;
    if (val > 0.0) s->double_positive++;
    if (val < 0.0) s->double_negative++;
}

int collect_stats(Table *t, ColumnStats **out_stats) {
    char **fields;
    int count;

    // Пропускаем заголовок
    if (read_csv_line(t->file, &fields, &count) <= 0) return -1;
    for (int i = 0; i < count; i++) free(fields[i]);
    free(fields);

    ColumnStats *stats = calloc(t->num_columns, sizeof(ColumnStats));
    if (!stats) return -1;
    for (int i = 0; i < t->num_columns; i++) stats_init(&stats[i]);

    while (1) {
        int res = read_csv_line(t->file, &fields, &count);
        if (res < 0) { free(stats); return -1; }
        if (res == 0) break;
        if (count != t->num_columns) {
            for (int i = 0; i < count; i++) free(fields[i]);
            free(fields); free(stats);
            return -2;
        }

        for (int i = 0; i < count; i++) {
            const char *val = fields[i];
            if (val[0] == '\0') {
                stats[i].null_count++;
            } else {
                switch (t->schema[i].type) {
                    case TYPE_INT:
                        stats_update_int(&stats[i], strtoll(val, NULL, 10));
                        break;
                    case TYPE_DOUBLE:
                        stats_update_double(&stats[i], strtod(val, NULL));
                        break;
                    default:
                        break;
                }
            }
            free(fields[i]);
        }
        free(fields);
    }

    *out_stats = stats;
    return 0;
}

void stats_free(ColumnStats *stats) {
    free(stats);
}