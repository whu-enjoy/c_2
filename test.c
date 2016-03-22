/*******************************************************************************
程序员       : enjoy5512
最后修改时间 : 2016年3月22日 20:56:41
程序说明     : 本程序,用于读取给定输入的磁盘扇区的扇区号的512字节内容,并回显到
               命令行界面
测试环境     : Windows XP SP2 + VC6.0
待解决问题   : 磁盘第二个扇区的信息获取不到
               因为测试的原因,程序只有在输入超大的扇区号才会退出
*******************************************************************************/

#include <windows.h>  
#include <stdio.h>
#include <stdlib.h>


/*
函数说明:
	这个函数用于按照指定的格式输出一个扇区的512字节的信息,字符ascll码低于32或者大于127
	的都用'.'来代替,其余的输出指定的字符
输入参数:
	char buff[] : 字符串数组
输出参数:
	0
  */
int ShowBuff(char buff[])
{
	int i = 0;
	int j = 0;

	system("cls");                            //清屏
	system("mode con cols=80 lines=37");      //设定命令行窗口大小

	printf("********************************************************************************");
	printf("********************************************************************************");
	printf("\t     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  |\n");
	for(i = 0;i < 32;i++)                     //每行输出16个字节,后面跟着相应的字符
	{
		printf("%#011X:",i*16);
		for(j = 0;j < 16;j++)
		{
			//每四位单独输出,否则会被当作八位有符号整数输出,暂无解决办法
			printf(" %X%X",(buff[j+i*16]&240)>>4,buff[j+i*16]&15);   	                                         
		}
		printf(" | ");
		for(j = 0;j < 16;j++)
		{
			if(buff[j+i*16] > 31 && buff[j+i*16] < 127)
			{
				//输出相应的字符,若对应ascll码低于32或者大于127的都用'.'来代替
				printf("%c",buff[j+i*16]);     
			}
			else
			{
				printf(".");
			}
		}
		printf("\n");
	}

	return 0;
}

/*
函数说明:
	这个函数用于按照给定的扇区号返回相应扇区512字节信息
输入参数:
	HANDLE hDisk : 磁盘句柄
	char buff[]  : 字符串数组
	int n        : 扇区号
	DWORD* DCount: 读取的字节数
输出参数:
	0
  */
int GetBuff(HANDLE hDisk,char buff[] , int n,DWORD* DCount)
{
	LARGE_INTEGER li={0}; //记得初始化
	int h_sector = 0;     //32位系统,用于当超出32位寻址范围时保存高地址
	int l_sector = 0;     //32位系统,用于当超出32位寻址范围时保存低地址

	l_sector = n%8388607; //取低地址部分
	h_sector = n/8388607; //取高地址部分

	li.LowPart = l_sector * 512; //我32位系统int取四位,FFFFFFFF/200H = 8388607
	li.HighPart = h_sector;      //高位直接传过去


	//如果超出寻址范围则退出,否则将指针移动到相应扇区
	if(-1 == SetFilePointer(hDisk,li.LowPart,&li.HighPart,FILE_BEGIN))
	{
		MessageBox(NULL,"Error sector!The program will exit!!","Error",MB_OK);
		exit(-1);
	}

	//读取512字节内容到buff
	ReadFile(hDisk,  //要读的文件句柄 
	buff,            //存放读出内容
	512,                 //要读的字节数
	DCount,        //实际读取字节数
	NULL);

	return 0;
}


/*
函数说明:
	这个函数用于按照给定的磁盘号,返回相应的句柄
输入参数:
	HANDLE hDisk : 磁盘句柄
	int nDisk    : 磁盘号号
输出参数:
	0
  */
int GetHandle(HANDLE* hDisk,int nDisk)
{
	char szDriverBuffer[128];     //保存相应磁盘路径信息

	memset(szDriverBuffer,0,128); //格式化设备文件名称
	sprintf(szDriverBuffer,"\\\\.\\PhysicalDrive%d",nDisk);//获取设备名称

	*hDisk = CreateFileA(
	szDriverBuffer,               // 设备名称,这里指第一块硬盘，多个硬盘的自己修改就好了
	GENERIC_READ,                 // 指定读访问方式
	FILE_SHARE_READ | FILE_SHARE_WRITE,  // 共享模式为读 | 写,只读的话我总是出错..
	NULL,                         // NULL表示该句柄不能被子程序继承
	OPEN_EXISTING,                // 打开已经存在的文件，文件不存在则函数调用失败
	NULL,                         // 指定文件属性
	NULL); 

	if (*hDisk==INVALID_HANDLE_VALUE)    //如果获取句柄失败,则退出
	{
		MessageBox(NULL,"Error open disk!","Error",MB_OK);
		exit(-1);
	}

	return 0;
}


/*
函数说明:
	主函数
输入参数:
	0
输出参数:
	0
  */
int main(void)
{
	char buff[513] = {0};  //保存512字节信息,因为字符串数组后面会加\0,所以申请513字节
	HANDLE hDisk;          //磁盘句柄
	DWORD DCount=0;        //返回读取的字节数
	int sector = 0;        //扇区号
	int nDisk = 0;         //磁盘号

	printf("请输入需要读取的磁盘号:");   //获取磁盘号
	scanf("%d",&nDisk);

	ShowBuff(buff);                      //初始化界面
	GetHandle(&hDisk,nDisk);             //获取磁盘句柄
	
	for(;;)                              //循环显示扇区信息
	{
		printf("please input the number of sector : ");  //获取扇区号
		scanf("%d",&sector);

		GetBuff(hDisk,buff,sector,&DCount);              //获取扇区信息
		ShowBuff(buff);                                  //显示扇区信息
	}

	CloseHandle(hDisk);                  //释放句柄
	return 0;
}