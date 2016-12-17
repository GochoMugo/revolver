#ifndef _REVOLVER_revolver_h_
#define _REVOLVER_revolver_h_ 1


#define REVOLVER_VERSION "0.0.0"


#define ERR_REV         -2
#define ERR_REV_FOPEN   -3
#define ERR_REV_FCLOSE  -4
#define ERR_REV_FREAD   -5
#define ERR_REV_FWRITE  -6


int revolver_open_items_file(FILE **items_file, const char *items_file_path, const char *items_file_mode);
int revolver_close_items_file(FILE *items_file, const char *items_file_path);
int revolver_push_item(FILE *items_file, const char* item);
int revolver_shift_item(char **out, FILE *items_file);
int revolver_revolve(const char *items_file_path, const char *command);


#endif
