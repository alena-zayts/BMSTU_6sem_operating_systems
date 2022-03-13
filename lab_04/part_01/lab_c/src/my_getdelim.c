#include "my_getdelim.h"

int my_getdelim(char **lineptr, size_t *n, int delim, FILE *stream)
{
    *lineptr = NULL;
    char *temp = NULL;
    char c;
    int rc = OK;

    size_t len = 0;
    while (((c = fgetc(stream)) != delim) && (c != EOF) && (rc == OK))
    {
        len++;
        temp = realloc((*lineptr), (len + 1) * sizeof(char));
        if (temp != NULL)
        {
            (*lineptr) = temp;
            (*lineptr)[len - 1] = c;
        }
        else
        {
            free(*lineptr);
            rc = ERRMEM;
        }
    }
    if (rc == OK)
    {
        if (len == 0)
        {
            if (feof(stream))
                rc = END;
            else
                rc = ERRVALUE;
        }
        else
            (*lineptr)[len] = '\0';
    }
    *n = len;
    return rc;
}
