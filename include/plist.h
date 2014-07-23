#ifndef PLIST_H
#define PLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct sets {
	uint8_t *base_set;
	uint8_t **add_set;
	uint8_t **rm_set;
	uint8_t **sub_set;

};

struct p_config {
	uint8_t min_s;
	uint8_t max_s;
	#if __LP64__
	uint64_t
	#else
	uint32_t
	#endif
		total;
	struct sets charsets;
};

struct p_config *new_p_config (void);

void p_config_set_min_s (uint8_t, struct p_config *);
void p_config_set_max_s (uint8_t, struct p_config *);
void p_config_set_size (uint8_t, struct p_config *);

void free_p_config (struct p_config *);

#endif
