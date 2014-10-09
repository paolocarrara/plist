#include "../include/plist.h"
#include "../include/plist_err.h"
#ifndef __GNUC__
#define __inline__ inline
#endif

/* Default Values */
#define PLIST_DEFAULT_INTERVAL	0x05 /* default save interval of one second */

__attribute__((malloc, warn_unused_result)) 
struct p_config *new_p_config (void)
{
	struct p_config *new = calloc (1, sizeof (struct p_config)); /*all zero and null*/

	if (!new) {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}

	return new;
}

static inline void p_config_extra_set_default (struct p_config_extra *const);

__attribute__((malloc, warn_unused_result)) 
struct p_config_extra *new_p_config_extra (void)
{
	struct p_config_extra *new = calloc (1, sizeof(struct p_config_extra));

	if (new) {
		p_config_extra_set_default (new);
	}
	else {
		plist_err (PLIST_ERR_NULL_MALLOC);
	}
	
	return new;
}

__attribute__((always_inline, nonnull)) 
static inline void p_config_extra_set_default (struct p_config_extra *const new)
{
	p_config_set_num_threads (P_CONFIG_DEFAULT_NUM_THREADS, new);
	p_config_set_buffer_size (255, new);
	p_config_open_out_file (NULL, new);
	p_config_set_interval (PLIST_DEFAULT_INTERVAL, new);
	p_config_set_save (NULL, new);
	new->restore.remaining = NULL;
	new->load = P_CONFIG_NO_LOAD_RECOVER;
}

void p_config_set_num_threads (const uint8_t num_threads, struct p_config_extra *const extra)
{
	if (extra) {
		if (num_threads > 0) {
			extra->num_threads = num_threads;
		}
		else {
			extra->num_threads = omp_get_num_procs ();
		}
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

void p_config_set_save (const char *const save_name, struct p_config_extra *const extra)
{
	if (extra) {
		if (save_name) {
			extra->save = P_CONFIG_SAVE;
			extra->save_fd = open (save_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		}
		else {
			extra->save = P_CONFIG_NO_SAVE;
		}
		extra->restore.save_name = save_name;
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

void p_config_set_interval (const uint8_t interval, struct p_config_extra *const extra)
{
	if (extra) {
		if (interval > 0) {
			extra->interval = interval;
		}
		else {
			extra->interval = PLIST_DEFAULT_INTERVAL;
		}
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

void p_config_open_out_file (const char *const out_name, struct p_config_extra *const extra)
{
	if (extra) {
		if (out_name != NULL) {
			extra->out_fd = open (out_name, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
		}
		else {
			extra->out_fd = fileno (stdout);
		}
		extra->restore.out_name = out_name;
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

void p_config_set_buffer_size (const uint16_t buffer_size, struct p_config_extra *const extra)
{
	if (extra) {
		if (buffer_size > 0) {
			extra->buffer_size = buffer_size;
		}
		else {
			extra->buffer_size = /*caculate_buffer_size()*/ buffer_size;
		}
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

uint16_t p_config_get_buffer_size (const struct p_config_extra *const extra)
{
	if (extra) {
		return extra->buffer_size;
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
	return 0;
}

__attribute__((flatten)) 
static inline void p_config_set_total (struct p_config *const);

__attribute__((flatten)) 
void p_config_set_size (const uint8_t size, struct p_config *const config) 
{
	if (config) {
		p_config_set_min_s (size, config);
		p_config_set_max_s (size, config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

/*OK*/
__attribute__((flatten)) 
void p_config_set_min_s (const uint8_t min, struct p_config *const config)
{
	if (config) {
		if (min > config->max_s) {
			p_config_set_max_s (min, config);
		}
		config->min_s = min;
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
void p_config_set_max_s (const uint8_t max, struct p_config *const config)
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
static inline void p_config_realloc_sets (struct p_config *const config, const uint8_t max)
{
	config->charsets.sub_set = realloc ( config->charsets.sub_set, max*sizeof (int8_t *) );
}

__attribute__((nonnull, always_inline)) 
static inline void p_config_null_sets (struct p_config *const config, const uint8_t max)
{
	while (config->max_s < max) {
		config->charsets.sub_set[config->max_s] = NULL;
		config->max_s++;
	}
}

__attribute__((flatten)) 
void p_config_base_set (char *const base_set, struct p_config *const config)
{
	if (config) {
		if (base_set) {
			config->charsets.base_set = (int8_t *)base_set;
			p_config_set_total (config);
		}
		else {
			config->charsets.base_set = NULL;
		}
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((flatten)) 
void p_config_sub_chars_from (char *const sub_set, const uint8_t ndx, struct p_config *const config)
{
	if (config && ndx < config->max_s) {
		if (sub_set) {
			config->charsets.sub_set[ndx] = (int8_t *)sub_set;
			p_config_set_total (config);
		}
		else {
			p_config_rm_sub_set (ndx, config);
		}
	}
	else if (ndx >= config->max_s){
		plist_err (PLIST_ERR_NDX_OUT);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

/*OK*/
__attribute__((flatten))
void p_config_rm_sub_set (const uint8_t ndx, struct p_config *const config)
{
	if (config && ndx < config->max_s) {
		if (config->charsets.sub_set[ndx]) {
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
static inline uint8_t *malloc_lengths			(const struct p_config *const);
__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_zero_to_min_s		(struct p_config *const, const uint8_t *const);
__attribute__((nonnull, always_inline)) 
static inline void sub_total_from_min_s_to_max_s	(struct p_config *const, const uint8_t *const);

__attribute__((flatten)) 
static inline void p_config_set_total (struct p_config *const config)
{
	uint8_t *lengths;

	if (config->charsets.base_set) {

		lengths = malloc_lengths (config);

		sub_total_from_zero_to_min_s (config, lengths);

		sub_total_from_min_s_to_max_s (config, lengths);

		free (lengths);
	}
}

/*OK*/
#if __LP64__
uint64_t p_config_get_total (const struct p_config *const config)
#else
uint32_t p_config_get_total (const struct p_config *const config)
#endif
{
	if (config) {
		return config->total; 
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
	return 0;
}

__attribute__((nonnull, always_inline)) 
static inline uint8_t pos_length (const struct p_config *const, const uint8_t);

__attribute__((nonnull, malloc, flatten)) 
static inline uint8_t *malloc_lengths (const struct p_config *const config)
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
static inline uint8_t noec	(const int8_t *const, const int8_t *const); /*noec = number of equal chars*/
__attribute__((always_inline, nonnull)) 
static inline int8_t *cnt	(const int8_t *const, const int8_t *const); /*concatenates*/

__attribute__((nonnull, always_inline)) 
static inline uint8_t pos_length (const struct p_config *const config, const uint8_t ndx)
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
static inline uint8_t noec (const int8_t *const str_1, const int8_t *const str_2)
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
static inline int8_t *cnt (const int8_t *const str_1, const int8_t *const str_2)
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
static inline void sub_total_from_zero_to_min_s (struct p_config *const config, const uint8_t *const lengths)
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
static inline void sub_total_from_min_s_to_max_s (struct p_config *const config, const uint8_t *const lengths)
{
	uint8_t i;
	#if __LP64__
	uint64_t pre_state;
	#else
	uint32_t pre_state;
	#endif 
	
	pre_state = (config->total == 0) ? 1 : config->total;

	for (i = config->min_s; i < config->max_s; i++) {
		config->total += pre_state*lengths[i];
		pre_state *= lengths[i];
	}
}

/*!
 * Buffer structure
 * Holds the passwords before they go the their proper output.
*/

struct Buffer {
	uint8_t *elements;	/* Elements in the buffer */
	uint8_t length;		/* Buffer length */
	uint8_t total;		/* Total of elements in the buffer */
};

__attribute__((always_inline, nonnull)) 
static inline uint8_t  **p_config_compile 	(const struct p_config *const);

__attribute__((nonnull, hot)) 
static void check_bounds			(uint8_t **, const uint8_t **const, const uint8_t *const, uint8_t);

__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t *lengths		(const struct p_config * const);

__attribute__((always_inline, nonnull, malloc)) 
static inline const uint8_t **boundS		(const struct p_config *const, uint8_t **const, const uint8_t *const);

__attribute__((always_inline, flatten, nonnull, hot)) 
static inline void password_to_buffer		(uint8_t **const, const int8_t, struct Buffer *const, const uint8_t);

__attribute__((always_inline, malloc)) 
static inline struct Buffer *get_buffer 	(const uint8_t);

__attribute__((always_inline, hot)) 
static inline void buffer_to_out		(const int8_t, struct Buffer *const);

__attribute__((flatten, nonnull))
static void save_to_restore			(const struct p_config *const, struct p_config_extra *const, const uint8_t);

__attribute__((always_inline, nonnull))
static void save_number_of_threads		(const struct p_config_extra *const, uint8_t);

__attribute__((nonnull, always_inline))
static void restore_limit 			(const struct p_config_extra *const, uint8_t *const);

__attribute__((nonnull, always_inline))
static void restore_threads			(struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static void restore_remaining 			(struct p_config_extra *const);

static void watch_limit_variable 		(uint8_t *const);

static void valid_config			(const struct p_config *const, const struct p_config_extra *const);

#ifdef __LP64__
__attribute__((nonnull))
static void positionate_password 		(uint8_t ***const, const uint8_t, const uint8_t *const, uint64_t, uint64_t, struct p_config_extra *const);

__attribute__((always_inline)) 
static inline uint64_t get_thread_total 	(const uint64_t total, struct p_config_extra *const);

__attribute__((nonnull))
static inline uint64_t round_total		(const uint8_t limit, const uint8_t *const lens);

__attribute__((nonnull))
static void watch_total_thread_variable		(uint64_t *const, const uint8_t);
#else
__attribute__((nonnull))
static void positionate_password 		(uint8_t ***const, const uint8_t, const uint8_t *const, uint32_t, uint32_t, struct p_config_extra *const);

__attribute__((always_inline)) 
static inline uint32_t get_thread_total 	(const uint32_t total, struct p_config_extra *const);

__attribute__((nonnull))
static inline uint32_t round_total		(const uint8_t limit, const uint8_t *const lens);

__attribute__((nonnull))
static void watch_total_thread_variable		(uint32_t *const, const uint8_t);
#endif

static void start_save				(uint8_t);

static void stop_save				(void);


/*ok mas precisa rever, não usar thread pro monitoramento e sim outro processo*/
__attribute__((hot)) 
void generate (struct p_config *config, struct p_config_extra *extra)
{
	#if __LP64__
	uint64_t total;
	#else
	uint32_t total;
	#endif
	uint8_t **password;
	uint8_t limit;
	const uint8_t **bounds;
	const uint8_t *lens;
	struct Buffer *buffer;
	int8_t fd = extra->out_fd;

	/* Verify if 'config' is a valid password configuration */
	valid_config (config, extra);

	/* Get the length of every position set in the password */
	lens = lengths (config);
	
	if (extra->save) {

		/* Initiate the saving process */
		save_to_restore (config, extra, P_CONFIG_OPEN_SAVE);

		/* Enable nesting threads */
		omp_set_nested (0x01);
	}
	
	/*
	 * One thread will generate the passwords and the other will keep saving the generation state.
	*/
	#pragma omp parallel num_threads (0x02) if (extra->save)
	{
		if (!omp_get_thread_num ()) {

			/* Restore the number of threads */
			if (extra->load) {
				restore_threads (extra);
			}

			/* Set the number of threads for the passwords generations */
			omp_set_num_threads (extra->num_threads);
	
			if (extra->save) {

				/* Save the number of threads acquired */
				save_number_of_threads (extra, omp_get_max_threads ());
				
				/* Share limit varialbe address */
				watch_limit_variable (&limit);
			}


			/* Password loop creation */
			for (limit = config->min_s; limit <= config->max_s; limit++) {

				if (extra->load) {

					/* Restore the limit variable */
					restore_limit (extra, &limit);
		
					/* Restore the remaining for each thread */
					restore_remaining (extra);
				}

				/* Get the total that will be divided between the threads */
				total = round_total (limit, lens);

				#pragma omp parallel private (password, buffer, bounds)
				{
					#if __LP64__
					uint64_t *thread_total = malloc (sizeof(uint64_t));
					#else
					uint32_t *thread_total = malloc (sizeof(uint64_t));
					#endif

					/* Share the thread_total variable address */
					if (extra->save) {
						#pragma omp critical
						watch_total_thread_variable (thread_total, omp_get_thread_num ());
					}

					/* Get the total of passwords to be generate for each thread */
					*thread_total = get_thread_total (total, extra);

					/* Set up the password */
					password = p_config_compile (config);

					/* Set up the buffer */
					buffer = get_buffer (extra->buffer_size);

					/* Get the bounds of every position set in the password */
					bounds = boundS (config, password, lens);
		
					/* Positionate the initial password for each thread */
					positionate_password (&password, limit, lens, *thread_total, total, extra);

					/* The actual password generation */
					while ((*thread_total)-->0) {

						password_to_buffer (password, fd, buffer, limit);

						check_bounds (password, bounds, lens, 0);

						password[0]++;
					}
	
					/* Cleans the buffer */
					buffer_to_out (fd, buffer);

					/* Free */
					free (buffer->elements);
					free (buffer);
					free (bounds);
					free (password);
				}
			}

			/* Free */
			free ((void *) lens);
		}
		else {
			/* Start saving */
			start_save (extra->interval);
		}
		
		#pragma omp master
		{
			if (extra->save) {
				/* End the saving to restore process */
				stop_save ();

				/* Close the file where the savings were done */
				save_to_restore (config, extra, P_CONFIG_CLOSE_SAVE);
			}
		}
	}
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

	/*
	memcpy (buffer->elements, "\n", 1);
	buffer->elements++;
	*/
	
	buffer->total += limit/*+1*/;

	if (buffer->total >= buffer->length - limit) {
		buffer_to_out (fd, buffer);
	}
}

__attribute__((always_inline, hot)) 
static inline void buffer_to_out (const int8_t fd, struct Buffer *const buffer)
{
	buffer->elements -= buffer->total;
	/*#pragma omp critical*/ /*a função write parece ser thread safe*/
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

#ifdef __LP64__
__attribute__((nonnull))
static void positionate_password (uint8_t ***const password, const uint8_t limit, const uint8_t *const lens, uint64_t thread_total, uint64_t total, struct p_config_extra *const extra)
#else
__attribute__((nonnull))
static void positionate_password (uint8_t ***const password, const uint8_t limit, const uint8_t *const lens, uint32_t thread_total, uint32_t total, struct p_config_extra *const extra)
#endif
{
			/* Melhorar essa função */
			uint8_t i, thread_num;
			static uint8_t thread_counter = 0;
			#ifdef __LP64__
			uint64_t skip;
			#else
			uint32_t skip = 0;
			#endif

			if (extra->load) {

				skip = (total/omp_get_num_threads ())*omp_get_thread_num ();
				skip += (total/omp_get_num_threads ())-thread_total;

				for (i = 0; i < limit; i++) {
					(*password)[i] += skip%lens[i];
					skip /= lens[i];
				}

				#pragma omp critical
				thread_counter++;

				if (thread_counter == omp_get_num_threads()) {
					extra->load = 0;
				}
			}
			else {
				thread_num = omp_get_thread_num ();

				if (thread_num == omp_get_num_threads () - 1) {
					thread_total -= total%omp_get_num_threads ();
				}

				thread_total *= thread_num;
	
				for (i = 0; i < limit; i++) {
					(*password)[i] += thread_total%lens[i];
					thread_total /= lens[i];
				}
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
__attribute__((nonnull))
static inline uint64_t round_total (const uint8_t limit, const uint8_t *const lens)
#else
__attribute__((nonnull))
static inline uint32_t round_total (const uint8_t limit, const uint8_t *const lens)
#endif
{
	#ifdef __LP64__
	uint64_t round_total = 1;
	#else
	uint32_t round_total = 1;
	#endif
	uint8_t i;

	for (i = 1; i <= limit; i++) {
		round_total *= lens[i-1];
	}

	return round_total;
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

#ifdef __LP64__
__attribute__((always_inline)) 
static inline uint64_t get_thread_total (const uint64_t total, struct p_config_extra *const extra)
#else 
__attribute__((always_inline)) 
static inline uint32_t get_thread_total (const uint32_t total, struct p_config_extra *const extra)
#endif
{
	#ifdef __LP64__
	uint64_t thread_total = total/omp_get_num_threads ();
	#else
	uint32_t thread_total = total/omp_get_num_threads ();
	#endif
	uint8_t thread_num = omp_get_thread_num ();

	if (extra->restore.remaining) {
		if (extra->restore.remaining[omp_get_thread_num ()]) {
			thread_total = extra->restore.remaining[thread_num];
			extra->restore.remaining[thread_num] = 0;
		}
	}
	else {
		thread_total = total/omp_get_num_threads ();
		if (thread_num == omp_get_num_threads () - 1) {
			thread_total += total%omp_get_num_threads ();
		}
	}

	return thread_total;
}

__attribute__((nonnull, always_inline))
static inline void valid_sets (const struct p_config *const);

static void valid_config (const struct p_config *const config, const struct p_config_extra *const extra)
{
	if (config && extra) {
		valid_sets (config);
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, always_inline))
static inline void valid_sets (const struct p_config *const config)
{
	uint8_t ndx;

	if (!config->charsets.base_set) {
		for (ndx = 0; ndx < config->max_s; ndx++) {
			if (config->charsets.sub_set[ndx] == NULL) {
				printf ("ERROR: NO SUB SET TO SUBSTITUTE EMPTY BASE SET AT POSITION %i\n", ndx);
				exit (-1);
			}
		}
	}
}

__attribute__((nonnull, always_inline)) 
static inline void free_p_config_sub_set (struct p_config *const);

__attribute__((flatten)) 
void free_p_config (struct p_config *const config)
{
	if (config) {
		free_p_config_sub_set (config);
		free (config);
	}
}
void free_p_config_extra (struct p_config_extra *const extra)
{
	if (extra) {
		free (extra);
	}
}

__attribute__((nonnull, always_inline)) 
static inline void free_p_config_sub_set (struct p_config *const config)
{
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

__attribute__((nonnull, always_inline))
static inline void restore_out_name	(struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_min_size	(struct p_config *const, const struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_max_size	(struct p_config *const, const struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_base_set	(struct p_config *const, const struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_sub_set	(struct p_config *const, const struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_buffer_size	(struct p_config_extra *const);

__attribute__((nonnull, always_inline))
static inline void restore_interval 	(struct p_config_extra *const);

void p_config_restore (struct p_config *const config, struct p_config_extra *const extra, const char *const restore_file_name)
{
	if (config && extra && restore_file_name) {

		extra->save_fd = open (restore_file_name, O_RDONLY);

		restore_out_name	(extra);
		restore_min_size	(config, extra);
		restore_max_size 	(config, extra);
		restore_base_set 	(config, extra);
		restore_sub_set 	(config, extra);
		restore_buffer_size 	(extra);
		restore_interval 	(extra);

		/*p_config_set_save ();*/
		extra->load = 1;
	}
	else {
		plist_err (PLIST_ERR_NULL);
	}
}

__attribute__((nonnull, always_inline))
static inline void restore_out_name (struct p_config_extra *const extra)
{
	uint16_t out_file_name_size;
	char *out_name;

	read (extra->save_fd, &out_file_name_size, 2);

	out_name = malloc ((out_file_name_size+1)*sizeof(char));

	out_name[out_file_name_size] = '\0';

	read (extra->save_fd, out_name, out_file_name_size);

	p_config_open_out_file (out_name, extra);
}

__attribute__((nonnull, always_inline))
static inline void restore_min_size (struct p_config *const config, const struct p_config_extra *const extra)
{
	uint8_t min_size;

	read (extra->save_fd, &min_size, 1);
	p_config_set_min_s (min_size, config);
}

__attribute__((nonnull, always_inline))
static inline void restore_max_size (struct p_config *const config, const struct p_config_extra *const extra)
{
	uint8_t max_size;

	read (extra->save_fd, &max_size, 1);
	p_config_set_max_s (max_size, config);
}

__attribute__((nonnull, always_inline))
static inline void restore_base_set (struct p_config *const config, const struct p_config_extra *const extra)
{
	uint16_t base_set_length;
	char *base_set;
	
	read (extra->save_fd, &base_set_length, 2);

	base_set = malloc ((base_set_length+1)*sizeof(char));

	base_set[base_set_length] = '\0';

	read (extra->save_fd, base_set, base_set_length);
	
	p_config_base_set (base_set, config);
}

__attribute__((nonnull, always_inline))
static inline void restore_sub_set (struct p_config *const config, const struct p_config_extra *const extra)
{
	uint8_t nmttbs; /*number of modifications to the base set*/
	uint8_t ndx;
	char *sub_set;
	uint16_t sub_set_length;
	
	
	read (extra->save_fd, &nmttbs, 1);

	while (nmttbs-->0) {
		read (extra->save_fd, &sub_set_length, 2);
		read (extra->save_fd, &ndx, 2);
		sub_set = malloc (sub_set_length*sizeof(char));
		read (extra->save_fd, sub_set, sub_set_length);
		p_config_sub_chars_from (sub_set, ndx, config);
	}
}

__attribute__((nonnull, always_inline))
static inline void restore_buffer_size (struct p_config_extra *const extra)
{
	uint8_t buffer_size;

	read (extra->save_fd, &buffer_size, 1);
	p_config_set_buffer_size (buffer_size, extra);
}

__attribute__((nonnull, always_inline))
static inline void restore_interval (struct p_config_extra *const extra)
{
	uint8_t interval;

	read (extra->save_fd, &interval, 1);
	p_config_set_interval (interval, extra);
}

__attribute__((nonnull, always_inline))
static inline void restore_limit (const struct p_config_extra *const extra, uint8_t *const limit)
{	
	read (extra->save_fd, limit, 1);
}

__attribute__((nonnull, always_inline))
static inline void restore_threads (struct p_config_extra *const extra)
{
	uint8_t num_threads;

	read (extra->save_fd, &num_threads, 1);
	p_config_set_num_threads (num_threads, extra);
}

__attribute__((nonnull, always_inline))
static inline void restore_remaining (struct p_config_extra *const extra)
{
	uint8_t i;

	#ifdef __LP64__
	extra->restore.remaining = malloc (extra->num_threads*sizeof(uint64_t));
	#else
	extra->restore.remaining = malloc (extra->num_threads*sizeof(uint32_t));
	#endif
	

	for (i = 0; i < extra->num_threads; i++) {
		#ifdef __LP64__
		read (extra->save_fd, &extra->restore.remaining[i], 8);
		#else
		read (extra->save_fd, &extra->restore.remaining[i], 4);
		#endif
	}
}

static void share_save_fd_variable	(const uint8_t);

__attribute__((always_inline, nonnull))
static inline void save_min_s		(const struct p_config *const, const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_max_s		(const struct p_config *const, const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_base_set	(const struct p_config *const, const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_nomttbs		(const struct p_config *const, const struct p_config_extra *const); /*nomttbs = number of modifications to the base set*/

__attribute__((always_inline, nonnull))
static inline void save_sub_set		(const struct p_config *const, const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_buffer_size	(const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_out_name	(const struct p_config_extra *const);

__attribute__((always_inline, nonnull))
static inline void save_interval	(const struct p_config_extra *const);

__attribute__((flatten, nonnull))
static void save_to_restore (const struct p_config *const config, struct p_config_extra *const extra, uint8_t type)
{
	if (type == P_CONFIG_OPEN_SAVE) {
		save_out_name		(extra);
		save_min_s		(config, extra);
		save_max_s		(config, extra);
		save_base_set		(config, extra);
		save_nomttbs		(config, extra);
		save_sub_set 		(config, extra);
		save_buffer_size	(extra);
		save_interval		(extra);

		share_save_fd_variable (extra->save_fd);
	}
	else if (type == P_CONFIG_CLOSE_SAVE) {
		close (extra->save_fd);
	}
}

__attribute__((always_inline, nonnull))
static inline void save_out_name (const struct p_config_extra *const extra)
{
	uint16_t output_file_name_length;

	if (extra->restore.out_name) {
		output_file_name_length = strlen ((char *)extra->restore.out_name);

		/* Writes the output file name length in bytes */
		write (extra->save_fd, &output_file_name_length, 2);

		/* Writes the output file name */
		write (extra->save_fd, extra->restore.out_name, output_file_name_length);
	}
	else {
		output_file_name_length = strlen ("NULL");

		/* Writes the output file name length in bytes */
		write (extra->save_fd, &output_file_name_length, 2);

		/* Writes the output file name */
		write (extra->save_fd, "NULL", output_file_name_length);
	}
}

__attribute__((always_inline, nonnull))
static inline void save_min_s (const struct p_config *const config, const struct p_config_extra *const extra)
{
	write (extra->save_fd, &config->min_s, 1);
}

__attribute__((always_inline, nonnull))
static inline void save_max_s (const struct p_config *const config, const struct p_config_extra *const extra)
{
	write (extra->save_fd, &config->max_s, 1);
}

__attribute__((always_inline, nonnull))
static inline void save_base_set (const struct p_config *const config, const struct p_config_extra *const extra)
{
	uint16_t base_set_length = strlen ((char *)config->charsets.base_set);

	/* Writes the length of the base set in bytes */
	write (extra->save_fd, &base_set_length, 2);

	/* Writes the base set */
	write (extra->save_fd, config->charsets.base_set, base_set_length);
}

__attribute__((always_inline, nonnull))
static inline void save_nomttbs (const struct p_config *const config, const struct p_config_extra *const extra)
{
	uint8_t n, i;

	for (n = 0, i = 0; i < config->max_s; i++) {
		if (config->charsets.sub_set[i]) {
			n++;
		}
	}

	/* Writes the number of modifications to the base set */
	write (extra->save_fd, &n, 1);
}

__attribute__((always_inline, nonnull))
static inline void save_sub_set (const struct p_config *const config, const struct p_config_extra *const extra)
{
	uint16_t sub_set_length;
	uint8_t ndx;

	for (ndx = 0; ndx < config->max_s; ndx++) {
		if (config->charsets.sub_set[ndx]) {
			sub_set_length = strlen ((char *)config->charsets.sub_set[ndx]);

			/* Writes the modification index */
			write (extra->save_fd, &ndx, 1);
			
			/* Writes the subset lenth in bytes */
			write (extra->save_fd, &sub_set_length, 2);
			
			/* Writes the subset */
			write (extra->save_fd, config->charsets.sub_set[ndx], sub_set_length);
		}
	}
}

__attribute__((always_inline, nonnull))
static inline void save_buffer_size (const struct p_config_extra *const extra)
{
	write (extra->save_fd, &extra->buffer_size, 1);
}

__attribute__((always_inline, nonnull))
static inline void save_interval (const struct p_config_extra *const extra)
{
	write (extra->save_fd, &extra->interval, 1);
}

__attribute__((always_inline, nonnull))
static inline void save_number_of_threads (const struct p_config_extra *const extra, uint8_t num_threads)
{
	write (extra->save_fd, &num_threads, 1);
}

/* Backup Types */
#define LIMIT_ADDRESS		0x01
#define THREAD_ADDRESS		0x02
#define SAVE_FD			0x03
#define START_CHECK_POINT	0x04

#ifdef __LP64__
static void backup (uint64_t *address, uint8_t thread_num_or_seconds_or_save_fp, uint8_t pre_check_type);
#else
static void backup (uint32_t *address, uint8_t thread_num_or_seconds_or_save_fp, uint8_t pre_check_type);
#endif

static void share_save_fd_variable (const uint8_t save_fd)
{
	backup (NULL, save_fd, SAVE_FD);
}

static void watch_limit_variable (uint8_t *const limit)
{
	#ifdef __LP64__
	backup ((uint64_t *)limit, 0, LIMIT_ADDRESS);
	#else 
	backup ((uint32_t *)limit, 0, LIMIT_ADDRESS);
	#endif
}

#ifdef __LP64__
__attribute__((nonnull))
static void watch_total_thread_variable (uint64_t *const total_thread, const uint8_t thread_num)
#else
__attribute__((nonnull))
static void watch_total_thread_variable (uint32_t *const total_thread, const uint8_t thread_num)
#endif
{
	backup (total_thread, thread_num, THREAD_ADDRESS);
}

static void start_save (const uint8_t seconds)
{
	backup (NULL, seconds, START_CHECK_POINT);
}

static void stop_save (void)
{
	backup (NULL, 0, LIMIT_ADDRESS);
}

/*!
 * Unify all of the variables that are needed to the saving process. Also, initiates the saving process.
 *
 * @param address of the variable to monitor its value.
 * @param time interval to save the state | file descriptor of the save file | number of threads thats being used
 * @param type of backup
*/
#ifdef __LP64__
static void backup (uint64_t *address, uint8_t aux, uint8_t backup_type)
#else
static void backup (uint32_t *address, uint8_t aux, uint8_t backup_type)
#endif
{
	#ifdef __LP64__
	static uint64_t **thread_total = NULL;
	#else
	static uint32_t **thread_total = NULL;
	#endif
	static uint16_t limit_offset;
	static uint8_t save_fd;
	static uint8_t num_threads = 0;
	static uint8_t *limit = NULL;
	uint8_t i;

	if (backup_type == START_CHECK_POINT) {
		uint8_t interval = aux;

		while (num_threads == 0) {
			/* Makes every thread address to be ready for saving */
		}
		for (i = 0; i < num_threads; i++) {
			if (thread_total[i] == NULL) {
				i = -1;
			}
		}

		printf ("Saving every %i seconds!\n", interval);

		while (limit) {

			printf ("salving..\n");

			/* Positionate the file pointer */
			lseek (save_fd, limit_offset, SEEK_SET);

			/* Writes the limit value */
			write (save_fd, limit, 1);

			/* Writes the total remaining of each thread*/
			for (i = 0; i < num_threads; i++) {
				#ifdef __LP64__
				write (save_fd, thread_total[i], 8);
				#else
				write (save_fd, thread_total[i], 4);
				#endif
			}

			/* Waits */
			sleep (interval);
		}
	}
	else if (backup_type == LIMIT_ADDRESS) {
		/* Get the limit address variable */
		limit = (uint8_t *)address;

		/* Get the limit value position on file */
		limit_offset = lseek (save_fd, 0, SEEK_CUR);

		/* Reserve the postion */
		lseek (save_fd, limit_offset+1, SEEK_SET);
	}
	else if (backup_type == SAVE_FD) {
		/* Saves the file descriptor */
		save_fd = aux;
	}
	else if (backup_type == THREAD_ADDRESS) {
		uint8_t thread_num = aux;

		if (num_threads == 0) {
			#ifdef __LP64__
			thread_total = calloc (omp_get_max_threads (), sizeof (uint64_t *));
			#else
			thread_total = calloc (omp_get_max_threads (), sizeof (uint32_t *));
			#endif
			num_threads = omp_get_max_threads ();
		}
		thread_total[thread_num] = address;
	}
}
