#ifndef _MDATA_H
#define _MDATA_H

#include <stdint.h>

#include "mfile.h"

struct mdata_file;
typedef struct mdata_file mdata_file_t;

void mdata_init(void);
void mdata_add_extension_handler(const char *dir, const char *ext, bool (*mdata_file_creator)(int num_files, mfile_t *files, bool *file_changed,  mdata_file_t **mdata_files, void *udata), void *udata);

void mdata_file_add_val_int(mdata_file_t *mdata_file, const char *field, int val);
void mdata_file_add_val_uint64(mdata_file_t *mdata_file, const char *field, uint64_t val);
void mdata_file_add_val_binary_data(mdata_file_t *mdata_file, const char *field, char *data, int data_len, bool compress);

#endif