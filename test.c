/*******************************************************************************
����Ա       : enjoy5512
����޸�ʱ�� : 2016��3��22�� 20:56:41
����˵��     : ������,���ڶ�ȡ��������Ĵ��������������ŵ�512�ֽ�����,�����Ե�
               �����н���
���Ի���     : Windows XP SP2 + VC6.0
���������   : ���̵ڶ�����������Ϣ��ȡ����
               ��Ϊ���Ե�ԭ��,����ֻ�������볬��������ŲŻ��˳�
*******************************************************************************/

#include <windows.h>  
#include <stdio.h>
#include <stdlib.h>


/*
����˵��:
	����������ڰ���ָ���ĸ�ʽ���һ��������512�ֽڵ���Ϣ,�ַ�ascll�����32���ߴ���127
	�Ķ���'.'������,��������ָ�����ַ�
�������:
	char buff[] : �ַ�������
�������:
	0
  */
int ShowBuff(char buff[])
{
	int i = 0;
	int j = 0;

	system("cls");                            //����
	system("mode con cols=80 lines=37");      //�趨�����д��ڴ�С

	printf("********************************************************************************");
	printf("********************************************************************************");
	printf("\t     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  |\n");
	for(i = 0;i < 32;i++)                     //ÿ�����16���ֽ�,���������Ӧ���ַ�
	{
		printf("%#011X:",i*16);
		for(j = 0;j < 16;j++)
		{
			//ÿ��λ�������,����ᱻ������λ�з����������,���޽���취
			printf(" %X%X",(buff[j+i*16]&240)>>4,buff[j+i*16]&15);   	                                         
		}
		printf(" | ");
		for(j = 0;j < 16;j++)
		{
			if(buff[j+i*16] > 31 && buff[j+i*16] < 127)
			{
				//�����Ӧ���ַ�,����Ӧascll�����32���ߴ���127�Ķ���'.'������
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
����˵��:
	����������ڰ��ո����������ŷ�����Ӧ����512�ֽ���Ϣ
�������:
	HANDLE hDisk : ���̾��
	char buff[]  : �ַ�������
	int n        : ������
	DWORD* DCount: ��ȡ���ֽ���
�������:
	0
  */
int GetBuff(HANDLE hDisk,char buff[] , int n,DWORD* DCount)
{
	LARGE_INTEGER li={0}; //�ǵó�ʼ��
	int h_sector = 0;     //32λϵͳ,���ڵ�����32λѰַ��Χʱ����ߵ�ַ
	int l_sector = 0;     //32λϵͳ,���ڵ�����32λѰַ��Χʱ����͵�ַ

	l_sector = n%8388607; //ȡ�͵�ַ����
	h_sector = n/8388607; //ȡ�ߵ�ַ����

	li.LowPart = l_sector * 512; //��32λϵͳintȡ��λ,FFFFFFFF/200H = 8388607
	li.HighPart = h_sector;      //��λֱ�Ӵ���ȥ


	//�������Ѱַ��Χ���˳�,����ָ���ƶ�����Ӧ����
	if(-1 == SetFilePointer(hDisk,li.LowPart,&li.HighPart,FILE_BEGIN))
	{
		MessageBox(NULL,"Error sector!The program will exit!!","Error",MB_OK);
		exit(-1);
	}

	//��ȡ512�ֽ����ݵ�buff
	ReadFile(hDisk,  //Ҫ�����ļ���� 
	buff,            //��Ŷ�������
	512,                 //Ҫ�����ֽ���
	DCount,        //ʵ�ʶ�ȡ�ֽ���
	NULL);

	return 0;
}


/*
����˵��:
	����������ڰ��ո����Ĵ��̺�,������Ӧ�ľ��
�������:
	HANDLE hDisk : ���̾��
	int nDisk    : ���̺ź�
�������:
	0
  */
int GetHandle(HANDLE* hDisk,int nDisk)
{
	char szDriverBuffer[128];     //������Ӧ����·����Ϣ

	memset(szDriverBuffer,0,128); //��ʽ���豸�ļ�����
	sprintf(szDriverBuffer,"\\\\.\\PhysicalDrive%d",nDisk);//��ȡ�豸����

	*hDisk = CreateFileA(
	szDriverBuffer,               // �豸����,����ָ��һ��Ӳ�̣����Ӳ�̵��Լ��޸ľͺ���
	GENERIC_READ,                 // ָ�������ʷ�ʽ
	FILE_SHARE_READ | FILE_SHARE_WRITE,  // ����ģʽΪ�� | д,ֻ���Ļ������ǳ���..
	NULL,                         // NULL��ʾ�þ�����ܱ��ӳ���̳�
	OPEN_EXISTING,                // ���Ѿ����ڵ��ļ����ļ���������������ʧ��
	NULL,                         // ָ���ļ�����
	NULL); 

	if (*hDisk==INVALID_HANDLE_VALUE)    //�����ȡ���ʧ��,���˳�
	{
		MessageBox(NULL,"Error open disk!","Error",MB_OK);
		exit(-1);
	}

	return 0;
}


/*
����˵��:
	������
�������:
	0
�������:
	0
  */
int main(void)
{
	char buff[513] = {0};  //����512�ֽ���Ϣ,��Ϊ�ַ������������\0,��������513�ֽ�
	HANDLE hDisk;          //���̾��
	DWORD DCount=0;        //���ض�ȡ���ֽ���
	int sector = 0;        //������
	int nDisk = 0;         //���̺�

	printf("��������Ҫ��ȡ�Ĵ��̺�:");   //��ȡ���̺�
	scanf("%d",&nDisk);

	ShowBuff(buff);                      //��ʼ������
	GetHandle(&hDisk,nDisk);             //��ȡ���̾��
	
	for(;;)                              //ѭ����ʾ������Ϣ
	{
		printf("please input the number of sector : ");  //��ȡ������
		scanf("%d",&sector);

		GetBuff(hDisk,buff,sector,&DCount);              //��ȡ������Ϣ
		ShowBuff(buff);                                  //��ʾ������Ϣ
	}

	CloseHandle(hDisk);                  //�ͷž��
	return 0;
}