#include "../include/plist.h"
#include "../include/plist_err.h"
#ifndef __GNUC__
#define __inline__ inline
#endif

__attribute__((always_inline, nonnull)) 
static inline void p_config_set_default (struct p_config * const new) 
{
	new->min_s = 0;
	new->max_s = 0;
	new->total = 0;
	new->charsets.base_set = NULL;
	new->charsets.sub_set = NULL;
}

__attribute__((malloc, warn_unused_result)) 
struct p_config *new_p_config (void)
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

__attribute__((always_inline, nonnull)) 
static inline void p_config_extra_set_default (struct p_config_extra * const new)
{
	new->num_threads = 1;
	new->buffer_size = 255;
	new->mode = 0;
	new->out = fileno (stdout);
	new->remaining = NULL;
	new->save = 0;
}

__attribute__((malloc, warn_unused_result)) 
struct p_config_extra *new_p_config_extra (void)
{
	struct p_config_extra *new = malloc (sizeof(struct p_config_extra));

	if (new) {
		p_config_extra_set_default (new);
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	
	return new;
}

__attribute__((flatten)) 
static inline void p_config_set_total (struct p_config * const);

__attribute__((flatten)) 
void p_config_set_size (const uint8_t size, struct p_config * const config) 
{
	if (config) {
		p_config_set_min_s (size, config);
		p_config_set_max_s (size, config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) 
void p_config_set_min_s (const uint8_t min, struct p_config * const config)
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

__attribute__((nonnull, always_inline)) 
static inline void p_config_realloc_sets	(struct p_config * const, const uint8_t);
__attribute__((nonnull, always_inline)) 
static inline void p_config_null_sets		(struct p_config * const, const uint8_t);

__attribute__((flatten)) 
void p_config_set_max_s (const uint8_t max, struct p_config *config)
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

__attribute__((nonnull, always_inline)) 
static inline void p_config_realloc_sets (struct p_config * const config, const uint8_t max)
{
	config->charsets.sub_set = realloc ( config->charsets.sub_set, max*sizeof (int8_t *) );
}

__attribute__((nonnull, always_inline)) 
static inline void p_config_null_sets (struct p_config * const config, const uint8_t max)
{
	while (config->max_s < max) {
		config->charsets.sub_set[config->max_s] = NULL;
		config->max_s++;
	}
}

__attribute__((flatten)) 
void p_config_base_set (char * const base_set, struct p_config * const config)
{
	if (config) {
		uint8_t len = strlen (base_set)+1;
		config->charsets.base_set = realloc (config->charsets.base_set, len*sizeof (int8_t));
		memcpy (config->charsets.base_set, base_set, len);
		p_config_set_total (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) 
void p_config_sub_chars_from (char * const chars_to_sub, const uint8_t ndx, struct p_config * const config)
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

__attribute__((flatten))
void p_config_rm_sub_set (struct p_config * const config, const uint8_t ndx)
{
	if (config && ndx < config->max_s) {
		if (config->charsets.sub_set[ndx]) {
			free (config->charsets.sub_set[ndx]);
			config->charsets.sub_set[ndx] = NULL;
			p_config_set_total (config);
		}
		else {
			plist_err (PLIST_ERR_NULL);
		}
	}
	else if (ndx >= config->max_s){
		plist_err (PLIST_ERR_NDX_OUT);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, malloc, flatten, always_inline)) 
static inline uint8_t *malloc_lengths			(const struct p_config * const);
__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_zero_to_min_s		(struct p_config * const, const uint8_t * const);
__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_min_s_to_max_s	(struct p_config * const, const uint8_t * const);

__attribute__((flatten)) 
static inline void p_config_set_total (struct p_config * const config) /*Tenho que fazer com que essa função possa ser nonnull*/
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
uint64_t p_config_get_total (const struct p_config * const config)
#else
uint32_t p_config_get_total (const struct p_config * const config)
#endif
{ 
	return config->total; 
}

__attribute__((nonnull, always_inline)) 
static inline uint8_t pos_length (const struct p_config * const, const uint8_t);

__attribute__((nonnull, malloc, flatten)) 
static inline uint8_t *malloc_lengths (const struct p_config * const config)
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

__attribute__((always_inline, nonnull)) 
static inline uint8_t noec	(const int8_t * const, const int8_t * const); /*noec = number of equal chars*/
__attribute__((always_inline, nonnull)) 
static inline int8_t *cnt	(const int8_t * const, const int8_t * const); /*concatenates*/

__attribute__((nonnull, always_inline)) 
static inline uint8_t pos_length (const struct p_config * const config, const uint8_t ndx)
{
	uint8_t len = 0;

	if (config->charsets.sub_set[ndx]) {
		len = strlen ((char *) config->charsets.sub_set[ndx]);
	}
	else {
		len = strlen ((char *) config->charsets.base_set);
	}

	return len;
}

__attribute__((always_inline)) 
static inline uint8_t noec (const int8_t * const str_1, const int8_t * const str_2)
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

__attribute__((always_inline)) 
static inline int8_t *cnt (const int8_t * const str_1, const int8_t * const str_2)
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

__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_zero_to_min_s (struct p_config * const config, const uint8_t * const lengths)
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

__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_min_s_to_max_s (struct p_config * const config, const uint8_t * const lengths)
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

struct State {
	uint8_t num_threads;
	struct p_config *config;
};

struct Buffer {
	uint8_t *elements;	/*elements in the buffer*/
	uint8_t length;		/*buffer length*/
	uint8_t total;		/*total of elements in the buffer*/
};

__attribute__((always_inline, nonnull)) 
static inline uint8_t  **p_config_compile 	(const struct p_config * const);
__attribute__((always_inline)) 
static inline void valid_config			(const struct p_config * const);
__attribute__((nonnull, hot)) 
static void check_bounds			(uint8_t **, const uint8_t ** const, const uint8_t * const, uint8_t);
__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t *lengths		(const struct p_config * const);
__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t **boundS		(const struct p_config * const, uint8_t ** const, const uint8_t * const);
#ifdef __LP64__
__attribute__((always_inline, nonnull)) 
static inline uint64_t initial_total 		(const struct p_config * const, const uint8_t * const);
__attribute__((always_inline)) 
static inline uint64_t get_thread_total 	(const uint64_t total);
#else
__attribute__((always_inline, nonnull)) 
static inline uint32_t initial_total 		(const struct p_config * const, const uint8_t * const);
__attribute__((always_inline)) 
static inline uint32_t get_thread_total 	(const uint32_t total);
#endif
__attribute__((always_inline, flatten, nonnull, hot)) 
static inline void password_to_buffer		(uint8_t ** const, const int8_t, struct Buffer *const, const uint8_t);
__attribute__((always_inline, malloc)) 
static inline struct Buffer *get_buffer 	(const uint8_t);
__attribute__((nonnull))
static void positionate_password 		(uint8_t ***const, const uint8_t, const uint8_t *const, uint64_t);
__attribute__((always_inline, hot)) 
static inline void buffer_to_out		(const int8_t, struct Buffer *const);

__attribute__((hot)) 
void generate (struct p_config *config, int8_t fd, uint16_t buff_s, uint16_t num_threads)
{
	uint8_t **password;
	struct Buffer *buffer;
	const uint8_t **bounds;
	const uint8_t *lens;
	#if __LP64__
	uint64_t total;
	#else
	uint32_t total;
	#endif
	uint8_t limit;

	/*Verify if 'config' is a valid password configuration*/
	valid_config (config);

	/*Set the total of threads*/
	if (num_threads == 0)
		num_threads = omp_get_num_procs ();

	/*Get the length of every position set in the password*/
	lens = lengths (config);

	/*Get the initial total of passwords to generate*/
	total = initial_total (config, lens);

	/*Paralized password creation loop*/
	for (limit = config->min_s; limit <= config->max_s; limit++) {

		total *= lens[limit-1];
		#pragma omp parallel num_threads (num_threads) private (password, buffer, bounds)
		{
			#if __LP64__
			uint64_t thread_total;
			#else
			uint32_t thread_total;
			#endif

			/*Get the total of passwords to be generate for each thread*/
			thread_total = get_thread_total (total);

			/*Set up the password*/
			password = p_config_compile (config);

			/*Set up the buffer*/
			buffer = get_buffer (buff_s);

			/*Get the bounds of every position set in the password*/
			bounds = boundS (config, password, lens);
		
			/*Positionate the initial password for each thread*/
			positionate_password (&password, limit, lens, thread_total);
		
			/*Add the reamaining of the total passwords to the last thread*/ /*looks like its smelling here, make it better!*/
			if (omp_get_thread_num () == omp_get_num_threads () - 1) {
				thread_total += total%omp_get_num_threads ();
			}

			while (thread_total-->0) {
				password_to_buffer (password, fd, buffer, limit);
				check_bounds (password, bounds, lens, 0);
				password[0]++;
			}

			/*Cleans the buffer*/
			buffer_to_out (fd, buffer);

			/*Free*/
			free (buffer->elements);
			free (buffer);
			free (bounds);
			
			free (password);
		}
	}

	/*Frees*/
	free ((void *) lens);
}

__attribute__((always_inline, nonnull)) 
static inline uint8_t **p_config_compile (const struct p_config * const config)
{
	uint8_t **compiled_password;
	uint8_t i;

	compiled_password = malloc ((config->max_s+1)*sizeof (uint8_t *));

	if (compiled_password) {

		for (i = 0; i < config->max_s; i++) {
			if (config->charsets.sub_set[i]) {
				compiled_password[i] = (uint8_t *)config->charsets.sub_set[i];
			}
			else {
				compiled_password[i] = (uint8_t *)config->charsets.base_set;
			}
		}

		compiled_password[i] = NULL;
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return compiled_password;
}

__attribute__((always_inline, flatten, nonnull, hot)) 
static inline void password_to_buffer (uint8_t **const password, const int8_t fd, struct Buffer *const buffer, const uint8_t limit)
{
	uint8_t k;
	for (k = 0; k < limit; k++) {
		memcpy (buffer->elements++, password[k], 1);
	}

	/*Ugly. maybe don't do it, or make it optional..*/
	memcpy (buffer->elements, "\n", 1);
	buffer->elements++;

	buffer->total += limit+1;

	if (buffer->total >= buffer->length - limit) {
		buffer_to_out (fd, buffer);
	}
}

__attribute__((always_inline, hot)) 
static inline void buffer_to_out (const int8_t fd, struct Buffer *const buffer)
{
	buffer->elements -= buffer->total;
	#pragma omp critical
	write (fd, buffer->elements, buffer->total);
	buffer->total = 0;
}

__attribute ((always_inline, malloc, alloc_size(1))) 
static inline struct Buffer *get_buffer (const uint8_t buffer_length)
{
	struct Buffer *const buffer = malloc (sizeof (struct Buffer));

	if (!buffer) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	else {
		buffer->length = buffer_length;
		buffer->elements = malloc (buffer->length*sizeof(uint8_t));
		if (!buffer->elements) {
			plist_err (PLIST_ERR_NULL_MALLOC);
		}
		buffer->total = 0;
	}

	return buffer;
}

__attribute__((nonnull))
static void positionate_password (uint8_t ***const password, const uint8_t limit, const uint8_t *const lens, uint64_t thread_total)
{
			uint8_t i;
			printf ("total:%li\n", thread_total);
			thread_total *= omp_get_thread_num ();
			printf ("total:%li\n", thread_total);

			for (i = 0; i < limit; i++) {
				(*password)[i] += thread_total%lens[i];
				thread_total /= lens[i];
			}
}
__attribute__((nonnull, hot)) 
static void check_bounds (uint8_t **compiled_password, const uint8_t ** const bounds, const uint8_t * const lens, uint8_t ndx)
{
	if (compiled_password[ndx] == bounds[ndx]) {
		compiled_password[ndx] -= lens[ndx];
		check_bounds (compiled_password, bounds, lens, ++ndx);
		compiled_password[ndx]++;
	}
}

__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t *lengths (const struct p_config *const config)
{
	uint8_t i, *lengths;
	
	lengths =  malloc (config->max_s*sizeof (uint8_t));

	if (!lengths) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	else {
		for (i = 0; i < config->max_s; i++) {
			if (config->charsets.sub_set[i]) {
				lengths[i] = strlen ((char *)config->charsets.sub_set[i]);
			}
			else {
				lengths[i] = strlen ((char *)config->charsets.base_set);
			}
		}
	}

	return lengths;
}

#ifdef __LP64__
__attribute__((always_inline, nonnull)) 
static inline uint64_t initial_total (const struct p_config * const config, const uint8_t * const len)
#else
__attribute__((always_inline, nonnull)) 
static inline uint32_t initial_total (const struct p_config * const config, const uint8_t * const len)
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

__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t **boundS (const struct p_config * const config, uint8_t ** const password, const uint8_t * const len)
{
	uint8_t i;
	const uint8_t **bounds;
	
	bounds = malloc ( (config->max_s+1)*sizeof (uint8_t *) );

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

__attribute__((always_inline)) 
static inline void valid_config (const struct p_config * const config)
{
	if (!config) {
		plist_err (PLIST_ERR_NULL);
	}
}

#ifdef __LP64__
__attribute__((always_inline)) 
static inline uint64_t get_thread_total (const uint64_t total)
#else 
__attribute__((always_inline)) 
static inline uint32_t get_thread_total (const uint32_t total)
#endif
{
	#ifdef __LP64__
	uint64_t thread_total = total/omp_get_num_threads ();
	#else
	uint32_t thread_total = total/omp_get_num_threads ();
	#endif

	return thread_total;
}


__attribute__((nonnull, always_inline)) 
static inline void free_p_config_sub_set	(struct p_config * const);

__attribute__((flatten)) 
void free_p_config (struct p_config * const config)
{
	free_p_config_sub_set (config);
	free (config->charsets.base_set);
	free (config);
}

__attribute__((nonnull, always_inline)) 
static inline void free_p_config_sub_set (struct p_config *const config)
{
	uint8_t i;
	for (i = 0; i < config->max_s; i++) {
		free (config->charsets.sub_set[i]);
	}
	free (config->charsets.sub_set);
}

__attribute__((nonnull, always_inline)) 
static inline void print_base_set	(const struct p_config *const);
__attribute__((nonnull, always_inline)) 
static inline void print_sub_set	(const struct p_config *const);
__attribute__((nonnull, always_inline)) 
static inline void print_total		(const struct p_config *const);

__attribute__((flatten)) void print_p_config (const struct p_config *config)
{
	if (config != NULL) {
		printf ("Minimum password size: %u\n", config->min_s);
		printf ("Maximum password size: %u\n", config->max_s);

		print_base_set (config);
		print_sub_set (config);
		print_total (config);
	}
}

__attribute__((nonnull, always_inline)) 
static inline void print_base_set (const struct p_config *const config)
{
	printf ("\nBase set:\n\t%s\n", config->charsets.base_set);
}

__attribute__((nonnull, always_inline)) 
static inline void print_sub_set (const struct p_config *const config)
{
	uint8_t i;

	printf ("\nSubstitutions to the base set\n");

	for (i = 0; i < config->max_s; i++)
		printf ("\tchar[%u]:%s\n", i, config->charsets.sub_set[i]);
}

__attribute__((nonnull, always_inline)) 
static inline void print_total (const struct p_config *const config)
{
	printf ("\nTOTAL: %lu\n", config->total);
}

/*fazer um if para não salvar o estado caso não for especificado arquivo de saida*/
void recover (struct p_config * const config, struct p_config_extra * const extra, const char * const recover_file_name)
{
	char *line;
	size_t line_size;
	uint8_t aux;
	uint8_t ndx;

	/* Open the recovery file */
	FILE *fs = fopen (recover_file_name, "r+");

	/* Get the output file name */
	getline (&line, &line_size, fs);

	/* Open the output file in append mode */
	extra->out = open (line, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);

	/* Get the minimum password size */
	getline (&line, &line_size, fs);
	config->min_s = atoi (line);

	/* Get the maximum password size */
	getline (&line, &line_size, fs);
	config->max_s = atoi (line);

	/* Get the default password set */
	getline (&line, &line_size, fs);
	if (!strcmp (line, "NULL")) {
		config->charsets.base_set = NULL;
	}
	else {
		config->charsets.base_set = (int8_t *)line;
	}
	
	/* Get the number of modifications to the base set */
	getline (&line, &line_size, fs);
	aux = atoi (line);	
	
	while (aux-->0) {
		getline (&line, &line_size, fs);
		ndx = atoi(strtok (line, " "));
		config->charsets.sub_set[ndx] = (int8_t *)strtok (NULL, " ");
	}

	/* Get buffer size */
	getline (&line, &line_size, fs);
	extra->buffer_size = atoi (line);

	/* Get the number of threads */
	getline (&line, &line_size, fs);
	extra->num_threads = atoi (line);

	/* Malloc the remaining vector */
	if (extra->num_threads == 0) {
		#ifdef __LP64__
		extra->remaining = malloc (sizeof(uint64_t));
		#else
		extra->remaining = malloc (sizeof(uint32_t));
		#endif
	}
	else {
		#ifdef __LP64__
		extra->remaining = malloc (extra->num_threads*sizeof(uint64_t));
		#else
		extra->remaining = malloc (extra->num_threads*sizeof(uint32_t));
		#endif
	}

	/* Get each thread remaining total */
	aux = extra->num_threads;
	while (aux-->0) {
		getline (&line, &line_size, fs);
		ndx = atoi (strtok (line, ""));
		extra->remaining[ndx] = atoi (strtok (NULL, " "));
	}

	/* Get the limit variable */
	getline (&line, &line_size, fs);

	/*nome do arquivo de saida*/
	/*min*/
	/*max*/
	/*default set*/
	/*numero de modificacoes ao conjunto default*/
	/*indice da modificacao : modificacao*/
	/*tamanho do buffer*/
	/*numero de threads*/
	/*tamanho atual*/
	/*thread id : quantidade de senhas calculadas para esse tamanho*/

	/*Colocar o arquivo de recuração utilizado como arquivo de recuperação para o resto do processamento*/	

	free (line);
}

