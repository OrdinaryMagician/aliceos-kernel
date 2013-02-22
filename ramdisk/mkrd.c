/*
	mkrd.c : Create a simple ram disk for AOS.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "mkrd.h"

int main( int argc, char **argv )
{
	if ( argc <= 1 )
	{
		fprintf(stderr,"usage: %s DISK_NAME [FILES...]\n",argv[0]);
		return 1;
	}
	rd_header_t head;
	memcpy(&head.magic,&aosrd_hdmagic,4);
	rd_entry_t *ents = NULL;
	int fdes;
	if ( (fdes = open(argv[1],O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1 )
	{
		fprintf(stderr,"could not create file \"%s\"\n",argv[1]);
		return 1;
	}
	uint32_t elems = 0;
	uint32_t rdpos = sizeof(rd_header_t);
	int i;
	struct stat finf;
	for ( i=2; i<argc; i++ )
	{
		if ( stat(argv[i],&finf) == -1 )
		{
			fprintf(stderr,"could not stat file \"%s\"\n",argv[i]);
			close(fdes);
			return 1;
		}
		if ( !S_ISREG(finf.st_mode) )
		{
			fprintf(stderr,"file type not valid for \"%s\"\n",argv[i]);
			close(fdes);
			return 1;
		}
		elems++;
		if ( ents )
			ents = (rd_entry_t*)realloc(ents,elems*sizeof(rd_entry_t));
		else
			ents = (rd_entry_t*)malloc(elems*sizeof(rd_entry_t));
		char *basenm = strrchr(argv[i],'/');
		strncpy(ents[elems-1].name,(basenm)?(basenm+1):argv[i],256);
		ents[elems-1].start = 0;
		ents[elems-1].size = finf.st_size;
		rdpos += sizeof(rd_entry_t);
	}
	head.numents = elems;
	write(fdes,&head,sizeof(rd_header_t));
	unsigned int j;
	for ( j=0; j<elems; j++ )
	{
		ents[j].start = rdpos;
		rdpos += ents[j].size;
		write(fdes,&ents[j],sizeof(rd_entry_t));
	}
	for ( i=2; i<argc; i++ )
	{
		int fdesr;
		if ( (fdesr = open(argv[i],O_RDONLY)) == -1 )
		{
			fprintf(stderr,"could not open \"%s\" for reading\n",argv[i]);
			close(fdes);
			return 1;
		}
		char buf[131072];
		ssize_t bsiz;
		do
		{
			bsiz = read(fdesr,&buf[0],131072);
			write(fdes,&buf[0],bsiz);
		}
		while ( bsiz > 0 );
		close(fdesr);
	}
	write(fdes,&aosrd_trmagic,4);
	write(fdes,aosrd_trsig,28);
	close(fdes);
	return 0;
}
