#include "Compression.h"
#include "MiniWorld.h"

#define SIZE MINIWORLD_W*MINIWORLD_H*MINIWORLD_D*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)


// TODO (value en uint8_t ?)

void chunkCompression(
		chunkVal* data,
		FILE* file
)
{
	uint16_t value;
	uint8_t repeat;
	unsigned int i=0;
	while(i<SIZE)
	{
		value = data[i];
		repeat = 0 ;
		i++;
		while( i<SIZE and data[i] == value and repeat<0xFF)
		{
			repeat++;
			i++;
		}
		fwrite(&value,sizeof(uint16_t),1,file);
		fwrite(&repeat,sizeof(uint8_t),1,file);
	}
}

void chunkDecompression(
		FILE* file,
		chunkVal* data
)
{
	// on obtient la taille du fichier
	fseek(file, 0L, SEEK_END);
	int inSize = ftell(file)/3; // (3 = nombre de octet dans repeat et value)
	fseek(file, 0L, SEEK_SET);

	// on decompresse dans data
	uint16_t value;
	uint8_t repeat;
	unsigned int i=0;
	unsigned int j=0;
	while(i<inSize)
	{
		fread(&value,sizeof(uint16_t),1,file);
		fread(&repeat,sizeof(uint8_t),1,file);
		memset(data+j,value,repeat+1); j+=repeat+1;
		i++;
	}
}
