#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "stats.h"
#include "output.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: no arguments\n");
        return 2;
    }
    /* ================== --help ================== */
    if (strcmp(argv[1], "--help") == 0) {
        if (argc > 2) {
            fprintf(stderr, "Error: extra arguments after --help\n");
            return 2;
        }
        print_help();
        return 0;
    }

    if (argc != 3) {
        fprintf(stderr, "Error: expected <command> <csv-file>\n");
        return 2;
    }

    const char *command = argv[1];
    const char *filename = argv[2];

    if (strcmp(command, "schema") != 0 && strcmp(command, "stats") != 0) {
        fprintf(stderr, "Error: unknown command '%s'\n", command);
        return 2;
    }

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: cannot open file '%s'\n", filename);
        return 1;
    }

    Table t = {0};
    t.file = f;

    if (read_header(&t) != 0) {
        fprintf(stderr, "Error: invalid header or empty file\n");
        table_free(&t);
        return 1;
    }

    int result = scan_schema(&t);
    if (result != 0) {
        fprintf(stderr, "Error: schema scan failed (%d)\n", result);
        table_free(&t);
        return 1;
    }
    /* ================== schema ================== */
    if (strcmp(command, "schema") == 0) {
        print_schema(&t);
    } 
    /* ================== stats ================== */
    else if (strcmp(command, "stats") == 0) {
        ColumnStats *stats = NULL;
        result = collect_stats(&t, &stats);
        if (result != 0) {
            fprintf(stderr, "Error: stats collection failed (%d)\n", result);
            table_free(&t);
            return 1;
        }
        print_stats(&t, stats);
        stats_free(stats);
    }

    table_free(&t);
    return 0;
}