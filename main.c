#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bf.h"
#include "fnv1a.h"

#define DEFAULT_DICT	"/usr/share/dict/words"
#define DEFAULT_KB	64

#define WHITESPC	" \n\r\t\v"
#define IGNORE		WHITESPC "!\"@#£¤$%‰&/{([)]=}?\\+´¸'*,.^~:;-_`"

void
usage(void)
{
	extern char *__progname;
	fprintf(stderr, "usage: %s [-d dictionary] [-k kilobytes] [file | -]\n",
	    __progname);
	exit(EXIT_FAILURE);
}

void
parse_dict(const char *filename, struct bf *bf)
{
	assert(filename != NULL);
	assert(bf != NULL);

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		err(1, "can't open dictionary \"%s\"", filename);

	struct stat sb;
	if (fstat(fd, &sb) == -1)
		err(1, "fstat");

	size_t size = (size_t)sb.st_size;
	const char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED)
		err(1, "mmap");

	size_t word = 0;
	while (word < size) {
		size_t len = strcspn(data + word, WHITESPC);
		//printf("word: \"%.*s\"\n", (int)len, data + word);
		uint64_t h = fnv1a_hash64(data + word, len);
		bf_set(bf, h % bf->nbits);

		size_t nskip = strspn(data + word + len, WHITESPC);
		word += len + nskip;
	}
}

int
check_file(FILE *in, struct bf *bf)
{
	assert(in != NULL);
	assert(bf != NULL);

	int rc = EXIT_SUCCESS;

	char block[65536];
	size_t nread;

	errno = 0;
	while ((nread = fread(block, 1, sizeof(block), in)) > 0) {
		if (errno != 0)
			err(1, "error while reading file");

		for (size_t i = 0; i < nread; i++)
			block[i] = (char)tolower(block[i]);

		size_t word = 0;
		while (word < nread) {
			size_t len = strcspn(block + word, IGNORE);
			uint64_t h = fnv1a_hash64(block + word, len);
			int found = bf_get(bf, h % bf->nbits);
			if (!found) {
				printf("INVALID: %.*s\n", (int)len,
				    block + word);
				rc = EXIT_FAILURE;
			}

			size_t nskip = strspn(block + word + len, IGNORE);
			word += len + nskip;
		}
	}

	return rc;
}

int
main(int argc, char **argv)
{
	const char *dict = DEFAULT_DICT;
	size_t kb = DEFAULT_KB;
	const char *input = "-";

	int opt;
	while ((opt = getopt(argc, argv, "d:k:")) != -1) {
		switch (opt) {
		case 'd':
			dict = optarg;
			break;
		case 'k':
			kb = (size_t)atoi(optarg);
			break;
		default:
			usage();
			break;
		}
	};

	argc -= optind;
	argv += optind;

	if (argc == 1)
		input = argv[0];
	else if (argc == 2)
		usage();

	FILE *in = NULL;

	if (strcmp(input, "-") == 0)
		in = stdin;
	else {
		in = fopen(input, "r");
		if (in == NULL)
			err(1, "can't open file \"%s\"", input);
	}

	//printf("dict = \"%s\", kb = %zu\n, file = ", dict, kb);

	struct bf bf;
	bf_init(&bf, (kb * 1024) / BF_WORDBYTES);

	parse_dict(dict, &bf);
	//bf_dbg(&bf);

	return check_file(in, &bf);
}
