#ifndef __ZERO_COMPRESSION_H_
#define __ZERO_COMPRESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*protocol zero compression*/
int32_t proto_pack(const void* src, int32_t src_size, void* dst, int32_t dst_size);

/*protocal zerok uncompression*/
int32_t proto_unpack(const void* src, int32_t src_size, void* dst, int32_t dst_size);

#ifdef __cplusplus
}
#endif

#endif