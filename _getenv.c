#include "main.h"

char *_getenv(const char *name) {
   size_t name_length;
   int i;

    if (name == NULL || environ == NULL) {
        return NULL;
    }

    name_length = strlen(name);

    for (i = 0; environ[i] != NULL; ++i) {
        if (strncmp(environ[i], name, name_length) == 0 && environ[i][name_length] == '=') {
            
            return environ[i] + name_length + 1;
        }
    }

    return NULL;
}
