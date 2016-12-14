#ifndef __LOG_H__
#define __LOG_H__
#include "string.h"

#define debug(msg, ...) printf("%s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define error(msg, ...) printf("%s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
