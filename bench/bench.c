#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>

#include "stif.h"

#define PROGNAME "bench"

#define FATAL(msg) fatal(__FILE__, __LINE__, msg)
static inline void fatal(const char *file, int line, const char *message)
{
	fprintf(stderr, "fatal error: (%s, %d): %s\n", file, line, message);
	exit(EXIT_FAILURE);
}

static void usage(void)
{
	fprintf(stderr, "usage: %s [options]\n", PROGNAME);
	fprintf(stderr, "\t--help,-h      : display this help\n");
	fprintf(stderr, "\t--input,-i     : stif image to parse\n");
	fprintf(stderr, "\t--ntimes,-n     : number of time to repeat the parse\n");
	exit(EXIT_FAILURE);
}

static unsigned char *read_file(const char *input_file, size_t *size)
{
	FILE *file = NULL;
	unsigned char *data;
	size_t nmemb;

	assert(input_file != NULL);
	assert(size != NULL);

	file = fopen(input_file, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "error when opening file \"%s\": %s\n", input_file, strerror(errno));
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	*size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	data = malloc(*size);
	if ( data == NULL )
	{
		fclose(file);
		return NULL;
	}

	nmemb = fread(data, *size, 1, file);
	fclose(file);
	if (nmemb != 1)
	{
		fprintf(stderr, "error when reading file \"%s\": %s\n", input_file, strerror(errno));
		free(data);
		return NULL;
	}

	return data;
}

static struct option long_options[] =
{
	{"help",  no_argument,       NULL, 'h' },
	{"input", required_argument, NULL, 'i' },
	{"ntimes", required_argument, NULL, 'n' },
	{NULL,    0,                 NULL, 0   },
};

int main(int argc, char *argv[])
{
	int c, ret = EXIT_FAILURE;
	char *input_fn = NULL;
	size_t times = 30;
	unsigned char *buffer;
	size_t buffer_size;
	stif_t *image = NULL;

	/* Parse program parameters
	 */
	while ( (c = getopt_long(argc, argv, "hi:n:", long_options, NULL)) != -1 )
	{
		switch ( c )
		{
			case '?': /* FALLTHROUGH */
			case 'h':
				usage();
				break;
			case 'i':
			{
				input_fn = strdup(optarg);
				if ( input_fn == NULL )
					FATAL("unable to allocate input filename");
				break;
			}
			case 'n':
			{
				times = (size_t)atoi(optarg);
				break;
			}
		}
	}

	/* Verify there is an input file
	 */
	if ( input_fn == NULL )
	{
		fprintf(stderr, "missing stif input file\n");
		usage();
	}

	/* Reading input file
	 */
	buffer = read_file(input_fn, &buffer_size);
	if ( buffer == NULL )
	{
		fprintf(stderr, "unable to read stif file\n");
		goto end;
	}

	for (size_t i = 0; i < times; i++)
	{
		image = parse_stif(buffer, buffer_size);
		if ( image == NULL )
		{
			fprintf(stderr, "unable to parse stif file\n");
			goto end;
		}
		stif_free(image);
	}

	ret = EXIT_SUCCESS;

end:
	free(buffer);
	free(input_fn);

	return ret;
}
