#include <stdlib.h>
#include <string.h>

/**
 * Utility functions that everyone might need
 */

char *str_duplicate(const char *src)
{
    char *dest;
    
    if (!src)
        return NULL;
    
    dest = malloc(strlen(src) + 1);
    if (dest)
        strcpy(dest, src);
    
    return dest;
}
