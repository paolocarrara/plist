#include "../include/plist.h"

#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline, nonnull)) static __inline__ void p_config_set_default (struct p_config *new)
{
	new->min_s = 0;
	new->max_s = 0;
	new->total = 0;
	new->char_set = NULL;
	new->char_pos = NULL;
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

__attribute__((nonnull)) void p_config_set_min_s (uint8_t min, struct p_config *config)
{
	config->min_s = min;
	if (config->min_s > config->max_s)
		config->max_s = config->min_s;
}

__attribute__((nonnull)) void p_config_set_max_s (uint8_t max, struct p_config *config)
{
	config->max_s = max;
	if (config->min_s > config->max_s)
		config->min_s = config->max_s;
}

__attribute__((flatten, nonnull)) void p_config_set_size (uint8_t size, struct p_config *config) 
{
	p_config_set_min_s (size, config);
	p_config_set_max_s (size, config);
}
