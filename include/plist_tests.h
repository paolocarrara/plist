#ifndef PLIST_TESTS_H
#define PLIST_TESTS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "plist.h"

uint8_t test__p_config_set_default	(struct p_config *);
uint8_t test__p_config_set_size		(uint8_t, struct p_config *);
uint8_t test__p_config_set_min_s	(uint8_t, uint8_t, struct p_config *);
uint8_t test__p_config_set_max_s	(uint8_t, uint8_t, struct p_config *);
uint8_t test__p_config_set_total	(uint64_t, struct p_config *);

#endif
