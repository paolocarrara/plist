#include "../include/plist_tests.h"

uint8_t test__p_config_set_default (struct p_config *new)
{
	if (new->min_s == 0 && new->max_s == 0 && new->total == 0 && new->char_set == NULL && new->char_pos == NULL) {
		printf ("TEST P CONFIG SET DEFAULT -- OK\n");
		return 1;
	}
	else {
		printf ("TEST P CONFIG SET DEFAULT -- NOTOK\n");
		return 0;
	}
}
