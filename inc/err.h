/**
 * @file err.h
 *
 * @brief Error macros & defines
 *
 * @author Lars Wander 
 */

#ifndef _ERR_H_
#define _ERR_H_

#include <assert.h>

#define ERR_OOB (-3)

#define ERR_INP (-8)
#define ERR_CORRUPT (-9)
#define ERR_FILE_ACTION (-10)
#define ERR_MEM_ALLOC (-11)
#define ERR_SEMANTICS (-12)
#define ERR_BAD_PARSE (-13)

#endif /* _ERR_H_ */
