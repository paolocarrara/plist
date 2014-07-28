#include "../include/plist.h"
#include "../include/plist_err.h"
#include <time.h>
#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline, nonnull)) static inline void p_config_set_default (struct p_config * const new) 
{
	if (new) {
		new->min_s = 0;
		new->max_s = 0;
		new->total = 0;
		new->charsets.base_set = NULL;
		new->charsets.add_set = NULL;
		new->charsets.rm_set = NULL;
		new->charsets.sub_set = NULL;
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((malloc, warn_unused_result)) struct p_config *new_p_config (void)
{
	struct p_config *new = malloc (sizeof(struct p_config));

	if (new) {
		p_config_set_default (new);
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return new;
}

__attribute__((flatten)) static inline void p_config_set_total (struct p_config * const);

__attribute__((flatten)) void p_config_set_size (const uint8_t size, struct p_config * const config) 
{
	if (config) {
		p_config_set_min_s (size, config);
		p_config_set_max_s (size, config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) void p_config_set_min_s (const uint8_t min, struct p_config * const config)
{
	if (config) {
		if (min > config->max_s) {
			p_config_set_max_s (min, config);
		}
		config->min_s = min;
		p_config_set_total (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, always_inline)) static inline void p_config_realloc_sets	(struct p_config * const, const uint8_t);
__attribute__((nonnull, always_inline)) static inline void p_config_null_sets		(struct p_config * const, const uint8_t);

__attribute__((flatten)) void p_config_set_max_s (const uint8_t max, struct p_config *config)
{
	if (config) {
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
		p_config_set_total (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, always_inline)) static inline void p_config_realloc_sets (struct p_config * const config, const uint8_t max)
{
	config->charsets.add_set = realloc ( config->charsets.add_set, max*sizeof (int8_t *) );
	config->charsets.rm_set = realloc ( config->charsets.rm_set, max*sizeof (int8_t *) );
	config->charsets.sub_set = realloc ( config->charsets.sub_set, max*sizeof (int8_t *) );
}

__attribute__((nonnull, always_inline)) static inline void p_config_null_sets (struct p_config * const config, const uint8_t max)
{
	for (;config->max_s < max; config->max_s++) {
		config->charsets.add_set[config->max_s] = NULL;
		config->charsets.rm_set[config->max_s] = NULL;
		config->charsets.sub_set[config->max_s] = NULL;
	}
}

__attribute__((flatten)) void p_config_base_set (char * const base_set, struct p_config * const config)
{
	if (config) {/*maybe free, checkt that*/
		uint8_t len = strlen (base_set)+1;
		config->charsets.base_set = realloc (config->charsets.base_set, len*sizeof (int8_t));
		memcpy (config->charsets.base_set, base_set, len);
		p_config_set_total (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) void p_config_add_chars_in (char * const chars_to_add, const uint8_t ndx, struct p_config * const config)
{
	if (config && ndx < config->max_s) {
		uint8_t len = strlen (chars_to_add)+1;
		config->charsets.add_set[ndx] = realloc (config->charsets.add_set[ndx], len*sizeof (int8_t));
		memcpy (config->charsets.add_set[ndx], chars_to_add, len);
		p_config_set_total (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) void p_config_rm_chars_from (char * const chars_to_rm, const uint8_t ndx, struct p_config * const config)
{
	if (config && ndx < config->max_s) {
		uint8_t len = strlen (chars_to_rm)+1;
		config->charsets.rm_set[ndx] = realloc (config->charsets.rm_set[ndx], len*sizeof (int8_t));
		memcpy (config->charsets.rm_set[ndx], chars_to_rm, len);
		p_config_set_total (config);
	}
	else if (ndx >= config->max_s){
		plist_err (PLIST_ERR_NDX_OUT);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) void p_config_sub_chars_from (char * const chars_to_sub, const uint8_t ndx, struct p_config * const config)
{
	if (config && ndx < config->max_s) {
		uint8_t len = strlen (chars_to_sub)+1;
		config->charsets.sub_set[ndx] = realloc (config->charsets.sub_set[ndx], len*sizeof (int8_t));
		memcpy (config->charsets.sub_set[ndx], chars_to_sub, len);
		p_config_set_total (config);
	}
	else if (ndx >= config->max_s){
		plist_err (PLIST_ERR_NDX_OUT);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, malloc, flatten, always_inline)) static inline uint8_t *malloc_lengths	(const struct p_config * const);
__attribute__((nonnull, always_inline)) static inline void sub_total_from_zero_to_min_s		(struct p_config * const, const uint8_t * const);
__attribute__((nonnull, always_inline)) static inline void sub_total_from_min_s_to_max_s	(struct p_config * const, const uint8_t * const);

__attribute__((flatten)) static inline void p_config_set_total (struct p_config * const config) /*Tenho que fazer com que essa função possa ser nonnull*/
{
	uint8_t *lengths;

	if (config->charsets.base_set) {

		lengths = malloc_lengths (config);

		sub_total_from_zero_to_min_s (config, lengths);

		sub_total_from_min_s_to_max_s (config, lengths);

		free (lengths);
	}
}

#if __LP64__
uint64_t
#else
uint32_t
#endif
	p_config_get_total (const struct p_config * const config)
{ return config->total; }

__attribute__((nonnull, always_inline)) static inline uint8_t pos_length (const struct p_config * const, const uint8_t);

__attribute__((nonnull, malloc, flatten)) static inline uint8_t *malloc_lengths (const struct p_config * const config)
{
	uint8_t i;
	uint8_t *lengths = malloc (config->max_s*sizeof (uint8_t));

	if (lengths) {
		for (i = 0; i < config->max_s; i++) {
			lengths[i] = pos_length (config, i);
		}
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return lengths;
}

__attribute__((always_inline, nonnull)) static inline uint8_t noec	(const int8_t * const, const int8_t * const); /*noec = number of equal chars*/
__attribute__((always_inline, nonnull)) static inline int8_t *cnt	(const int8_t * const, const int8_t * const); /*concatenates*/

__attribute__((nonnull, always_inline)) static inline uint8_t pos_length (const struct p_config * const config, const uint8_t ndx)
{
	uint8_t len = 0;

	if (config->charsets.sub_set[ndx]) {
		len = strlen ((char *) config->charsets.sub_set[ndx]);

		if (config->charsets.add_set[ndx] && config->charsets.rm_set[ndx]) {
			int8_t *cntn = cnt (config->charsets.sub_set[ndx], config->charsets.add_set[ndx]);
			len -= noec (cntn, config->charsets.rm_set[ndx]);
			len += strlen ((char *) config->charsets.add_set[ndx]);
			free (cntn);
		}
		else if (config->charsets.rm_set[ndx]) {
			len -= noec (config->charsets.sub_set[ndx], config->charsets.rm_set[ndx]);
		}
		else if (config->charsets.add_set[ndx]) {
			len += ( strlen ((char *) config->charsets.add_set[ndx]) - noec (config->charsets.add_set[ndx], config->charsets.sub_set[ndx]) );
		}
	}
	else {
		len = strlen ((char *) config->charsets.base_set);

		if (config->charsets.add_set[ndx] && config->charsets.rm_set[ndx]) {
			int8_t *cntn = cnt (config->charsets.base_set, config->charsets.add_set[ndx]);
			len -= noec (cntn, config->charsets.rm_set[ndx]);
			len += strlen ((char *) config->charsets.add_set[ndx]);
			free (cntn);
		}
		else if (config->charsets.rm_set[ndx]) {
			len -= noec (config->charsets.base_set, config->charsets.rm_set[ndx]);
		}
		else if (config->charsets.add_set[ndx]) {
			len += ( strlen ((char *) config->charsets.add_set[ndx]) - noec (config->charsets.add_set[ndx], config->charsets.base_set) );
		}
	}

	return len;
}

__attribute__((always_inline)) static inline uint8_t noec (const int8_t * const str_1, const int8_t * const str_2)
{
	uint8_t n_equals = 0;
	uint8_t len_1 = strlen ((char *) str_1);
	uint8_t len_2 = strlen ((char *) str_2);
	uint8_t i, j;

	for (i = 0; i < len_1; i++) {
		for (j = 0; j < len_2; j++) {
			if (str_2[j] == str_1[i]) {
				n_equals++;
				j = len_2;
			}
		}
	}

	return n_equals;
}

__attribute__((always_inline)) static inline int8_t *cnt (const int8_t * const str_1, const int8_t * const str_2)
{
	uint8_t len_cntn = strlen ((char*) str_1) + strlen ((char*) str_2) + 1;
	int8_t *cntn = malloc (len_cntn*sizeof(int8_t));
	
	if (cntn) {
		memcpy (cntn, str_1, strlen ((char*) str_1)+1);
		strcat ((char *) cntn, (char *) str_2);
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return cntn;
}

__attribute__((nonnull, always_inline)) static inline void sub_total_from_zero_to_min_s (struct p_config * const config, const uint8_t * const lengths)
{
	uint8_t i;

	if (config->min_s != 0) {
		for (i = 0, config->total = 1; i < config->min_s; i++) {
			config->total *= lengths[i];
		}
	}
	else {
		config->total = 0;
	}
}

__attribute__((nonnull, always_inline)) static inline void sub_total_from_min_s_to_max_s (struct p_config * const config, const uint8_t * const lengths)
{
	uint8_t i;
	#if __LP64__
	uint64_t
	#else
	uint32_t
	#endif 
		pre_state;
	
	pre_state = (config->total == 0) ? 1 : config->total;

	for (i = config->min_s; i < config->max_s; i++) {
		config->total += pre_state*lengths[i];
		pre_state *= lengths[i];
	}
}

__attribute__((always_inline, nonnull)) static inline /*const*/ uint8_t **p_config_compile (const struct p_config * const config)
{
	uint8_t **compiled_password;
	uint8_t *len;
	uint8_t i;

	compiled_password = malloc ((config->max_s+1)*sizeof(uint8_t *));
	len = malloc (config->max_s*sizeof(uint8_t));

	if (compiled_password && len) {
	
		for (i = 0; i < config->max_s; i++) {
			len[i] = strlen((char *)config->charsets.base_set);
			compiled_password[i] = malloc ((len[i]+1)*sizeof(uint8_t));
			memcpy (compiled_password[i], config->charsets.base_set, len[i]+1);
		}
		compiled_password[i] = NULL;

		free (len);
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return /*(const uint8_t **)*/compiled_password;
}

static uint8_t limit;
static uint8_t *buff;
static uint8_t buffs;
static uint8_t buffl;

__attribute__((always_inline)) static inline void valid_config				(const struct p_config * const);
__attribute__((nonnull, hot)) static void check_bounds					(uint8_t **, const uint8_t ** const, const uint8_t * const, uint8_t);
__attribute__((always_inline, nonnull, malloc)) static inline const uint8_t *lengths	(const struct p_config * const, uint8_t ** const);
__attribute__((always_inline, nonnull, malloc)) static inline const uint8_t **boundS	(const struct p_config * const, uint8_t ** const, const uint8_t * const);
#ifdef __LP64__
__attribute__((always_inline, nonnull)) static inline uint64_t initial_total (const struct p_config * const, const uint8_t * const);
#else
__attribute__((always_inline, nonnull)) static inline uint32_t initial_total (const struct p_config * const, const uint8_t * const);
#endif
__attribute__((always_inline, flatten, nonnull, hot)) static inline void buff_password	(uint8_t ** const, const int8_t);
__attribute__((always_inline, malloc)) static inline uint8_t *buffer 			(const uint8_t);
__attribute__((always_inline, hot)) static inline void buffout				(const int8_t);

__attribute__((hot)) void generate (struct p_config *config, int8_t fd, uint8_t buff_s)
{
	/*const*/ uint8_t **password;
	const uint8_t **bounds;
	const uint8_t *len;
	uint8_t min, max;
	clock_t t;
	#if __LP64__
	uint64_t total, i;
	#else
	uint32_t total, i;
	#endif

	valid_config (config);

	/*Set up the buffer*/
	buff = buffer (buff_s);

	/*Set up the password*/
	password = p_config_compile (config);

	/*Get the length of every position set in the password*/
	len = lengths (config, password);

	/*Get the bounds of every position set in the password*/
	bounds = boundS (config, password, len);

	/*Get the initial total of passwords to generate*/
	total = initial_total (config, len);

	t = clock (); /*<-- Not needed*/
	for (limit = min = config->min_s, max = config->max_s; min <= max; min++, limit++) {

		total *= len[min-1];
		i = total;

		while (i-->0) {
			buff_password (password, fd);
			check_bounds (password, bounds, len, 0);
			password[0]++;
		}
		if (min < max)
			password[min]--;
	}
	buffout (fd); /*Cleans the buffer*/
	t = clock() - t; /*<-- Not needed*/

	printf ("TIME ELAPSED: %f\n", (float)t/CLOCKS_PER_SEC); /*<-- Not needed*/

	/*Frees*/
	free ((void *) len);
	free (buff);
	free (bounds);
	free (password);
}

__attribute__((always_inline, flatten, nonnull, hot)) static inline void buff_password (uint8_t ** const password, const int8_t fd)
{
	uint8_t i;
	for (i = 0; i < limit; i++, buffl++, buff++)
		memcpy (buff, password[i], 1); 

	/*Ugly. maybe don't do it, or make it optional..*/
	memcpy (buff, "\n", 1);
	buffl++;
	buff++;

	if (buffl >= buffs-limit) {
		buffout (fd);
	}
}

__attribute__((always_inline, hot)) static inline void buffout (const int8_t fd)
{
	buff -= buffl;
	write (fd, buff, buffl); /*maybe: buffl -= write (fd, buff, buffl)*/
	buffl = 0;
}

__attribute ((always_inline, malloc, alloc_size(1))) static inline uint8_t *buffer (const uint8_t buff_s)
{
	uint8_t *aux;

	buffl = 0;
	buffs = buff_s;
	aux = malloc (buff_s*sizeof (uint8_t));

	if (!aux) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return aux;
}

__attribute__((nonnull, hot)) static void check_bounds (uint8_t **compiled_password, const uint8_t ** const bounds, const uint8_t * const len, uint8_t ndx)
{
	if (compiled_password[ndx] == bounds[ndx]) {
		compiled_password[ndx] -= len[ndx];
		check_bounds (compiled_password, bounds, len, ++ndx);
		compiled_password[ndx]++;
	}
}

__attribute__((always_inline, nonnull, malloc)) static inline const uint8_t *lengths (const struct p_config *config, uint8_t ** const password) /*const*/
{
	uint8_t i, *lengths;
	
	lengths =  malloc (config->max_s*sizeof (uint8_t));

	if (!lengths) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	else {
		for (i = 0; i < config->max_s; i++) {
			lengths[i] = strlen ((char *) password[i]);
		}
	}

	return lengths;
}

#ifdef __LP64__
__attribute__((always_inline, nonnull)) static inline uint64_t initial_total (const struct p_config * const config, const uint8_t * const len)
#else
__attribute__((always_inline, nonnull)) static inline uint32_t initial_total (const struct p_config * const config, const uint8_t * const len)
#endif
{
	uint8_t i;
	#ifdef __LP64__
	uint64_t
	#else
	uint32_t
	#endif
		total;
	if (config->min_s > 1) {
		for (i = 0, total = 1; i < config->min_s-1; i++) {
			total *=len[i];
		}
	}
	else {
		total = 1;
	}
	return total;
}

__attribute__((always_inline, nonnull, malloc)) static inline const uint8_t **boundS (const struct p_config * const config, uint8_t ** const password, const uint8_t * const len)
{
	uint8_t i;
	const uint8_t **bounds;
	
	bounds = malloc ( (config->max_s+1)*sizeof (uint8_t *));

	if (!bounds) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	else {
		for (i = 0; i < config->max_s; i++) {
			bounds[i] = password[i] + len[i] - 1;
		}

		bounds[i] = (void *) ((uint8_t *) NULL + 1);
	}

	return bounds;
}

__attribute__((always_inline)) static inline void valid_config (const struct p_config * const config)
{
	if (!config) {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, always_inline)) static inline void free_p_config_add_set	(struct p_config * const);
__attribute__((nonnull, always_inline)) static inline void free_p_config_rm_set		(struct p_config * const);
__attribute__((nonnull, always_inline)) static inline void free_p_config_sub_set	(struct p_config * const);

__attribute__((flatten)) void free_p_config (struct p_config * const config)
{
	free_p_config_add_set (config);
	free_p_config_rm_set (config);
	free_p_config_sub_set (config);
	free (config->charsets.base_set);
	free (config);
}

__attribute__((nonnull, always_inline)) static inline void free_p_config_add_set (struct p_config *const config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.add_set[i]);
	}
	free (config->charsets.add_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_rm_set (struct p_config *const config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.rm_set[i]);
	}
	free (config->charsets.rm_set);
}
__attribute__((nonnull, always_inline)) static inline void free_p_config_sub_set (struct p_config *const config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.sub_set[i]);
	}
	free (config->charsets.sub_set);
}

__attribute__((nonnull, always_inline)) static inline void print_base_set	(const struct p_config *const);
__attribute__((nonnull, always_inline)) static inline void print_add_set	(const struct p_config *const);
__attribute__((nonnull, always_inline)) static inline void print_rm_set		(const struct p_config *const);
__attribute__((nonnull, always_inline)) static inline void print_sub_set	(const struct p_config *const);
__attribute__((nonnull, always_inline)) static inline void print_total		(const struct p_config *const);

__attribute__((flatten)) void print_p_config (const struct p_config *config)
{
	if (config != NULL) {
		printf ("Minimum password size: %u\n", config->min_s);
		printf ("Maximum password size: %u\n", config->max_s);

		print_base_set (config);
		print_add_set (config);
		print_rm_set (config);
		print_sub_set (config);
		print_total (config);
	}
}

__attribute__((nonnull, always_inline)) static inline void print_base_set (const struct p_config *const config)
{
	printf ("\nBase set:\n\t%s\n", config->charsets.base_set);
}

__attribute__((nonnull, always_inline)) static inline void print_add_set (const struct p_config *const config)
{
	uint8_t i;

	printf ("\nAdds to the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.add_set[i]);
}

__attribute__((nonnull, always_inline)) static inline void print_rm_set (const struct p_config *const config)
{
	uint8_t i;

	printf ("\nRemoves from the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.rm_set[i]);
}

__attribute__((nonnull, always_inline)) static inline void print_sub_set (const struct p_config *const config)
{
	uint8_t i;

	printf ("\nSubstitutions to the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.sub_set[i]);
}

__attribute__((nonnull, always_inline)) static inline void print_total (const struct p_config *const config)
{
	printf ("\nTOTAL: %lu\n", config->total);
}
