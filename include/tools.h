#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>

#include "colors.h"
#include "errors.h"

#define ERROR_CHECK_RET_STATUS(status)                                                                          \
        {                                                                                                       \
            enum Errors temp = status;                                                                          \
            if (temp != NO_ERRORS)                                                                              \
            {                                                                                                   \
                fprintf (stderr, "\n" PURPLE_TEXT ("in %s - %s:%d:") RED_TEXT (" ERROR <%d>:") " %s. " "\n\n",  \
                         __FILE__, __FUNCTION__, __LINE__, (int) temp, ErrorsMessenger(temp));                  \
                return temp;                                                                                    \
            }                                                                                                   \
        }

#define ERROR_MESSAGE(status)                                                                                   \
        {                                                                                                       \
            enum Errors temp = status;                                                                          \
            if (temp != NO_ERRORS)                                                                              \
                fprintf (stderr, "\n" PURPLE_TEXT ("in %s - %s:%d:") RED_TEXT (" ERROR <%d>:") " %s. " "\n\n",  \
                         __FILE__, __FUNCTION__, __LINE__, (int) temp, ErrorsMessenger(temp));                  \
        }

const char* ErrorsMessenger (enum Errors status);

#endif // TOOLS_H
