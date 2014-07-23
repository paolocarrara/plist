#include "../include/plist.h"

#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline, nonnull)) static inline void p_config_set_default (struct p_config *new)
{
	new->min_s = 0;
	new->max_s = 0;
	new->total = 0;
	new->charsets.base_set = NULL;
	new->charsets.add_set = NULL;
	new->charsets.rm_set = NULL;
	new->charsets.sub_set = NULL;
}

__attribute__((malloc, warn_unused_result)) struct p_config *new_p_config (void)
{
	struct p_config *new = malloc (sizeof(struct p_config));
	
	if (new) {
		p_config_set_default (new);
	}

	return new;
}

__attribute__((flatten)) void p_config_set_size (uint8_t size, struct p_config *config) 
{
	if (config != NULL) {
		p_config_set_min_s (size, config);
		p_config_set_max_s (size, config);
	}
}

__attribute__((nonnull)) void p_config_set_min_s (uint8_t min, struct p_config *config)
{
	if (min > config->max_s) {
		p_config_set_max_s (min, config);
	}
	config->min_s = min;
}

__attribute__ ((nonnull, always_inline)) static inline void p_config_realloc_sets	(struct p_config *, uint8_t);
__attribute__ ((nonnull, always_inline)) static inline void p_config_null_sets		(struct p_config *, uint8_t);

__attribute__((nonnull)) void p_config_set_max_s (uint8_t max, struct p_config *config)
{
	if (config->max_s != max) {

		p_config_realloc_sets (config, max);

		if (config->max_s < max) {
			p_config_null_sets (config, max);
		}
		else {
			config->max_s = max;
		}
	}
	if (max < config->min_s) {
		config->min_s = config->max_s;
	}
}
__attribute__ ((nonnull, always_inline)) static inline void p_config_realloc_sets (struct p_config *config, uint8_t max)
{
	config->charsets.add_set = realloc ( config->charsets.add_set, max*sizeof (char *) );
	config->charsets.rm_set = realloc ( config->charsets.rm_set, max*sizeof (char *) );
	config->charsets.sub_set = realloc ( config->charsets.sub_set, max*sizeof (char *) );
}
__attribute__ ((nonnull, always_inline)) static inline void p_config_null_sets (struct p_config *config, uint8_t max)
{
	for (;config->max_s < max; config->max_s++) {
		config->charsets.add_set[config->max_s] = NULL;
		config->charsets.rm_set[config->max_s] = NULL;
		config->charsets.sub_set[config->max_s] = NULL;
	}
}

__attribute__((nonnull)) void p_config_base_set (int8_t *base_set, struct p_config *config)
{
	uint8_t len = strlen ((char*)base_set)+1;
	config->charsets.base_set = realloc (config->charsets.base_set, len*sizeof (char));
	memcpy (config->charsets.base_set, base_set, len);
}
__attribute__((nonnull)) void p_config_add_chars_in (int8_t *chars_to_add, uint8_t pos, struct p_config *config)
{
	uint8_t len = strlen ((char*)chars_to_add)+1;
	config->charsets.add_set[pos] = realloc (config->charsets.add_set[pos], len*sizeof (char));
	memcpy (config->charsets.add_set[pos], chars_to_add, len);
}
__attribute__((nonnull)) void p_config_rm_chars_from (int8_t *chars_to_rm, uint8_t pos, struct p_config *config)
{
	uint8_t len = strlen ((char*)chars_to_rm)+1;
	config->charsets.rm_set[pos] = realloc (config->charsets.rm_set[pos], len*sizeof (char));
	memcpy (config->charsets.rm_set[pos], chars_to_rm, len);
}
__attribute__((nonnull)) void p_config_sub_chars_from (int8_t *chars_to_sub, uint8_t pos, struct p_config *config)
{
	uint8_t len = strlen ((char*)chars_to_sub)+1;
	config->charsets.sub_set[pos] = realloc (config->charsets.sub_set[pos], len*sizeof (char));
	memcpy (config->charsets.sub_set[pos], chars_to_sub, len);
}

__attribute__((nonnull, always_inline, cold)) static inline void free_p_config_add_set	(struct p_config *);
__attribute__((nonnull, always_inline, cold)) static inline void free_p_config_rm_set		(struct p_config *);
__attribute__((nonnull, always_inline, cold)) static inline void free_p_config_sub_set	(struct p_config *);

__attribute__((flatten)) void free_p_config (struct p_config *config)
{
	free_p_config_add_set (config);
	free_p_config_rm_set (config);
	free_p_config_sub_set (config);
	free (config->charsets.base_set);
	free (config);
}

__attribute__((nonnull, always_inline)) static inline void free_p_config_add_set (struct p_config *config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.add_set[i]);
	}
	free (config->charsets.add_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_rm_set (struct p_config *config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.rm_set[i]);
	}
	free (config->charsets.rm_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_sub_set (struct p_config *config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.sub_set[i]);
	}
	free (config->charsets.sub_set);
}


__attribute__((nonnull, always_inline)) static inline void print_base_set (struct p_config *);
__attribute__((nonnull, always_inline)) static inline void print_add_set (struct p_config *);
__attribute__((nonnull, always_inline)) static inline void print_rm_set (struct p_config *);
__attribute__((nonnull, always_inline)) static inline void print_sub_set (struct p_config *);

__attribute__((flatten)) void print_p_config (struct p_config *config)
{
	if (config != NULL) {
		printf ("Minimum password size: %u\n", config->min_s);
		printf ("Maximum password size: %u\n", config->max_s);

		print_base_set (config);
		print_add_set (config);
		print_rm_set (config);
		print_sub_set (config);
	}
}

__attribute__((nonnull, always_inline)) static inline void print_base_set (struct p_config *config)
{
	printf ("\nBase set:\n\t%s\n", config->charsets.base_set);
}

__attribute__((nonnull, always_inline)) static inline void print_add_set (struct p_config *config)
{
	uint8_t i;

	printf ("\nAdds to the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.add_set[i]);
}

__attribute__((nonnull, always_inline)) static inline void print_rm_set (struct p_config *config)
{
	uint8_t i;

	printf ("\nRemoves from the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.rm_set[i]);
}

__attribute__((nonnull, always_inline)) static inline void print_sub_set (struct p_config *config)
{
	uint8_t i;

	printf ("\nSubstitutions to the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.sub_set[i]);
}

