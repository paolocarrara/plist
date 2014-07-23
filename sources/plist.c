#include "../include/plist.h"

#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline, nonnull)) static __inline__ void p_config_set_default (struct p_config *new)
{
	new->min_s = 0;
	new->max_s = 0;
	new->total = 0;
	new->charsets.base_set = NULL;
	new->charsets.add_set = NULL;
	new->charsets.rm_set = NULL;
	new->charsets.sub_set = NULL;
}

__attribute__((malloc)) struct p_config *new_p_config (void)
{
	struct p_config *new = malloc (sizeof(struct p_config));

	/*TO DO: ERROR HANDLING*/
	/*if (new)
		p_config_set_default (new);
	else
		plist_fatal (NULL_POINTER);*/
	p_config_set_default (new);

	return new;
}

__attribute__((flatten, nonnull)) void p_config_set_size (uint8_t size, struct p_config *config) 
{
	p_config_set_min_s (size, config);
	p_config_set_max_s (size, config);
}

__attribute__((nonnull)) void p_config_set_min_s (uint8_t min, struct p_config *config)
{
	if (min > config->max_s) {
		p_config_set_max_s (min, config);
	}
	config->min_s = min;
}

__attribute__((nonnull)) void p_config_set_max_s (uint8_t max, struct p_config *config)
{
	if (config->max_s != max) {
		config->charsets.add_set = realloc ( config->charsets.add_set, max*sizeof (char *) );
		config->charsets.rm_set = realloc ( config->charsets.rm_set, max*sizeof (char *) );
		config->charsets.sub_set = realloc ( config->charsets.sub_set, max*sizeof (char *) );
		if (config->max_s < max) {
			for (;config->max_s < max; config->max_s++) {
				config->charsets.add_set[config->max_s] = NULL;
				config->charsets.rm_set[config->max_s] = NULL;
				config->charsets.sub_set[config->max_s] = NULL;
			}
		}
		else {
			config->max_s = max;
		}
	}
	if (max < config->min_s) {
		config->min_s = config->max_s;
	}
}

__attribute__((nonnull)) void p_config_set_charset (int8_t *compressed_char_sets, struct p_config *config)
{
	
	/*add_char_set ("[asdf]", -1, );*/

	/*[a-z]*/
	/*[A-Z]*/
	/*[alpha]*/
	/*[ALPHA]*/
	/*[AlPhA]*/
	/*[num]*/
	/*[NUM]*/
	/*[NuM]*/
	/*[alnum]*/
	/*[AlNuM]*/	
}

__attribute__((nonnull, always_inline)) static inline void free_p_config_add_set	(struct p_config *);
__attribute__((nonnull, always_inline)) static inline void free_p_config_rm_set		(struct p_config *);
__attribute__((nonnull, always_inline)) static inline void free_p_config_sub_set	(struct p_config *);

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
	int i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.add_set[i]);
	}
	free (config->charsets.add_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_rm_set (struct p_config *config)
{
	int i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.rm_set[i]);
	}
	free (config->charsets.rm_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_sub_set (struct p_config *config)
{
	int i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.sub_set[i]);
	}
	free (config->charsets.sub_set);
}

