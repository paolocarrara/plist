#ifndef PLIST_ERR_H
#define PLIST_ERR_H

#define PLIST_ERR_NULL 		1
#define PLIST_ERR_NULL_MALLOC	2
#define PLIST_ERR_NDX_OUT	3

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void plist_err (uint8_t);

#endif
