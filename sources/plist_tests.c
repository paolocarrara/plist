#include "../include/plist_tests.h"

uint8_t test__p_config_set_default (struct p_config *new)
{
	if (new->min_s == 0 && new->max_s == 0 && new->total == 0 && new->charsets.base_set == NULL && new->charsets.add_set == NULL && new->charsets.rm_set == NULL && new->charsets.sub_set == NULL) {
		printf ("TEST <p_config_set_default> -- OK\n");
		return 1;
	}
	else {
		printf ("TEST <p_config_set_default> -- NOTOK\n");
		return 0;
	}
}

uint8_t test__p_config_set_size (uint8_t expected_size, struct p_config *config)
{
	if (config->min_s == expected_size && config->max_s == expected_size) {
		printf ("TEST <p_config_set_size> -- OK\n");
		return 1;
	}
	else {
		printf ("TEST <p_config_set_size> -- NOTOK (%d, %d)\n", config->min_s, config->max_s);
		return 0;
	}
}

uint8_t test__p_config_set_min_s (uint8_t expected_min_s, uint8_t expected_max_s, struct p_config *config)
{
	if (config->min_s == expected_min_s && config->max_s == expected_max_s) {
		printf ("TEST <p_config_set_min_s> -- OK\n");
		return 1;
	}
	else {
		printf ("TEST <p_config_set_min_s> -- NOTOK (%d, %d)\n", config->min_s, config->max_s);
		return 0;
	}
}

uint8_t test__p_config_set_max_s (uint8_t expected_min_s, uint8_t expected_max_s, struct p_config *config)
{
	if (config->min_s == expected_min_s && config->max_s == expected_max_s) {
		printf ("TEST <p_config_set_max_s> -- OK\n");
		return 1;
	}
	else {
		printf ("TEST <p_config_set_max_s> -- NOTOK (%d, %d)\n", config->min_s, config->max_s);
		return 0;
	}
}

