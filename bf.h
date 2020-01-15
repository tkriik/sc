#ifndef BF_H
#define BF_H

#include <stddef.h>
#include <stdint.h>

#define BF_WORDBYTES sizeof(unsigned long)
#define BF_WORDBITS (8 * sizeof(unsigned long))
#define BF_BITCNT(bf) ((bf)->nwords * BF_WORDBITS)

struct bf {
	uint64_t	nbits;
	size_t		nwords;
	unsigned long	*words;
};

int	bf_init(struct bf *, size_t);
void	bf_set(struct bf *, uint64_t);
int	bf_get(struct bf *, uint64_t);
void	bf_dbg(struct bf *);

#endif
