/*
	dmprd.c : Dump AOS ramdisk contents.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mkrd.h"
int dump( int fd, char fname[256], uint32_t start, uint32_t size )
{
	unsigned int i;
	char c;
	int fdest = open(fname,O_WRONLY|O_CREAT|O_TRUNC,
			S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if ( fdest == -1 )
		return 1;
	lseek(fd,start,SEEK_SET);
	for ( i=0; i<size; i++ )
	{
		read(fd,&c,1);
		write(fdest,&c,1);
	}
	close(fdest);
	return 0;
}
int main( int argc, char **argv )
{
	if ( argc <= 1 )
	{
		fprintf(stderr,"usage: %s DISK_NAME [dump]\n",argv[0]);
		return 1;
	}
	int dodump = 0;
	if ( argc > 2 )
		dodump = !strcmp(argv[2],"dump");
	int fdes = open(argv[1],O_RDONLY);
	if ( fdes == -1 )
	{
		fprintf(stderr,"could not open \"%s\" for reading\n",argv[1]);
		return 1;
	}
	long int pos = 0;
	rd_header_t head;
	pos += read(fdes,&head,RDHEAD_SIZ);
	if ( head.magic != AOSRD_HDMAGIC )
	{
		fprintf(stderr,"\"%s\" is not a valid AOS ramdisk\n",argv[1]);
		fprintf(stderr,"file magic:     %#08X\n",head.magic);
		fprintf(stderr,"expected magic: %#08X\n",AOSRD_HDMAGIC);
		close(fdes);
		return 1;
	}
	rd_entry_t ent;
	unsigned int i;
	for ( i=0; i<head.numents; i++ )
	{
		pos += read(fdes,&ent,RDENT_SIZ);
		printf("\"%s\" (%u bytes)\n",ent.name,ent.size);
		if ( dodump && dump(fdes,ent.name,ent.start,ent.size) )
		{
			fprintf(stderr,"error dumping \"%s\"\n",ent.name);
			close(fdes);
			return 1;
		}
		lseek(fdes,pos,SEEK_SET);
	}
	close(fdes);
	return 0;
}
