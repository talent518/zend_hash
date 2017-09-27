/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_HASH_H
#define ZEND_HASH_H

#include <sys/types.h>

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_LONG 2
#define HASH_KEY_NON_EXISTENT 3
#define HASH_KEY_NON_EXISTANT HASH_KEY_NON_EXISTENT /* Keeping old define (with typo) for backward compatibility */

#define HASH_UPDATE 		(1<<0)
#define HASH_ADD			(1<<1)
#define HASH_NEXT_INSERT	(1<<2)

#define HASH_DEL_KEY 0
#define HASH_DEL_INDEX 1
#define HASH_DEL_KEY_QUICK 2

#define HASH_UPDATE_KEY_IF_NONE    0
#define HASH_UPDATE_KEY_IF_BEFORE  1
#define HASH_UPDATE_KEY_IF_AFTER   2
#define HASH_UPDATE_KEY_ANYWAY     3

// custom begin
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_NULL		0
#define IS_STRING	6
#define IS_LONG		1

typedef unsigned char zend_bool;
typedef unsigned char zend_uchar;
typedef unsigned int zend_uint;
typedef unsigned long zend_ulong;
typedef unsigned short zend_ushort;

#ifdef __cplusplus
	#define BEGIN_EXTERN_C() extern "C" {
	#define END_EXTERN_C() }
#else
	#define BEGIN_EXTERN_C()
	#define END_EXTERN_C()
#endif

#include <stdarg.h>

#ifdef ZEND_WIN32
/* Only use this macro if you know for sure that all of the switches values
   are covered by its case statements */
#define EMPTY_SWITCH_DEFAULT_CASE() \
			default:				\
				__assume(0);		\
				break;
#else
#define EMPTY_SWITCH_DEFAULT_CASE()
#endif

#define SIZEOF_LONG 4
#define LONG_MAX 2147483648

#if SIZEOF_LONG == 4
#define MAX_LENGTH_OF_LONG 11
static const char long_min_digits[] = "2147483648";
#elif SIZEOF_LONG == 8
#define MAX_LENGTH_OF_LONG 20
static const char long_min_digits[] = "9223372036854775808";
#else
#error "Unknown SIZEOF_LONG"
#endif

#define MAX_LENGTH_OF_DOUBLE 32

typedef enum {
	SUCCESS =  0,
	FAILURE = -1,		/* this MUST stay a negative number, or it may affect functions! */
} ZEND_RESULT_CODE;

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define zend_always_inline inline __attribute__((always_inline))
#define zend_never_inline __attribute__((noinline))
#else
#define zend_always_inline inline
#define zend_never_inline
#endif /* __GNUC__ >= 3 */
#elif defined(_MSC_VER)
#define zend_always_inline __forceinline
#define zend_never_inline
#else
#define zend_always_inline inline
#define zend_never_inline
#endif

#if (defined (__GNUC__) && __GNUC__ > 2 ) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX)
# define EXPECTED(condition)   __builtin_expect(condition, 1)
# define UNEXPECTED(condition) __builtin_expect(condition, 0)
#else
# define EXPECTED(condition)   (condition)
# define UNEXPECTED(condition) (condition)
#endif

#define IS_INTERNED(s) (0)
#define INTERNED_HASH(s) \
	(((Bucket*)(((char*)(s))-sizeof(Bucket)))->h)

#include <assert.h>
#define ZEND_ASSERT(exp) assert(exp)

#define HANDLE_BLOCK_INTERRUPTIONS()
#define HANDLE_UNBLOCK_INTERRUPTIONS()

///////////////////////////////////////////////////////////////
// custom end

typedef int  (*compare_func_t)(const void *, const void *);
typedef void (*sort_func_t)(void *, size_t, register size_t, compare_func_t);
typedef void (*dtor_func_t)(void *pDest);
typedef void (*copy_ctor_func_t)(void *pElement);

struct _hashtable;

typedef struct bucket {
	ulong h;						/* Used for numeric indexing */
	uint nKeyLength;
	void *pData;
	void *pDataPtr;
	struct bucket *pListNext;
	struct bucket *pListLast;
	struct bucket *pNext;
	struct bucket *pLast;
	const char *arKey;
} Bucket;

typedef struct _hashtable {
	uint nTableSize;
	uint nTableMask;
	uint nNumOfElements;
	ulong nNextFreeElement;
	Bucket *pInternalPointer;	/* Used for element traversal */
	Bucket *pListHead;
	Bucket *pListTail;
	Bucket **arBuckets;
	dtor_func_t pDestructor;
	unsigned char nApplyCount;
	zend_bool bApplyProtection;
} HashTable;


typedef struct _zend_hash_key {
	const char *arKey;
	uint nKeyLength;
	ulong h;
} zend_hash_key;


typedef zend_bool (*merge_checker_func_t)(HashTable *target_ht, void *source_data, zend_hash_key *hash_key, void *pParam);

typedef Bucket* HashPosition;

BEGIN_EXTERN_C()

/* startup/shutdown */
int _zend_hash_init(HashTable *ht, uint nSize, dtor_func_t pDestructor);
int _zend_hash_init_ex(HashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool bApplyProtection);
void zend_hash_destroy(HashTable *ht);
void zend_hash_clean(HashTable *ht);
#define zend_hash_init(ht, nSize, pDestructor)						_zend_hash_init((ht), (nSize), (pDestructor))
#define zend_hash_init_ex(ht, nSize, pDestructor, bApplyProtection)		_zend_hash_init_ex((ht), (nSize), (pDestructor), (bApplyProtection))

/* additions/updates/changes */
int _zend_hash_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag);
#define zend_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest) \
		_zend_hash_add_or_update(ht, arKey, nKeyLength, pData, nDataSize, pDest, HASH_UPDATE)
#define zend_hash_add(ht, arKey, nKeyLength, pData, nDataSize, pDest) \
		_zend_hash_add_or_update(ht, arKey, nKeyLength, pData, nDataSize, pDest, HASH_ADD)

int _zend_hash_quick_add_or_update(HashTable *ht, const char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag);
#define zend_hash_quick_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest) \
		_zend_hash_quick_add_or_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest, HASH_UPDATE)
#define zend_hash_quick_add(ht, arKey, nKeyLength, h, pData, nDataSize, pDest) \
		_zend_hash_quick_add_or_update(ht, arKey, nKeyLength, h, pData, nDataSize, pDest, HASH_ADD)

int _zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag);
#define zend_hash_index_update(ht, h, pData, nDataSize, pDest) \
		_zend_hash_index_update_or_next_insert(ht, h, pData, nDataSize, pDest, HASH_UPDATE)
#define zend_hash_next_index_insert(ht, pData, nDataSize, pDest) \
		_zend_hash_index_update_or_next_insert(ht, 0, pData, nDataSize, pDest, HASH_NEXT_INSERT)


#define ZEND_HASH_APPLY_KEEP				0
#define ZEND_HASH_APPLY_REMOVE				1<<0
#define ZEND_HASH_APPLY_STOP				1<<1

typedef int (*apply_func_t)(void *pDest);
typedef int (*apply_func_arg_t)(void *pDest, void *argument);
typedef int (*apply_func_args_t)(void *pDest, int num_args, va_list args, zend_hash_key *hash_key);

void zend_hash_graceful_destroy(HashTable *ht);
void zend_hash_graceful_reverse_destroy(HashTable *ht);
void zend_hash_apply(HashTable *ht, apply_func_t apply_func);
void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *);
void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int, ...);

/* This function should be used with special care (in other words,
 * it should usually not be used).  When used with the ZEND_HASH_APPLY_STOP
 * return value, it assumes things about the order of the elements in the hash.
 * Also, it does not provide the same kind of reentrancy protection that
 * the standard apply functions do.
 */
void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func);


/* Deletes */
int zend_hash_del_key_or_index(HashTable *ht, const char *arKey, uint nKeyLength, ulong h, int flag);
#define zend_hash_del(ht, arKey, nKeyLength) \
		zend_hash_del_key_or_index(ht, arKey, nKeyLength, 0, HASH_DEL_KEY)
#define zend_hash_quick_del(ht, arKey, nKeyLength, h) \
		zend_hash_del_key_or_index(ht, arKey, nKeyLength, h, HASH_DEL_KEY_QUICK)
#define zend_hash_index_del(ht, h) \
		zend_hash_del_key_or_index(ht, NULL, 0, h, HASH_DEL_INDEX)
#define zend_get_hash_value \
		zend_hash_func

/* Data retreival */
int zend_hash_find(const HashTable *ht, const char *arKey, uint nKeyLength, void **pData);
int zend_hash_quick_find(const HashTable *ht, const char *arKey, uint nKeyLength, ulong h, void **pData);
int zend_hash_index_find(const HashTable *ht, ulong h, void **pData);

/* Misc */
int zend_hash_exists(const HashTable *ht, const char *arKey, uint nKeyLength);
int zend_hash_quick_exists(const HashTable *ht, const char *arKey, uint nKeyLength, ulong h);
int zend_hash_index_exists(const HashTable *ht, ulong h);
ulong zend_hash_next_free_element(const HashTable *ht);

/* traversing */
#define zend_hash_has_more_elements_ex(ht, pos) \
	(zend_hash_get_current_key_type_ex(ht, pos) == HASH_KEY_NON_EXISTENT ? FAILURE : SUCCESS)
int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos);
int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos);
int zend_hash_get_current_key_ex(const HashTable *ht, char **str_index, uint *str_length, ulong *num_index, zend_bool duplicate, HashPosition *pos);
int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos);
int zend_hash_get_current_data_ex(HashTable *ht, void **pData, HashPosition *pos);
void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos);
void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos);
int zend_hash_update_current_key_ex(HashTable *ht, int key_type, const char *str_index, uint str_length, ulong num_index, int mode, HashPosition *pos);

typedef struct _HashPointer {
	HashPosition pos;
	ulong h;
} HashPointer;

int zend_hash_get_pointer(const HashTable *ht, HashPointer *ptr);
int zend_hash_set_pointer(HashTable *ht, const HashPointer *ptr);

#define zend_hash_has_more_elements(ht) \
	zend_hash_has_more_elements_ex(ht, NULL)
#define zend_hash_move_forward(ht) \
	zend_hash_move_forward_ex(ht, NULL)
#define zend_hash_move_backwards(ht) \
	zend_hash_move_backwards_ex(ht, NULL)
#define zend_hash_get_current_key(ht, str_index, num_index, duplicate) \
	zend_hash_get_current_key_ex(ht, str_index, NULL, num_index, duplicate, NULL)
#define zend_hash_get_current_key_type(ht) \
	zend_hash_get_current_key_type_ex(ht, NULL)
#define zend_hash_get_current_data(ht, pData) \
	zend_hash_get_current_data_ex(ht, pData, NULL)
#define zend_hash_internal_pointer_reset(ht) \
	zend_hash_internal_pointer_reset_ex(ht, NULL)
#define zend_hash_internal_pointer_end(ht) \
	zend_hash_internal_pointer_end_ex(ht, NULL)
#define zend_hash_update_current_key(ht, key_type, str_index, str_length, num_index) \
	zend_hash_update_current_key_ex(ht, key_type, str_index, str_length, num_index, HASH_UPDATE_KEY_ANYWAY, NULL)

/* Copying, merging and sorting */
void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size);
void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size, int overwrite);
void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam);
int zend_hash_sort(HashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber);
int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered);
int zend_hash_minmax(const HashTable *ht, compare_func_t compar, int flag, void **pData);

#define zend_hash_merge(target, source, pCopyConstructor, tmp, size, overwrite)					\
	_zend_hash_merge(target, source, pCopyConstructor, tmp, size, overwrite)

int zend_hash_num_elements(const HashTable *ht);

int zend_hash_rehash(HashTable *ht);
void zend_hash_reindex(HashTable *ht, zend_bool only_integer_keys);

void _zend_hash_splice(HashTable *ht, uint nDataSize, copy_ctor_func_t pCopyConstructor, uint offset, uint length, void **list, uint list_count, HashTable *removed);
#define zend_hash_splice(ht, nDataSize, pCopyConstructor, offset, length, list, list_count, removed) \
	_zend_hash_splice(ht, nDataSize, pCopyConstructor, offset, length, list, list_count, removed)

/*
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%. 
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */

static inline ulong zend_inline_hash_func(const char *arKey, uint nKeyLength)
{
	register ulong hash = 5381;

	/* variant with the hash unrolled eight times */
	for (; nKeyLength >= 8; nKeyLength -= 8) {
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
		hash = ((hash << 5) + hash) + *arKey++;
	}
	switch (nKeyLength) {
		case 7: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 6: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 5: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 4: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 3: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 2: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
		case 1: hash = ((hash << 5) + hash) + *arKey++; break;
		case 0: break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return hash;
}


ulong zend_hash_func(const char *arKey, uint nKeyLength);

END_EXTERN_C()

#define ZEND_HANDLE_NUMERIC_EX(key, length, idx, func) do {					\
	register const char *tmp = key;											\
																			\
	if (*tmp == '-') {														\
		tmp++;																\
	}																		\
	if (*tmp >= '0' && *tmp <= '9') { /* possibly a numeric index */		\
		const char *end = key + length - 1;									\
																			\
		if ((*end != '\0') /* not a null terminated string */				\
		 || (*tmp == '0' && length > 2) /* numbers with leading zeros */	\
		 || (end - tmp > MAX_LENGTH_OF_LONG - 1) /* number too long */		\
		 || (SIZEOF_LONG == 4 &&											\
		     end - tmp == MAX_LENGTH_OF_LONG - 1 &&							\
		     *tmp > '2')) { /* overflow */									\
			break;															\
		}																	\
		idx = (*tmp - '0');													\
		while (++tmp != end && *tmp >= '0' && *tmp <= '9') {				\
			idx = (idx * 10) + (*tmp - '0');								\
		}																	\
		if (tmp == end) {													\
			if (*key == '-') {												\
				if (idx-1 > LONG_MAX) { /* overflow */						\
					break;													\
				}															\
				idx = 0 - idx;               									\
			} else if (idx > LONG_MAX) { /* overflow */						\
				break;														\
			}																\
			func;															\
		}																	\
	}																		\
} while (0)

#define ZEND_HANDLE_NUMERIC(key, length, func) do {							\
	ulong idx;																\
																			\
	ZEND_HANDLE_NUMERIC_EX(key, length, idx, return func);					\
} while (0)

static inline int zend_symtable_update(HashTable *ht, const char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest)					\
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_update(ht, idx, pData, nDataSize, pDest));
	return zend_hash_update(ht, arKey, nKeyLength, pData, nDataSize, pDest);
}


static inline int zend_symtable_del(HashTable *ht, const char *arKey, uint nKeyLength)
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_del(ht, idx));
	return zend_hash_del(ht, arKey, nKeyLength);
}


static inline int zend_symtable_find(HashTable *ht, const char *arKey, uint nKeyLength, void **pData)
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_find(ht, idx, pData));
	return zend_hash_find(ht, arKey, nKeyLength, pData);
}


static inline int zend_symtable_exists(HashTable *ht, const char *arKey, uint nKeyLength)
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_index_exists(ht, idx));
	return zend_hash_exists(ht, arKey, nKeyLength);
}

static inline int zend_symtable_update_current_key_ex(HashTable *ht, const char *arKey, uint nKeyLength, int mode, HashPosition *pos)
{
	ZEND_HANDLE_NUMERIC(arKey, nKeyLength, zend_hash_update_current_key_ex(ht, HASH_KEY_IS_LONG, NULL, 0, idx, mode, pos));
	return zend_hash_update_current_key_ex(ht, HASH_KEY_IS_STRING, arKey, nKeyLength, 0, mode, pos);
}
#define zend_symtable_update_current_key(ht,arKey,nKeyLength,mode) \
	zend_symtable_update_current_key_ex(ht, arKey, nKeyLength, mode, NULL)


#endif							/* ZEND_HASH_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
