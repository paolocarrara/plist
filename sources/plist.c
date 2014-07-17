#include "../include/plist.h"

#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline)) static __inline__ void p_config_set_default (struct p_config *new)
{
	new->min_s = 0;
	new->max_s = 0;
	new->total = 0;
	new->char_set = NULL;
	new->char_pos = NULL;
}

struct p_config *new_p_config (void)
{

	struct p_config *new = malloc (sizeof(struct p_config));

	p_config_set_default (new);

	return new;
}
