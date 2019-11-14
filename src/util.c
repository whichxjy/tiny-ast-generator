#include "util.h"
#include <stdlib.h>
#include <string.h>

// copy a string
char* copy_string(char *src) {
    if (src == NULL) {
        return NULL;
    }
    else {
        char *target = (char *)malloc(strlen(src) + 1);
        strcpy(target, src);
        return target;
    }
}