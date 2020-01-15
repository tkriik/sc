#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bf.h"

int
bf_init(struct bf *bf, size_t nwords)
{
	assert(bf != NULL);
	assert(0 < nwords);

	bf->nbits = nwords * BF_WORDBITS;
	bf->nwords = nwords;
	bf->words = calloc(nwords, BF_WORDBYTES);
	if (bf->words == NULL)
		return -1;

	return 0;
}

void
bf_set(struct bf *bf, uint64_t i)
{
	assert(bf != NULL);
	assert(i < BF_BITCNT(bf));

	size_t w_i = i / BF_WORDBITS;
	size_t b_i = i % BF_WORDBITS;

	bf->words[w_i] |= (0x1UL << b_i);
}

int
bf_get(struct bf *bf, uint64_t i)
{
	assert(bf != NULL);
	assert(i < BF_BITCNT(bf));

	size_t w_i = i / BF_WORDBITS;
	size_t b_i = i % BF_WORDBITS;

	return (bf->words[w_i] >> b_i) & 0x1;
}

void
bf_dbg(struct bf *bf)
{
	assert(bf != NULL);

	for (size_t w_i = 0; w_i < bf->nwords; w_i++) {
		size_t start = 8 * w_i * BF_WORDBYTES;
		size_t end = (start + BF_WORDBITS) - 1;
		printf("%4zu - %3zu:", start, end);
		for (size_t b_i = 0; b_i < BF_WORDBITS; b_i++) {
			if (b_i % 8 == 0)
				putchar(' ');

			int b = (bf->words[w_i] >> b_i) & 0x1;
			printf("%d", b);
		}

		putchar('\n');
	}
}
