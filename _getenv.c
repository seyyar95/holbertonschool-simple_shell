#include "main.h"

char *_getenv(const char *name) {
    

    if (name == NULL || environ == NULL) {
        return NULL;
    }

    size_t name_length = strlen(name);

    for (int i = 0; environ[i] != NULL; ++i) {
        if (strncmp(environ[i], name, name_length) == 0 && environ[i][name_length] == '=') {
            
            return environ[i] + name_length + 1;
        }
    }

    return NULL;
}
