#ifndef _REVOLVER_revolver_h_
#define _REVOLVER_revolver_h_ 1


#define REVOLVER_VERSION "0.1.0"


/**
 * Errors returned by our functions.
 */
/* Generic revolver error */
#define ERR_REV         -2
/* Error opening file */
#define ERR_REV_FOPEN   -3
/* Error closing file */
#define ERR_REV_FCLOSE  -4
/* Error reading file */
#define ERR_REV_FREAD   -5
/* Error writing to file */
#define ERR_REV_FWRITE  -6

/**
 * Items file open mode.
 */
/* Open file for pushing */
#define REVOLVER_O_PUSH     "a"
/* Open file for shifting */
#define REVOLVER_O_SHIFT    "r+"


/**
 * Open items file.
 *
 * @param  items_file       Opened file stream is placed here
 * @param  items_file_path  Path to the items file
 * @param  items_file_mode  File open mode. Either REVOLVER_O_PUSH or REVOLVER_O_SHIFT
 * @return 0 or error code
 *         ERR_REV_FOPEN - error opening the items file
 */
int revolver_open_items_file(FILE **items_file, const char *items_file_path, const char *items_file_mode);


/**
 * Close items file.
 *
 * @param  items_file       Items file stream (from revolver_open_items_file())
 * @param  items_file_path  Path to items file
 * @return 0 or error code
 *         ERR_REV_FOPEN - error opening temp file
 *         ERR_REV_FCLOSE - error closing items file or temp file
 *         ERR_REV_FREAD - error reading items file
 *         ERR_REV_FWRITE - error writing to temp file
 *         ERR_REV - any other error
 */
int revolver_close_items_file(FILE *items_file, const char *items_file_path);


/**
 * Push item to items file.
 *
 * @param  items_file       Items file stream
 * @param  item             Item
 * @return 0 or error code
 *         ERR_REV_FWRITE - error writing to items file
 */
int revolver_push_item(FILE *items_file, const char* item);


/**
 * Shift item from items file.
 *
 * @param  item             Item is placed here. NULL-terminated.
 * @param  items_file       Items file stream
 * @return 0 or error code
 *         ERR_REV_FREAD - error reading items file
 */
int revolver_shift_item(char **item, FILE *items_file);


/**
 * Revolve.
 *
 * @param  items_file_path  Path to items file
 * @param  command          Command to use for revolving e.g. "echo ="
 * @return 0 or error code
 *         ERR_REV - any other error
 */
int revolver_revolve(const char *items_file_path, const char *command);


#endif
