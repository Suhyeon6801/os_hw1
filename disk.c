#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "disk.h"

int fd;

void DevCreateDisk(void) //가상 디스크를 생성하는 함수. 이미 가상 디스크가 존재한다면 삭제하고 다시 생성시킴.
{
	fd = open("MY_DISK", O_RDWR | O_CREAT | O_TRUNC, 0644);
}

void DevOpenDisk(void) //가상 디스크를 생성하지는 않고 단지 open하는 함수. 이전에 저장된 내용을 그대로 저장하고 있음.
{
	fd = open("MY_DISK", O_RDWR);
}

void __DevMoveBlock(int blkno)
{
	lseek(fd, (off_t)+(BLOCK_SIZE * blkno), SEEK_SET);
}

void DevReadBlock(int blkno, char *pBuf)
{
	__DevMoveBlock(blkno);
	read(fd, pBuf, BLOCK_SIZE);
}

void DevWriteBlock(int blkno, char *pBuf)
{
	__DevMoveBlock(blkno);
	write(fd, pBuf, BLOCK_SIZE);
}