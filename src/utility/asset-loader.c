#include "utility/asset-loader.h"
#include "utility/function-name.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void *assetLoad(const char *filename, size_t *fileSize)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "%s: Failed to open file \"%s\"\n", __FUNCTION_NAME__, filename);
		if (fileSize)
			*fileSize = 0;
		return NULL;
	}

	// Calculate the size of the file
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the whole file into memory
	static const size_t nullTerminationSize = sizeof(char); // For the null terminator
	void *asset = malloc(size + nullTerminationSize);
	uint8_t *string = (uint8_t *)asset;
	fread(string, size, 1, file);
	fclose(file);

	// Null-terminate the string
	string[size] = 0;
	if (fileSize)
		*fileSize = size;
	return asset;
}
