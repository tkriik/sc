#include <assert.h>

#include "fnv1a.h"

#define FNV_PRIME64 1099511628211ULL
#define FNV_BASIS64 14695981039346656037ULL  

uint64_t
fnv1a_hash64(const void *p, size_t len)
{
	assert(p != NULL);

	const uint8_t *data = p;
	uint64_t h = FNV_BASIS64;
	for (size_t i = 0; i < len; i++) {
		h ^= data[i];
		h *= FNV_PRIME64;
	}

	return h;
}
