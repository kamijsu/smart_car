/*
 * ima_deal.c
 *
 *  Created on: 2017��4��17��
 *      Author: Administrator
 */
#include "img_deal.h"



//---------------------------����ɨ��--------------------
#define DELTA3     15       //����֮��ĸ߶Ȳ�3
#define DELTA2     15     //����֮��ĸ߶Ȳ�2
#define DELTA1     15    //����֮��ĸ߶Ȳ�1
#define D_value    60   //������ߵĸ߶Ȳ�

//-------------------------λ���жϣ����յ��е�λ��----------------------
#define MIDPOINT       61
#define TRACKWIDTH     53  //���������ȣ���λ�����ص�
#define LINEWIDTH      128

#define POINTAVGVALUE  200
//====================================================================
//�������ƣ�deal_image
//�������أ���
//����˵������
//���ܸ�Ҫ���ҳ���Զ���е�����
//====================================================================
void deal_image(uint8 Image[IMG_HEIGHT][IMG_WIDTH],int16 returnBuff[6])
{
	int i,j,k0,k1,k2,k3;
	int16 mid_very_near;
	int16 mid_near;
	int16 mid_far;
	int16 count_very_near;
	int16 count_near;
	int16 count_far;
	uint8 near_line_count;
	int count_place[IMG_WIDTH][4];//�԰׵�����Ϊ��������Ŵ�ʱ���������е�λ��


	mid_very_near=0;
	mid_near=0;
	mid_far=0;
	count_very_near=0;
	count_near=0;
	count_far=0;
	near_line_count=0;
	for(i=0;i<3;i++)
	{
		k0=0;
		k1=0;
		k2=0;
		k3=0;
		for(j=0;j<IMG_WIDTH;j++)
		{
			Image[i][j]=(Image[i][j]>BINARY_VALUE)?1:0;
			Image[i+IMG_DIVIDE][j]=(Image[i+IMG_DIVIDE][j]>BINARY_VALUE)?1:0;
			Image[i+2*IMG_DIVIDE][j]=(Image[i+2*IMG_DIVIDE][j]>BINARY_VALUE)?1:0;
			Image[i+3*IMG_DIVIDE][j]=(Image[i+3*IMG_DIVIDE][j]>BINARY_VALUE)?1:0;
			count_place[k0][0]=j;
			count_place[k1][1]=j;
			count_place[k2][2]=j;
		    count_place[k3][3]=j;
			k0+=Image[i][j];
			k1+=Image[i+IMG_DIVIDE][j];
			k2+=Image[i+2*IMG_DIVIDE][j];                        //ÿ����һ��������
		    k3+=Image[i+3*IMG_DIVIDE][j];
		}
		mid_very_near+=count_place[k0/2][0];
		if(k2>3)
		{
			near_line_count++;
			mid_near+=count_place[k2/2][2];
		}
		mid_far+=count_place[k3/2][3];
		count_very_near+=k0;
		count_near+=k2;
		count_far+=k3;



		/*if(count_place[k0/2][0]-count_place[k0/6][0]>k0/3+10&&count_place[k0/6*5][0]-count_place[k0/2][0]>k0/3+10) stop=1;
		if(count_place[k1/2][1]-count_place[k1/6][1]>k1/3+10&&count_place[k1/6*5][1]-count_place[k1/2][1]>k1/3+10) stop=1;
		if(count_place[k2/2][2]-count_place[k2/6][2]>k2/3+10&&count_place[k2/6*5][2]-count_place[k2/2][2]>k2/3+10) stop=1;*/
	}
	mid_very_near/=3;
	mid_near/=near_line_count;
	mid_far/=3;
	mid_very_near=mid_very_near;
	mid_near=mid_near;
	mid_far=mid_far;
	count_very_near/=3;
	count_near/=3;
	count_far/=3;

	returnBuff[0]=mid_very_near;
	returnBuff[1]=mid_near;
	returnBuff[2]=mid_far;
	returnBuff[3]=count_very_near;
	returnBuff[4]=count_near;
	returnBuff[5]=count_far;
}
//====================================================================
//�������ƣ�get_image_aimmidpoint
//�������أ���
//����˵������
//���ܸ�Ҫ���ҳ���Զ���е�����
//====================================================================
void get_image_midpoint(int16 returnBuff[6],float* midpoint)
{
	static float last_midpoint = 40.0f;
	if(returnBuff[5]==0)//��������ȫ��
	{
		*midpoint=last_midpoint;
	}
	else
	{
		if(returnBuff[4]==0)//�м���ȫ��
		{
			*midpoint=returnBuff[2];
		}
		else if(returnBuff[4]==IMG_WIDTH)//�м���ȫ��
		{

			*midpoint=(int16)(0.7f*returnBuff[2]+0.3f*returnBuff[0]);
		}
		else
		{
			*midpoint=(int16)(0.6f*returnBuff[2]+0.4f*returnBuff[1]);
		}

	}
	last_midpoint=*midpoint;
}


static inline bool find_left_edge(uint8* img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], uint8 line){

}

//===========================================================================
//�������ƣ�deal_ccd_line
//�������أ�ת��PWMֵ
//����˵����point-ccd�ɼ���������֡����
//���ܸ�Ҫ������ʶ���㷨����������ͼ����Ϣ���ó������PWM����ڶ��������Ҫ��������ֵ
//       ���岽��Ϊ����1��Ѱ�������߽�
//                ��2���Կ��ܴ��ڵ�ͼ��Ԫ�ؽ����жϣ�������Ӧ������״��
//                ��3��PWMֵ��ȷ��
//===========================================================================
void deal_ccd_line(uint8 *point,int16 returnBuff[2])
{
	int16 midpoint;//���ص��е�ֵ
	//���α�������һ�α��������ұ߽磬����������
	int16 leftborder=0;
	int16 rightborder=0;
	int16 leftbreak_num=0;
	int16 rightbreak_num=0;
	//�ڶ��α��������ұ߽磬����������
	int16 leftborder2;
	int16 rightborder2;
	int16 leftbreak_num2;
	int16 rightbreak_num2;
	//�ڰ�˫��flag
	int16 flag=0;

	static int16 lastmidpoint=0;

	//
	int16 temp1,temp2;

    //ѭ��ʹ�õ���ʱ�±�
	uint8 i=0;

    //1�����Ұ��۵㣬��ߵİ��۵���ұߵİ��۵�
    for(i=78;i>8;i--)          //��78�㿪ʼ�����Ѱ��
    {
          if((point[i+4]-point[i]> D_value)||((point[i]+DELTA1<point[i+1])&&
        	(point[i+1]+DELTA2<point[i+2])&&(point[i+2]+DELTA3<point[i+3])))  //��ߵİ��۵�
          {
              leftborder=i;
              break;
          }
        leftbreak_num++;      //�ж�����Ƿ�ȫ�׻�ȫ��
    }

    for(i=49;i<119;i++)
    {
         if((point[i-4]-point[i]>D_value)||((point[i]+DELTA1<point[i-1])&&
        	(point[i-1]+DELTA2<point[i-2])&&(point[i-2]+DELTA3<point[i-3])))
          {
              rightborder=i;//�ұߵİ��۵�
              break;
          }
        rightbreak_num++;      //�ж��ұ��Ƿ�ȫ��
    }


    //2)�������߾�ʶ�������,
    if(leftbreak_num!=70&&rightbreak_num!=70)   //�������߾�ʶ�������
    {
			//____-------______
		  if((rightborder-leftborder)>0)
		   {
			  midpoint=(leftborder+rightborder)/2;
		   }
		  //____-----___-----_____
		  else
		  {
			  //�ٴα���
				for(i=rightborder-1;i>8;i--)          //��85�㿪ʼ�����Ѱ��
				{

					  if((point[i+4]-point[i]>D_value)||((point[i]+DELTA1<point[i+1])&&
						(point[i+1]+DELTA2<point[i+2])&&(point[i+2]+DELTA3<point[i+3])))  //��ߵİ��۵�
					  {
						  leftborder2=i;
						  break;
					  }
					leftbreak_num2++;      //�ж�����Ƿ�ȫ��
				}

				for(i=leftborder+1;i<119;i++)
				{
					 if((point[i-4]-point[i]>D_value)||((point[i]+DELTA1<point[i-1])&&
						(point[i-1]+DELTA2<point[i-2])&&(point[i-2]+DELTA3<point[i-3])))
					  {
						  rightborder2=i;//�ұߵİ��۵�
						  break;
					  }
					rightbreak_num2++;      //�ж��ұ��Ƿ�ȫ��
				}


			    //�ڶ��α������ڶ��ߣ���һ������
				if(leftbreak_num2==rightborder-9)
				  {
					leftborder2=0;
				  }
				if(rightbreak_num2==118-leftborder)
				{
					rightborder2=LINEWIDTH;
				}
				//_--__--__����ߺ��ұ߰׵����
			   temp1=rightborder-leftborder2;
			   temp2=rightborder2-leftborder;
			   //����߶���
			   if(leftbreak_num2==rightborder-9&&rightbreak_num2!=118-leftborder)
			   {
				   temp1=TRACKWIDTH-(rightborder2-rightborder);
			   }
			   //���ұ߶���
			   if(leftbreak_num2!=rightborder-9&&rightbreak_num2==118-leftborder)
			   {
			   	   temp2=TRACKWIDTH-(leftborder-leftborder2);
			   }
				//��߰׵���ȴ�
				if(temp1>temp2)
				{
					midpoint=(rightborder+leftborder2)/2;
				}
				//�ұ߰׵���ȴ�
				else
				{
					midpoint=(rightborder2+leftborder)/2;
				}
			}
     }

    //3����һ�α������ڶ���
  if((leftbreak_num==70)||(rightbreak_num==70))   //���ڶ��ߵ����
    {
          if((leftbreak_num==70)&&(rightbreak_num!=70))          //����
          {
        		 leftborder=rightborder-TRACKWIDTH;
          }

          if((leftbreak_num!=70)&&(rightbreak_num==70))           //�Ҷ���
          {
        		 rightborder=leftborder+TRACKWIDTH;
          }
          midpoint=(leftborder+rightborder)/2;
          //ȫ���ߣ�����ԭ��״̬
          if((leftbreak_num==70)&&(rightbreak_num==70))                         //ȫ��
			{
				  if(ccd_pix_ave(30, &point[48])>POINTAVGVALUE)
				  {
					  //��˫��
						midpoint=MIDPOINT;
						flag=1;
				  }
				  else
				  {
					  //��˫��
					  midpoint=lastmidpoint;
					  flag=2;
				  }
        	 }
	}
  returnBuff[0]=midpoint;
  returnBuff[1]=flag;
}


//===========================================================================
//�������ƣ�ccd_pix_ave
//�������أ�����ƽ��ֵ
//����˵����len�����ݳ���
//       data�����ݴ洢��
//���ܸ�Ҫ������128�����ص��ƽ��ֵ
//===========================================================================
uint8 ccd_pix_ave(uint8 len, uint8 * data)
{
    uint8 i;
    uint32 sum;
    sum = 0;
    for(i = 0; i<len; i++)
    {
        sum = sum + *data++;
    }
    return ((uint8)(sum/len));
}

