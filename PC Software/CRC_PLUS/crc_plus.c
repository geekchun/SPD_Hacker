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
//增加自动更正功能
void crc_show(unsigned char *ptr,FILE *spd)//第一个参数是读出的文件内容缓冲区的指针，第二个是文件指针
{
    int data16=crc16(ptr,(ptr[0]&0x80?117:126));//根据首字节的最高位判断计算范围
    printf("CRC=0x%x\n",data16);//打印计算加结果
    if((ptr[126]==(data16&0xff))&&(ptr[127]==(data16>>8)))//将计算结果和文件中的126 127字节进行比较，判断是否校验通过
        printf("CRC OK!\n");
    else
    {
         printf("CRC Fail!Need auto correction? y=yes,others=no\n");//询问是否自动更正，输入y以更正
         char a;
         scanf(" %c",&a);//%c前面添加空格，防止scnaf吃掉回车符
         if(a=='y')
         {
             //执行自动更正，将SPD文件对应的字节更正为正确的计算结果
            printf("gatcha\n");
            ptr[126]=data16&0xff;
            ptr[127]=data16>>8;
            fseek(spd,126,SEEK_SET);//将文件读写指针移动到126字节的位置
            fputc(ptr[126],spd);//写入完成后，指针自动加1
            fputc(ptr[127],spd);
         }
    }
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
    error=fopen_s(&spd,(const char*)&file_name,"r+");//文件打开方式为读写
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
    MEM_CAP=(unsigned int)((SD_RAM_CAP/8)*(PRIMARY_WIDTH/SDRAM_WIDTH)*RANK)/1024;//根据文档中的公式自动计算容量
    printf("Chip number:%d\n",PRIMARY_WIDTH/SDRAM_WIDTH*RANK);
    printf("MEMSIZE=%dGB\n",MEM_CAP);
    crc_show(spd_buf,spd);//传入文件指针，以便更正校验码
    fclose(spd);
}