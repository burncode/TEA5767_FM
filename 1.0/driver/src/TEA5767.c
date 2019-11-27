#include "TEA5767.h"
#include "stm32f10x.h"
#include "delay.h"

#define max_freq 108000
#define min_freq 87500

unsigned char radio_data[5]={0x29,0xc2,0x20,0x11,0x00};

unsigned char read_data[5];        //TEA5767������״̬
unsigned long frequency;
unsigned int pll;

u8 key_down=0;

/**********************************************************
** ������:TEA5767_Write
** ��������:��TEA5767д��5���ֽ�����
** �������:��
** �������:��
** ��    ��:��
***********************************************************/
void TEA5767_Write(void)
{
    unsigned char i;
    IIC_Start(); //������ʼ�ź�
    IIC_Send_Byte(0xc0);        //TEA5767д��ַ
    IIC_Wait_Ack();	  //�ȴ�Ӧ��
    for(i=0;i<5;i++)
    {
        IIC_Send_Byte(radio_data[i]);//����д��5���ֽ�����
        IIC_Ack(); //����Ӧ��
    }
    IIC_Stop(); //����ֹͣ�ź�   
}

/**********************************************************
** ������:Get_PLL
** ��������:��Ƶ��ֵ����PLL
** �������:��
** �������:pll--�õ�PLLֵ
** ��    ��:��
***********************************************************/
void Get_PLL(void)
{
    unsigned char hlsi;
    hlsi=radio_data[2]&0x10;  //HLSIλ
    if (hlsi)
        pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
    else
        pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
}


/**********************************************************
** ������:Get_Frequency
** ��������:��PLL����Ƶ��
** �������:��
** �������:frequency--�õ�Ƶ��ֵ
** ��    ��:��
***********************************************************/
void Get_Frequency(void)
{
    unsigned char hlsi;
    unsigned int npll=0;
    npll=pll;
    hlsi=radio_data[2]&0x10;
    if(hlsi)
        frequency=(unsigned long)((float)(npll)*(float)8.192-225);    //Ƶ�ʵ�λ:KHz
    else
        frequency=(unsigned long)((float)(npll)*(float)8.192+225);    //Ƶ�ʵ�λ:KHz
}

/**********************************************************
** ������:Get_Frequency
** ��������:��PLL����Ƶ��
** �������:��
** �������:frequency--�õ�Ƶ��ֵ
** ��    ��:��
***********************************************************/
void TEA5767_Read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    IIC_Start();
    IIC_Send_Byte(0xc1);        //TEA5767����ַ
    IIC_Wait_Ack();
    for(i=0;i<5;i++)   //��ȡ5���ֽ�����
    {
        read_data[i]=IIC_Read_Byte(1);//��ȡ���ݺ󣬷���Ӧ��
    }
    IIC_Stop();
    temp_l=read_data[1];//�õ�PLL��8λ 
    temp_h=read_data[0];//�õ�PLL��6λ
    temp_h&=0x3f;
    pll=temp_h*256+temp_l; //PLLֵ
    Get_Frequency();//ת��ΪƵ��ֵ
}
/**********************************************************
** ������:Set_Frequency
** ��������:���õ�̨Ƶ��
** �������:fre--д��ĵ�̨Ƶ��ֵ��kHz��
** �������:��
** ��    ��:��
***********************************************************/
void Set_Frequency(u32 fre)
{
	frequency=fre;
	Get_PLL();
    radio_data[0]=pll/256;
    radio_data[1]=pll%256;
    radio_data[2]=0x20;
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();
}
/**********************************************************
** ������:Search
** ��������:�ֶ�������̨�����ÿ���TEA5767������̨�����λ:SM,SUD
** �������:mode=1������������Ƶ��ֵ+0.1MHz
			mode=0������������Ƶ��ֵ-0.1MHz
** �������:��
** ��    ��:��
***********************************************************/
void Search(char mode)
{
    TEA5767_Read(); //��ȡ��ǰƵ��ֵ       
    if(mode) //��������
    {
        frequency+=10;
        if(frequency>max_freq)//Ƶ�ʴﵽ���ֵ
            frequency=min_freq;
    }
    else   //��������
    {
        frequency-=10;
        if(frequency<min_freq)//Ƶ�ʴﵽ��Сֵ
            frequency=max_freq;
    }          
    Get_PLL();//����PLLֵ
    radio_data[0]=pll/256; //����д��5���ֽ�����
    radio_data[1]=pll%256;
    radio_data[2]=0x20;
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();
	TEA5767_Read();//��ȡƵ��ֵ
	//while(GPIOA->IDR&GPIO_Pin_0); //�ȴ�PA0�����ͷ�
}

/**********************************************************
** ������:Auto_Search
** ��������:�Զ�������̨
** �������:mode=1��Ƶ����������
			mode=0��Ƶ�ʼ�С����
** �������:��
** ��    ��:��
***********************************************************/
void Auto_Search(char mode)
{
	TEA5767_Read();//��ȡƵ��
    Get_PLL();	   //ת��ΪPLLֵ
    if(mode)//Ƶ��������̨
        radio_data[2]=0xa0;
    else	//Ƶ�ʼ�С��̨
        radio_data[2]=0x20;  
	  
    radio_data[0]=pll/256+0x40;
    radio_data[1]=pll%256;    
    radio_data[3]=0x11;
    radio_data[4]=0x00;
    TEA5767_Write();  //д��5���ֽ�����
    TEA5767_Read();//��ȡƵ��ֵ
	//printf(" |%d\n",frequency);	//�������Ƶ��ֵ 
    if((read_data[0]&0x80))     //��̨�ɹ����ɱ�����Ƶ��ֵ����
    {
        TEA5767_Read();
	//	printf(" frequency=%d\n",frequency);
		// while(GPIOA->IDR&GPIO_Pin_0); //�ȴ�PA0�����ͷ�
    }    
}

 