#include<stdio.h>
#include <stdlib.h>
#include<math.h>
void main()
{
    FILE *spd;
    char file_name[20];
    unsigned int SD_RAM_CAP,PRIMARY_WIDTH,SDRAM_WIDTH,RANK,MEM_CAP;//单片芯片容量，内存条位宽，芯片位宽，RANK数量，内存总容量
    unsigned char *spd_buf=(unsigned char*)malloc(256);
    printf("Plesae enter spd file name:");
    scanf("%s",&file_name);
    errno_t error;
    error=fopen_s(&spd,(const char*)&file_name,"r");
    if(error!=0)
    {
        printf("file not exist!\n");
        exit(0);
    }
    fread(spd_buf,256,1,spd);
    SD_RAM_CAP=(unsigned int)pow(2,spd_buf[4]&0x07)*256;//Mb
    printf("SD_RAM_CAP=%d Mbit\n",SD_RAM_CAP);
    PRIMARY_WIDTH=(unsigned int)pow(2,spd_buf[8]&0x07)*8;
    printf("PRIMARY_WIDTH=%d bit\n",PRIMARY_WIDTH);
    SDRAM_WIDTH=(unsigned int)pow(2,spd_buf[7]&0x07)*4;
    printf("SDRAM_WIDTH=%d bit\n",SDRAM_WIDTH);
    RANK=((spd_buf[7]>>3)&0x07)+1;
    printf("RANK=%d\n",RANK);
    MEM_CAP=(unsigned int)((SD_RAM_CAP/8)*(PRIMARY_WIDTH/SDRAM_WIDTH)*RANK)/1024;
    printf("Chip number:%d\n",PRIMARY_WIDTH/SDRAM_WIDTH*RANK);
    printf("MEMSIZE=%dGB\n",MEM_CAP);
    fclose(spd);
}