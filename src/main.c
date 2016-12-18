#include "main.h"


int DEBUG = 0;


int main(int argc, char **argv) {
    int ret_code = 0;
    char *short_options = "f:ps::r:dVH";
    struct option long_options[] = {
        {"file", required_argument, 0, 'f'},
        {"push", no_argument, 0, 'p'},
        {"shift", optional_argument, 0, 's'},
        {"revolve", required_argument, 0, 'r'},
        {"debug", no_argument, 0, 'd'},
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'H'},
        {0, 0, 0, 0},
    };
    int option_index = 0;
    int c, i;
    int do_push = 0;
    int do_shift = 0;
    int do_revolve = 0;
    int do_version = 0;
    int do_help = 0;
    int items_num = 1;
    char* items_file_path = NULL;
    FILE *items_file = NULL;
    char *items_file_mode = NULL;
    char *item = NULL;
    char *revolve_command = NULL;

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
        case 'f':
            items_file_path = optarg;
            break;
        case 'p':
            do_push = 1;
            items_file_mode = "a";
            break;
        case 's':
            do_shift = 1;
            if (NULL != optarg) items_num = atoi(optarg);
            items_file_mode = "r+";
            break;
        case 'r':
            do_revolve = 1;
            revolve_command = optarg;
            break;
        case 'd':
            DEBUG = 1;
            break;
        case 'V':
            do_version = 1;
            break;
        case 'H':
            do_help = 1;
            break;
        case '?':
        default:
            fprintf(stderr, "Unrecognized parameter\n");
            return 1;
        }
    }

    if (do_help) {
        print_help(argv[0]);
        return 0;
    }

    if (do_version) {
        printf("%s\n", REVOLVER_VERSION);
        return 0;
    }

    if (0 == items_num) {
        fprintf(stderr, "Argument for number of items Not a Number\n");
        return 1;
    }

    if (NULL == items_file_path) {
        fprintf(stderr, "Items file not specified\n");
        return 1;
    }

    if ((do_push + do_shift) > 1) {
        fprintf(stderr, "Conflicting options\n");
        return 1;
    }

    if ((do_push) && optind >= argc) {
        fprintf(stderr, "No items specified\n");
        return 1;
    }

    if (do_revolve) {
        ret_code = revolver_revolve(items_file_path, revolve_command);
        if (0 > ret_code) {
            return 2;
        }
        return 0;
    }

    ret_code = revolver_open_items_file(&items_file, items_file_path, items_file_mode);
    if (0 > ret_code) {
        return 2;
    }

    if (do_shift) {
        for (i = 0; i < items_num; i++) {
            ret_code = revolver_shift_item(&item, items_file);
            if (0 != ret_code) {
                return 2;
            }
            if (NULL == item) break;
            printf("%s\n", item);
            free(item);
        }
        ret_code = revolver_close_items_file(items_file, items_file_path);
        if (0 != ret_code) {
            return 2;
        }
        return 0;
    }

    for (i = optind; i < argc; i++) {
        if (do_push) {
            ret_code = revolver_push_item(items_file, argv[i]);
        }
        if (0 != ret_code) {
            return 2;
        }
    }
    return 0;
}


void print_help(const char *prog_name) {
    printf("usage: [options] item1 [item2 [...]]\n\n");
    printf("options:\n");
    printf(" -f, --file <file>      Specify items file\n");
    printf(" -p, --push             Push items to file\n");
    printf(" -s, --shift [n=1]      Shift 'n' items from file\n");
    printf(" -r, --revolve <cmd>    Revolve using command <cmd>\n");
    printf(" -d, --debug            Output debug information\n");
    printf(" -V, --version          Show version information\n");
    printf(" -H, --help             Show help information\n");
    printf("\nexamples:\n");
    printf(" %s --file=urls.txt --shift=2       # using long options\n", prog_name);
    printf(" %s -furls.txt -s2                  # using short options\n", prog_name);
}
