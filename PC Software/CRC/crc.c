#include<stdio.h>
#include <stdlib.h>
#include<math.h>
/**************************************************************************
SPD文件的第一个字节的最高位指示了CRC计算的范围，如果是1，计算0-116；如果是0，计算0-125；
校验码计算算法在SPD文档里提供，SPD文件的126（第127）字节和127（第128）字节记录了原始的校验码
分别为校验码的低字节和高字节，计算结果和这两个字节相符即为校验PASS
***************************************************************************/
int crc16(unsigned char *ptr,int count)
{
    int crc=0,i;
    while(--count>=0)
    {
        crc=crc^(int)*ptr++<<8;
        for(i=0;i<8;i++)
            if(crc&0x8000)
                crc=crc<<1^0x1021;
            else
                crc=crc<<1;
    }
    return (crc&0xffff);
}
void crc_show(unsigned char *ptr)
{
    int data16=crc16(ptr,(ptr[0]&0x80?117:126));
    printf("CRC=0x%x\n",data16);
    if((ptr[126]==(data16&0xff))&&(ptr[127]==(data16>>8)))
        printf("CRC OK!\n");
    else
        printf("CRC Fail!\n");
}
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
    crc_show(spd_buf);
    fclose(spd);
}