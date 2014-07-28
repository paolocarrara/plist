#ifndef PLIST_H
#define PLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct sets {
	int8_t *base_set;
	int8_t **add_set;
	int8_t **rm_set;
	int8_t **sub_set;
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

__attribute__((malloc, warn_unused_result)) struct p_config *new_p_config (void);

__attribute__((flatten)) void p_config_set_min_s	(uint8_t, struct p_config *);
__attribute__((flatten)) void p_config_set_max_s	(uint8_t, struct p_config *);
__attribute__((flatten)) void p_config_set_size		(uint8_t, struct p_config *);

#if __LP64__
uint64_t
#else
uint32_t
#endif
	p_config_get_total (const struct p_config *);

__attribute__((flatten)) void p_config_base_set 	(char *, struct p_config *);
__attribute__((flatten)) void p_config_add_chars_in	(char *, uint8_t, struct p_config *);
__attribute__((flatten)) void p_config_rm_chars_from	(char *, uint8_t, struct p_config *);
__attribute__((flatten)) void p_config_sub_chars_from	(char *, uint8_t, struct p_config *);

__attribute__((hot)) void generate (struct p_config *, int8_t, uint8_t);

__attribute__((flatten)) void free_p_config (struct p_config *);

__attribute__((flatten)) void print_p_config (const struct p_config *);

#endif
