#ifndef __UTILITY_ASSET_LOADER_H__
#define __UTILITY_ASSET_LOADER_H__

#include <stddef.h>

/**
 * @brief loads asset file with null-terminator, caller should delete returned pointer
 *
 * @param[in] filename asset file name
 * @param[out] size asset file size (optional, can be NULL)
 */
void *assetLoad(const char *filename, size_t *size);

#endif
