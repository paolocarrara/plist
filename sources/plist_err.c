#include "../include/plist_err.h"

void plist_err (uint8_t err)
{
	switch (err) {
	
		case PLIST_ERR_NULL:
			printf ("ERROR: NULL VALUE AS ARGUMENT\n");
			break;

		case PLIST_ERR_NULL_MALLOC:
			printf ("ERROR: INTERNAL MALLOC RETURNED NULL\n");
			exit (1);
			break;
		case PLIST_ERR_NDX_OUT:
			printf ("ERROR: TRYING TO ACCESS OUT OF SCOPE INDEX\n");
			break;
	}
}
