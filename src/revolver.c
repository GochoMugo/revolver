#include "main.h"


int DEBUG = 0;


int revolver_open_items_file(FILE **items_file, const char *items_file_path, const char *items_file_mode) {
    FILE *file = fopen(items_file_path, items_file_mode);
    if (NULL == file) {
        perror("open items file");
        return ERR_REV_FOPEN;
    }
    *items_file = file;
    return 0;
}


int revolver_close_items_file(FILE *items_file, const char *items_file_path) {
    int ret_code = 0;
    int items_file_fd;
    int items_file_stats;
    char temp_file_path[] = ".revolver-XXXXXX";
    int temp_file_fd;
    FILE *temp_file = NULL;
    char *lineptr = NULL;
    size_t n = 0;
    int lines = 0;

    items_file_fd = fileno(items_file);
    if (0 > items_file_fd) {
        perror("close_items_file");
        return ERR_REV;
    }

    items_file_stats = fcntl(items_file_fd, F_GETFL);
    if (0 > items_file_stats) {
        perror("close_items_file");
        return ERR_REV;
    }

    if (O_APPEND & items_file_stats) {
        ret_code = fclose(items_file);
        if (0 > ret_code) {
            perror("close_items_file");
            return ERR_REV_FCLOSE;
        }
        return 0;
    }

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


int revolver_push_item(FILE *items_file, const char *item) {
    int ret_code = 0;

    ret_code = fprintf(items_file, "%s\n", item);
    if (0 > ret_code) {
        perror("push_item");
        return ERR_REV_FWRITE;
    }

    return 0;
}


int revolver_shift_item(char **out, FILE *items_file) {
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


int revolver_revolve(const char *items_file_path, const char *command) {
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
        ret_code = revolver_open_items_file(&items_file, items_file_path, "r+");
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

            ret_code = revolver_push_item(items_file, item);
            if (0 > ret_code) return ERR_REV;

            ret_code = fseek(items_file, 0, SEEK_SET);
            if (0 > ret_code) return ERR_REV;
        }

        if (stop) {
            printf("revolver: Stopping...\n");
            break;
        }

        ret_code = revolver_shift_item(&item, items_file);
        if (0 > ret_code) return ERR_REV;

        ret_code = revolver_close_items_file(items_file, items_file_path);
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
