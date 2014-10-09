#ifndef PLIST_H
#define PLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <omp.h>

#define P_CONFIG_SAVE			0x01
#define P_CONFIG_NO_SAVE		0x00
#define P_CONFIG_LOAD_RECOVER 		0x01
#define P_CONFIG_NO_LOAD_RECOVER 	0x00
#define P_CONFIG_DEFAULT_NUM_THREADS	0x01
#define P_CONFIG_OPEN_SAVE		0x01
#define P_CONFIG_CLOSE_SAVE		0x02

/*!
 * Struct sets, contained in the p_config struct
 *
 * It holds the set of characters to the password generation
 */
struct sets {
	int8_t *base_set;	/*base set for the passwords generation*/
	int8_t **sub_set;	/*substitution sets to the base set*/
};

/*!
 * Struct p_config
 *
 * It holds basic information to the password generation regardless to the password specification.
 */
struct p_config {
	uint8_t min_s;		/*minimum password size*/
	uint8_t max_s;		/*maximum password size*/
	#if __LP64__
	uint64_t total;		/*total of passwords that will be generated (64bits)*/
	#else
	uint32_t total;		/*total of passwords that will be generated (32bits)*/
	#endif
	struct sets charsets;	/*sets of characters for the password list generation*/
};

/*! 
 * Struct p_config_recover
 *
 * It holds information relative to the recovering process.
 */
struct p_config_restore {
	const char *out_name;
	const char *save_name;
	#ifdef __LP64__
	uint64_t *remaining;
	#else
	uint32_t *remaining;
	#endif
};

/*!
 * Struct p_config_extra
 *
 * It holds information relative to the way the passwords will be generated.
 */
struct p_config_extra {
	uint16_t buffer_size;
	uint8_t num_threads;
	uint8_t interval;
	uint8_t out_fd;	
	uint8_t save_fd;
	uint8_t save;
	uint8_t load;
	struct p_config_restore restore;
};

/*!
 * Get a new p_config configuration structure
 *
 * @return return a struct of type p_config
*/
__attribute__((malloc, warn_unused_result))
struct p_config *new_p_config			(void);

/*!
 * Get a new p_config_extra configuration structure
 *
 * @return a struct of type p_config_extra
 */
__attribute__((malloc, warn_unused_result))
struct p_config_extra *new_p_config_extra	(void);

/*!
 * Set the minimum size to the passwords that will be generated
 *
 * @param minimum size of the passwords
 * @param p_config structure
 */
__attribute__((flatten)) 
void p_config_set_min_s		(const uint8_t, struct p_config *const);

/*!
 * Set the maximum size to the passwords that will be generated
 *
 * @param maximum size of the passwords
 * @param p_config structure
 */
__attribute__((flatten)) 
void p_config_set_max_s		(const uint8_t, struct p_config *const);

/*!
 * Set the minimum and the maximum size equals to the size specified
 *
 * @param password size
 * @param p_config struct
 */
__attribute__((flatten)) 
void p_config_set_size		(const uint8_t, struct p_config *const);

/*!
 * Set the buffer size, so it minimizes the file or screen writing process
 *
 * @param buffer size in bytes
 * @param p_config_extra structure
 */
void p_config_set_buffer_size		(const uint16_t, struct p_config_extra *const);

/*!
 * Get the buffer size that will be used in the passwords generation
 *
 * @return the size of the buffer size
 **/
uint16_t p_config_get_buffer_size	(const struct p_config_extra *const);

/*!
 * Open the file to save the passwords generated
 *
 * @param name of the file to save the generated passwords
 * @param p_config_extra structure
 */
void p_config_open_out_file		(const char *const, struct p_config_extra *const);

/*!
 * Set the time interval in which the passwords generation state will be save
 *
 * @param time interval
 * @param p_config_extra structure
*/
void p_config_set_interval		(const uint8_t, struct p_config_extra *const);

/*!
 * Set or unset the saving mode for the password generation
 *
 * @param name of the file to save the passwords generation state
 * @param p_config_extra structure
*/
void p_config_set_save			(const char *const, struct p_config_extra *const);

/*!
 * Set the number of threads to be used in the passwords generation
 *
 * @param number of threads to be used in the passwords generation
 * @param p_config_extra structure
 */
void p_config_set_num_threads		(const uint8_t, struct p_config_extra *const);

/*!
 * Restore the passwords generation that was interupted for some reason
 *
 * @param struct p_config
 * @param struct p_config_extra
 * @param file name from where it will be restored
 */
void p_config_restore			(struct p_config *const, struct p_config_extra * const, const char *const);

/*!
 * Get the total of passwords that will be generated
 *
 * @param struct p_config
 * @return total of passwords
 **/
#if __LP64__
uint64_t p_config_get_total 		(const struct p_config *const);
#else
uint32_t p_config_get_total 		(const struct p_config *const);
#endif

/*!
 * Set the characters to be used as the base for the password generation
 *
 * @param base set characters
 * @param p_config struct
 */
__attribute__((flatten)) 
void p_config_base_set			(char *const, struct p_config *const);

/*!
 * Set another character set in a specific position to substitute the base set for that position
 *
 * @param substitution set
 * @param index to substitute
 * @param p_config struct
*/
__attribute__((flatten)) 
void p_config_sub_chars_from		(char *const, const uint8_t, struct p_config *const);

/*!
 * Remove a substitution set that was set
 *
 * @param index to remove
 * @param p_config struct
 */
__attribute__((flatten)) 
void p_config_rm_sub_set		(const uint8_t , struct p_config *const);

/*!
 * Generate the passwords based on the configurations in the p_config and p_config_extra struct
 *
 * @param p_config struct
 * @param p_config_extra struct
 **/
__attribute__((hot)) 
void generate 				(struct p_config *, struct p_config_extra *);

__attribute__((flatten)) 
void free_p_config 			(struct p_config *const);

__attribute__((flatten))
void free_p_config_extra 		(struct p_config_extra *const);

__attribute__((flatten)) 
void print_p_config 			(const struct p_config *);

#endif
