#include <stdio.h>

#include "tools.h"
#include "errors.h"

const char* ErrorsMessenger (enum Errors status)
{
    switch (status)
    {
        case      NO_ERRORS: return "NO ERROR";
        case   NULL_PTR_ERR: return "NULL pointer detected";
        case  FILE_SIZE_ERR: return "File size is either not the same as expected or zero";
        case  FILE_OPEN_ERR: return "File failed to open";
        case FILE_CLOSE_ERR: return "File failed to close";
        case CREATE_BUF_ERR: return "Buffer has not been created";
        case   READ_BUF_ERR: return "Reading a file into the buffer failed";
        case  WRITE_BUF_ERR: return "Writing to the buffer failed";

        default:             return "UNDEFINED ERROR";
    }
}


