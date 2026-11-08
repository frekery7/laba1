#include "output.h"
#include <stdio.h>

static void print_escaped_name(const char *name) {
    printf("\"");
    for (const char *p = name; *p; p++) {
        if (*p == '"') printf("\"\"");
        else putchar(*p);
    }
    printf("\"");
}

void print_help(void) {
    printf("Usage:\n");
    printf("    csvstat --help\n");
    printf("    csvstat schema <csv-file>\n");
    printf("    csvstat stats <csv-file>\n");
}

void print_schema(Table *t) {
    printf("rows %lld columns %d\n", t->num_rows, t->num_columns);
    for (int i = 0; i < t->num_columns; i++) {
        printf("%d ", i);
        print_escaped_name(t->schema[i].name);
        printf(" %s\n", type_to_string(t->schema[i].type));
    }
}

void print_stats(Table *t, ColumnStats *stats) {
    printf("rows %lld\n", t->num_rows);
    printf("columns %d\n", t->num_columns);

    for (int i = 0; i < t->num_columns; i++) {
        printf("\n");
        ColumnStats *s = &stats[i];
        printf("column %d name ", i);
        print_escaped_name(t->schema[i].name);

        switch (t->schema[i].type) {
            case TYPE_INT:
                printf(" type INT\n");
                printf("nulls %lld\n", s->null_count);
                printf("min %lld\n", s->int_min);
                printf("max %lld\n", s->int_max);
                printf("mean %.6Lf\n", s->int_sum / (long double)s->int_count);
                printf("positive %lld\n", s->int_positive);
                printf("negative %lld\n", s->int_negative);
                printf("even %lld\n", s->int_even);
                break;

            case TYPE_DOUBLE:
                printf(" type DOUBLE\n");
                printf("nulls %lld\n", s->null_count);
                {
                    double d_min = s->double_min == 0.0 ? 0.0 : s->double_min;
                    double d_max = s->double_max == 0.0 ? 0.0 : s->double_max;
                    long double d_mean = s->double_sum / (long double)s->double_count;
                    if (d_mean == 0.0L) d_mean = 0.0L;
                    printf("min %.6f\n", d_min);
                    printf("max %.6f\n", d_max);
                    printf("mean %.6Lf\n", d_mean);
                }
                printf("positive %lld\n", s->double_positive);
                printf("negative %lld\n", s->double_negative);
                break;

            case TYPE_STRING:
                printf(" type STRING\n");
                printf("nulls %lld\n", s->null_count);
                break;

            case TYPE_NULL:
                printf(" type NULL\n");
                printf("rows %lld\n", t->num_rows);
                break;
        }
    }
}