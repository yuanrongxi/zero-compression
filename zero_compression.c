#include "zero_compression.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static void write_ff(const uint8_t* src, uint8_t* dst, int32_t n){
	dst[0] = 0xff;
	dst[1] = n - 1;
	memcpy(dst + 2, src, n * 8);
}

static int32_t pack_seg(const uint8_t* src, uint8_t* buffer, int32_t sz, int32_t n)
{
	uint8_t header = 0;
	int32_t notzero = 0;
	int32_t i;
	uint8_t* obuffer = buffer;

	/*header标识位*/
	++ buffer;
	-- sz;

	if(sz <= 0)
		obuffer = NULL;

	/*进行8字节bitmap描述*/
	for(i = 0; i < 8; i++){
		if(src[i] != 0){
			notzero ++;
			header |= 1 << i;
			if(sz > 0){
				*buffer = src[i];
				buffer ++;
				sz --;
			}
		}
	}

	if((notzero == 7 || notzero == 6) && n > 0) /*如果是6个以上单元不为0，直接合并到前面FF描述中，这样可以节省1个字节空间*/
		notzero = 8;

	/*不为0片段*/
	if(notzero == 8){
		if(n > 0)
			return 8;
		else  /*第一个全不为0的8字节片段*/
			return 10;
	}

	/*设置标识位*/
	if(obuffer != NULL) 
		*obuffer = header;

	return notzero + 1;
}

int32_t proto_pack(const void* src, int32_t src_size, void* dst, int32_t dst_size)
{
	uint8_t tmp[8];
	int32_t i, ff_n = 0, size = 0;

	const int8_t* ff_srcstart = NULL;
	uint8_t* ff_deststart = NULL;
	const uint8_t* src_p = src;
	uint8_t* buffer = dst;

	for(i = 0; i < src_size; i += 8){ 
		/*判断是否要0补齐*/
		int32_t n = 0, j = 0;
		int32_t padding = i + 8 - src_size;
		if(padding > 0){
			memcpy(tmp, src_p, 8 - padding);
			for(j = 0; j < padding; j ++)
				tmp[7 - j] = 0;

			src_p = tmp;
		}

		/*进行8字节为单位的片段打包*/
		n = pack_seg(src_p, buffer, dst_size, ff_n);
		dst_size -= n;

		/*第一个连续8个字节都不为0,记录初始位置*/
		if(n == 10){
			ff_srcstart = src_p;
			ff_deststart = buffer;
			ff_n = 1;
		}
		else if(n == 8 && ff_n > 0){/*连续的8字节不为0*/
			++ ff_n;
			if(ff_n >= 256){ /*最多256个连续不为0的8字节*/
				write_ff(ff_srcstart, ff_deststart, 256);
				ff_n = 0;
			}
		}
		else{
			if(ff_n > 0)
				write_ff(ff_srcstart, ff_deststart, ff_n);

			ff_n = 0;
		}

		/*进行下一轮8字节判断*/
		src_p += 8;
		buffer += n;
		size += n;
	}

	if(ff_n > 0 && dst_size >= 0)
		write_ff(ff_srcstart, ff_deststart, ff_n);

	return size;
}

int32_t proto_unpack(const void* srcv, int32_t src_size, void* dstv, int32_t dst_size)
{
	uint8_t header = 0;
	const uint8_t* src = srcv;
	uint8_t* buffer = dstv;
	int32_t size = 0, n = 0;

	while(src_size > 0){
		header = src[0];
		-- src_size;
		++ src;
		if(header == 0xff){ /*连续的8字节片段不为0*/
			if(src_size < 0)
				return -1;

			/*计算片段数量*/
			n = (src[0] + 1) * 8;
			if(src_size < n + 1)
				return -1;

			src_size -= n + 1;
			++ src;

			/*进行数据拷贝*/
			if(dst_size >= n)
				memcpy(buffer, src, n);

			dst_size -= n;
			buffer += n;
			src += n;
			size += n;
		}
		else {
			int32_t i;
			for (i = 0; i < 8; i++) { /*解码非0的值*/
				int nz = (header >> i) & 1;
				if(nz){
					if(src_size < 0)
						return -1;

					if(dst_size > 0){
						*buffer = *src;
						-- dst_size;
						++ buffer;
					}
					++ src;
					-- src_size;
				} 
				else {/*0填充*/
					if (dst_size > 0){
						*buffer = 0;
						-- dst_size;
						++ buffer;
					}
				}

				++ size;
			}
		}
	}

	return size;
}

#ifdef __cplusplus
}
#endif


