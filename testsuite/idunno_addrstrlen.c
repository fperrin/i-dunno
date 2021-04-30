#include <i-dunno.h>
#include "unicodehelpers.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

void compute_min_max(int size, int start, int len, int *min, int *max)
{
	for (int i = 0; i < utf8strides_max; i++) {
		int stride = utf8strides[i];
		int newlen = len + (i + 1);
		int newstart = start + stride;

		/* reached the end? */
		if (newstart >= size) {
			if (newlen < *min)
				*min = newlen;
			if (newlen > *max)
				*max = newlen;
		} else {
			compute_min_max(size, newstart, newlen, min, max);
		}
	}
}

int main(int argc, char **argv)
{
	int min = INT_MAX;
	int max = 0;
	compute_min_max(32, 0, 0, &min, &max);
	printf("For 32-bit addresses, min=%d, max=%d\n", min, max);

	min = INT_MAX;
	max = 0;
	compute_min_max(128, 0, 0, &min, &max);
	printf("For 128-bit addresses, min=%d, max=%d\n", min, max);

	assert (max + 1 == I_DUNNO_ADDRSTRLEN);

	return 0;
}
