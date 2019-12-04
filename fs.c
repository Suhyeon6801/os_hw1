#include <stdio.h>
#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
// #define BUFSIZE 512

void FileSysInit(void)
{
    DevCreateDisk();
    //DevOpenDisk();
    char *buffer = malloc(BLOCK_SIZE); //블록크키의 메모리할당
    memset(buffer, 0, sizeof(buffer));
    for (int i = 0; i < 7; i++)//0-6까지 0으로 채움
    {
        DevWriteBlock(i, buffer);
    }
    //char *buffer=calloc(8,BLOCK_SIZE);
    // printf("Init function complete\n");
}

void SetInodeBitmap(int inodeno)
{
    //int inode2 = 1;
    //int inode_numset;
    

    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(1, buffer);           //inodeno_inodemap_block(block1)읽어오기

    //inodeno가 들어갈 block, block당 inodeno개수가 8개씩 8-15번까지는 다음으로
    int num = inodeno / 8;
    int inodeno_inode_set = inodeno % 8;//1로 설정할 inode#
    //printf("start buffer[%d]=%d\n",num,buffer[num]);
    //inode2=1이면, 첫번째inode0과 or operationt시 1로 설정됨->buffer에 저장
    //00000000 | 00100000 = 00100000
    buffer[num] |= 1 << 7 - (inodeno_inode_set);
    //printf("end buffer[%d]=%d\n",num,buffer[num]);
    // memset(buffer, 0, sizeof(buffer));
    // buffer[inodeno]=1;
    //0000000 <<6 => 00000001

    DevWriteBlock(1, buffer);//block1(inodeno_inodemap)에 저장
    // if (inode2 | inodeno)
    // {
    // printf("SetInodeFunction function complete\n");
    //     DevWriteBlock(1, 8);
    // }
}

void ResetInodeBitmap(int inodeno)
{
    

    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(1, buffer);           //block읽어오기
    int num = inodeno / 8;
    int inodeno_inode_set = inodeno % 8;
    //printf("start buffer[%d]=%d\n",num,buffer[num]);
    //inode6=1이면 inode0=0
    //00000010 & ~(00000001)=00000000
    buffer[num] &= buffer[num] << ~(7 - (inodeno_inode_set));
    //printf("end buffer[%d]=%d\n",num,buffer[num]);
    // memset(buffer, 0, sizeof(buffer));
    // buffer[inodeno]=1;
    //0000000 <<6 => 00000001

    DevWriteBlock(1, buffer);
    // if (inode6 ^ inodeno)
    // {

    //     DevWriteBlock(1, 32);
    // }
}

void SetBlockBitmap(int blkno)
{
    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(2, buffer);           //block읽어오기
    int block_num = blkno / 8;//byte
    int blkno_set = blkno % 8;//bit
   // printf("set start buffer[%d]=%d\n",block_num,buffer[block_num]);
    buffer[block_num] |= (1 << (7-blkno_set));
   

   // printf("set end buffer[%d]=%d\n",block_num,buffer[block_num]);
    // memset(buffer, 0, sizeof(buffer));
    // buffer[inodeno]=1;
    //0000000 <<6 => 00000001

    DevWriteBlock(2, buffer);
    // if (inode3 | blkno)
    // {

    //     DevWriteBlock(2, 32);
    // }
}

void ResetBlockBitmap(int blkno)
{
    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(2, buffer);           //block읽어오기
    int block_num = blkno / 8;//byte
    int blkno_set = blkno % 8;//bit
   // printf("reset start buffer[%d]=%d\n",block_num,buffer[block_num]);
    buffer[block_num] &=  ~ (buffer[block_num] << (7-blkno_set));
  
   // printf("reset end buffer[%d]=%d\n",block_num,buffer[block_num]);
    // buffer[inodeno]=1;
    //0000000 <<6 => 00000001

    DevWriteBlock(2, buffer);

}

void PutInode(int inodeno, Inode *pInode)
{
    char *buffer = malloc(BLOCK_SIZE);     //메모리 할당
    DevReadBlock(3 + inodeno /8, buffer); //block읽어오기
    //inodeno * 1;
    //복사한메모리저장
    memcpy( buffer + sizeof(Inode) * (inodeno % 8), pInode , sizeof(Inode));
    DevWriteBlock(3 + inodeno/8, buffer);
}

void GetInode(int inodeno, Inode *pInode)
{
    char *buffer = malloc(BLOCK_SIZE);     //메모리 할당
    DevReadBlock(3 + inodeno / 8, buffer); //block읽어오기,inodeno=0-3 이면 block3에 
   // inodeno * 1;
   //메모리영역복사 inodeno->pinode
    memcpy(pInode, buffer + sizeof(Inode) * (inodeno % 8), sizeof(Inode));
   // DevWriteBlock(3+inodeno/8, buffer);
    
}

int GetFreeInodeNum(void)
{
    int inode_num;
    int inode_check=-1;//첫번째 inode_num=0이 0인거 &operation으로 확인
    int inode_set=inode_num%8;//inode_num이 할당될 곳
    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(1, buffer);//inodebitmap=1에서 읽어오기

    for(inode_num=0; inode_num<BLOCK_SIZE; inode_num++)
    {//할당된 블록의 bitmap으로 들어감
        //buffer[inode_num]=buffer[inode_num]&(1<<7-inode_set);
        for(int inode_set=0; inode_set<8; inode_set++){//inode를 8씩 비교
            if( ((buffer[inode_num]<<7-inode_set) & inode_check) == 0 ) {//1과 &연산시 0이면 그대로 반환
            //buffer[inode_num]=buffer[inode_num]&(1<<7-inode_set);
            return inode_num*8+(7-inode_set);
            }
            // else{//아니라면 그 다음 bit로 넘어간다
            //     buffer[inode_num]=buffer[inode_num]&(1<<7-inode_set);
            // }
        }
        // buffer[inode_num]=buffer[inode_num]&(1<<8-(inode_set));
        // if((buffer[inode_set]&inode_check)==0){//&계산시 0이면 그 inode_num을 반환
        // //printf("0인 inode %d\n",buffer[inode_set]);
        //     buffer[inode_num];//뭔가이상...
        // }
        // else{
        //     buffer[inode_num]=buffer[inode_num]&(1<<8-(inode_set));
        // }
    }
    //printf("0인 inode %d\n",buffer[inode_set]);
    
    // for(int i=0;i<BLOCK_SIZE;i++)
    // {
    //     buffer[i]=buffer[i]&(1<<8-(inode_set));
    // }
    //printf("0을 가지는 inode %d",inode_num);
    return 0;
}

int GetFreeBlockNum(void)
{
    int blk;
    int block_num=blk/8;
    int block_check=-1;//첫번째 block_num=0이 0인거 &operation으로 확인
    int block_set=block_num%8;//block_num이 할당될 곳
    char *buffer = malloc(BLOCK_SIZE); //메모리 할당
    DevReadBlock(2, buffer);//blockmap에서 읽어오기

    
    for(block_num=0; block_num<BLOCK_SIZE; block_num++)
    {//할당된 블록의 bitmap으로 들어감
        for(int block_set=0; block_set<8; block_set++){//64개 inode를 8씩 비교
            if(((buffer[block_num] <<(7- block_set)) & block_check) == 0){//1과 &연산시 0이면 그대로 반환
                return block_num*8+(7-block_set);//
            }
            // else{//아니라면 그 다음 bit로 넘어간다
            //     buffer[block_num]=buffer[block_num]&(1<<7-block_set);
            // }
        }
    }
    
    
}