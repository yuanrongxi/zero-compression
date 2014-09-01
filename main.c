// zero_comp.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "zero_compression.h"

#define SRC_SIZE 4096

void test_all_zero()
{
	uint8_t src[SRC_SIZE], dst[SRC_SIZE];
	uint8_t mid[SRC_SIZE] = {0};
	int32_t i, len;
	int32_t b, e;

	for(i = 0; i < SRC_SIZE; ++ i)
		src[i] = 0;

	len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE);
	if(len > 0){
		int size = proto_unpack(mid, len, dst, SRC_SIZE);
		printf("comp size = %d, dst size = %d\r\n", len, size);
		if(memcmp(src, dst, size) == 0)
			printf("zero comp ok\n");
		else 
			printf("failed\n");
	}

	b = GetTickCount();
	for(i = 0; i < 10000; i ++){
		len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE);
		if(len > 0){
			int size = proto_unpack(mid, len, dst, SRC_SIZE);
		}
	}

	e = GetTickCount();
	printf("10000 count comp, 4K all zero data, delay = %dms\r\n", e - b);
}

void test_all_char()
{
	uint8_t src[SRC_SIZE], dst[SRC_SIZE];
	uint8_t mid[SRC_SIZE + 10] = {0};
	int32_t i, len;
	int32_t b, e;

	for(i = 0; i < SRC_SIZE; ++ i)
		src[i] = rand()%256;

	len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE + 10);
	if(len > 0){
		int size = proto_unpack(mid, len, dst, SRC_SIZE);
		printf("src size = %d, comp size = %d, dst size = %d\r\n", SRC_SIZE, len, size);
		if(memcmp(src, dst, size) == 0)
			printf("char comp ok\n");
		else 
			printf("failed\n");
	}

	b = GetTickCount();
	for(i = 0; i < 10000; i ++){
		len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE + 10);
		if(len > 0){
			int size = proto_unpack(mid, len, dst, SRC_SIZE);
		}
	}

	e = GetTickCount();

	printf("10000 count comp, 4K all char data, delay = %dms\r\n", e - b);
}

void test_random_value()
{
	uint8_t src[SRC_SIZE], dst[SRC_SIZE];
	uint8_t *mid[SRC_SIZE + 10];
	int32_t i, len;
	int32_t b, e;

	srand(time(NULL));
	for(i = 0; i < SRC_SIZE; ++ i)
		src[i] = rand()%3;

	len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE + 10);
	if(len > 0){
		int32_t size = proto_unpack(mid, len, dst, SRC_SIZE);
		printf("src size = %d, comp size = %d, dst size = %d\r\n", SRC_SIZE, len, size);
		if(memcmp(src, dst, size) == 0)
			printf("random char comp ok\n");
		else 
			printf("failed\n");
	}


	b = GetTickCount();
	for(i = 0; i < 10000; i ++){
		len = proto_pack(src, SRC_SIZE, mid, SRC_SIZE + 10);
		if(len > 0){
			int size = proto_unpack(mid, len, dst, SRC_SIZE);
		}
	}

	e = GetTickCount();
	printf("10000 count comp, 4K all char data, delay = %dms\r\n", e - b);
}

int main(int argc, const char* argv[])
{
	test_all_zero();
	test_all_char();
	test_random_value();
	return 0;
}

