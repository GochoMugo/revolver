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
        ret_code = revolve(items_file_path, revolve_command);
        if (0 > ret_code) {
            return 2;
        }
        return 0;
    }

    ret_code = open_items_file(&items_file, items_file_path, items_file_mode);
    if (0 > ret_code) {
        return 2;
    }

    if (do_shift) {
        for (i = 0; i < items_num; i++) {
            ret_code = shift_item(&item, items_file);
            if (0 != ret_code) {
                return 2;
            }
            if (NULL == item) break;
            printf("%s", item);
            free(item);
        }
        ret_code = close_items_file(items_file, items_file_path);
        if (0 != ret_code) {
            return 2;
        }
        return 0;
    }

    for (i = optind; i < argc; i++) {
        if (do_push) {
            ret_code = push_item(items_file, argv[i]);
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


int open_items_file(FILE **items_file, const char *items_file_path, const char *items_file_mode) {
    FILE *file = fopen(items_file_path, items_file_mode);
    if (NULL == file) {
        perror("open items file");
        return ERR_REV_FOPEN;
    }
    *items_file = file;
    return 0;
}


int close_items_file(FILE *items_file, const char *items_file_path) {
    int ret_code = 0;
    char temp_file_path[] = ".revolver-XXXXXX";
    int temp_file_fd;
    FILE *temp_file = NULL;
    char *lineptr = NULL;
    size_t n = 0;
    int lines = 0;

    temp_file_fd = mkstemp(temp_file_path);
    if (0 > temp_file_fd) {
        perror("close_items_file");
        return ERR_REV;
    }

    temp_file = fdopen(temp_file_fd, "w");
    if (NULL == temp_file) {
        perror("close_items_file");
        return ERR_REV_FOPEN;
    }

    while (getline(&lineptr, &n, items_file) != -1) {
        ret_code = fprintf(temp_file, "%s", lineptr);
        if (0 > ret_code) {
            perror("close_items_file");
            return ERR_REV_FWRITE;
        }
        lines++;
    }
    if (0 != errno) {
        perror("close_items_file");
        return ERR_REV_FREAD;
    }

    ret_code = fclose(items_file);
    if (0 > ret_code) {
        perror("close_items_file");
        return ERR_REV_FCLOSE;
    }

    ret_code = fclose(temp_file);
    if (0 > ret_code) {
        perror("close_items_file");
        return ERR_REV_FCLOSE;
    }

    ret_code = rename(temp_file_path, items_file_path);
    if (0 > ret_code) {
        perror("close_items_file");
        return ERR_REV;
    }

    if (0 == lines) {
        ret_code = unlink(items_file_path);
        if (0 > ret_code) {
            perror("close_items_file");
            return ERR_REV;
        }
    }

    return 0;
}


int push_item(FILE *items_file, const char *item) {
    int ret_code = 0;

    ret_code = fprintf(items_file, "%s\n", item);
    if (0 > ret_code) {
        perror("push_item");
        return ERR_REV_FWRITE;
    }

    return 0;
}


int shift_item(char **out, FILE *items_file) {
    int ret_code = 0;
    char *lineptr = NULL;
    size_t n = 0;

    ret_code = getline(&lineptr, &n, items_file);
    if (0 > ret_code && 0 != errno) {
        perror("shift_item");
        return ERR_REV_FWRITE;
    }

    lineptr[strlen(lineptr)-1] = 0;

    *out = lineptr;
    return 0;
}


int revolve(const char *items_file_path, const char *command) {
    int ret_code = 0;
    FILE *items_file = NULL;
    char *item = NULL;
    char *actual_command = NULL;
    FILE *pstream = NULL;
    char *pline = NULL;
    size_t pn = 0;
    sig_atomic_t skip = 0;
    sig_atomic_t stop = 0;
    struct sigaction sig_action_int;
    struct sigaction sig_action_chld;

    void sig_handler_int(int sig_num) {
        if (skip) stop = 1;
        skip = 1;
    }
    void sig_handler_chld(int sig_num) {
        errno = 0;
    }

    memset(&sig_action_int, 0, sizeof(sig_action_int));
    memset(&sig_action_chld, 0, sizeof(sig_action_chld));

    sig_action_int.sa_handler = &sig_handler_int;
    sig_action_chld.sa_handler = &sig_handler_chld;

    ret_code = sigaction(SIGINT, &sig_action_int, NULL);
    if (0 > ret_code) return -1;

    ret_code = sigaction(SIGCHLD, &sig_action_chld, NULL);
    if (0 > ret_code) return -1;

    while (1) {
        ret_code = open_items_file(&items_file, items_file_path, "r+");
        if (0 > ret_code) {
            if (ERR_REV_FOPEN == ret_code) break;
            return ERR_REV;
        }

        if (skip) {
            printf("revolver: Skipping...\n");

            sleep(2);
            skip = 0;

            ret_code = fseek(items_file, 0, SEEK_END);
            if (0 > ret_code) return ERR_REV;

            ret_code = push_item(items_file, item);
            if (0 > ret_code) return ERR_REV;

            ret_code = fseek(items_file, 0, SEEK_SET);
            if (0 > ret_code) return ERR_REV;
        }

        if (stop) {
            printf("revolver: Stopping...\n");
            break;
        }

        ret_code = shift_item(&item, items_file);
        if (0 > ret_code) return ERR_REV;

        ret_code = close_items_file(items_file, items_file_path);
        if (0 > ret_code) return ERR_REV;

        ret_code = asprintf(&actual_command, "%s %s", command, item);
        if (0 > ret_code) return ERR_REV;

        printf("revolver: running command `%s'\n", actual_command);

        pstream = popen(actual_command, "r");
        if (NULL == pstream) return ERR_REV;

        while ((getline(&pline, &pn, pstream)) != -1) {
            printf("%s", pline);
        }

        ret_code = pclose(pstream);
        if (0 > ret_code) return ERR_REV;
        if (0 != WEXITSTATUS(ret_code)) {
            fprintf(stderr, "revolver: Command exited with non-zero status code\n");
        }
    }
    return 0;
}
