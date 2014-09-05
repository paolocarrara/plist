#ifndef PLIST_H
#define PLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <omp.h>

struct sets {
	int8_t *base_set;
	int8_t **sub_set;
};

struct p_config {
	uint8_t min_s;
	uint8_t max_s;
	#if __LP64__
	uint64_t total;
	#else
	uint32_t total;
	#endif
	struct sets charsets;
};

struct p_config_extra {
	uint8_t num_threads;
	uint8_t out;
	uint16_t buffer_size;
	uint8_t mode;
	uint8_t limit;
	#ifdef __LP64__
	uint64_t *remaining;
	#else
	uint32_t *remaining;
	#endif
	uint8_t save;
};

__attribute__((malloc, warn_unused_result))
struct p_config *new_p_config			(void);

__attribute__((malloc, warn_unused_result))
struct p_config_extra *new_p_config_extra	(void);

void recover 			(struct p_config * const, struct p_config_extra * const, const char * const);

__attribute__((flatten)) 
void p_config_set_min_s		(uint8_t, struct p_config *);
__attribute__((flatten)) 
void p_config_set_max_s		(uint8_t, struct p_config *);
__attribute__((flatten)) 
void p_config_set_size		(uint8_t, struct p_config *);

#if __LP64__
uint64_t p_config_get_total 	(const struct p_config *);
#else
uint32_t p_config_get_total 	(const struct p_config *);
#endif

__attribute__((flatten)) 
void p_config_base_set		(char *, struct p_config *);
__attribute__((flatten)) 
void p_config_sub_chars_from	(char *, uint8_t, struct p_config *);
__attribute__((flatten)) 
void p_config_rm_sub_set	(struct p_config *, uint8_t);
__attribute__((hot)) 
void generate 			(struct p_config *, int8_t, uint16_t, uint16_t);

__attribute__((flatten)) 
void free_p_config 		(struct p_config *);

__attribute__((flatten)) 
void print_p_config 		(const struct p_config *);

#endif
