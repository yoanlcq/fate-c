#ifndef FATE_FILE_TO_STRING_H
#define FATE_FILE_TO_STRING_H

#include <stdio.h>
#include <stdint.h>

char **fate_file_to_string_array(FILE *file, size_t *num_strings);
char *fate_file_to_string(FILE *file, uint64_t *length);


#endif /* FATE_FILE_TO_STRING_H */
