/*
 * ima_deal.c
 *
 *  Created on: 2017年4月17日
 *      Author: Administrator
 */
#include "img_deal.h"



//---------------------------赛道扫描--------------------
#define DELTA3     15       //两点之间的高度差3
#define DELTA2     15     //两点之间的高度差2
#define DELTA1     15    //两点之间的高度差1
#define D_value    60   //波形最高的高度差

//-------------------------位置判断，参照的中点位置----------------------
#define MIDPOINT       61
#define TRACKWIDTH     53  //测得赛道宽度，单位：像素点
#define LINEWIDTH      128

#define POINTAVGVALUE  200
//====================================================================
//函数名称：deal_image
//函数返回：无
//参数说明：无
//功能概要：找出近远两行的中心
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
	int count_place[IMG_WIDTH][4];//以白点总数为索引，存放此时点在整行中的位置


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
			k2+=Image[i+2*IMG_DIVIDE][j];                        //每段找一个代表行
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
//函数名称：get_image_aimmidpoint
//函数返回：无
//参数说明：无
//功能概要：找出近远两行的中心
//====================================================================
void get_image_midpoint(int16 returnBuff[6],int16* midpoint)
{
	static int16 last_midpoint;
	if(returnBuff[3]==IMG_WIDTH)//近处的线全黑
	{
		midpoint=last_midpoint;
	}
	else
	{
		if(returnBuff[4]==IMG_WIDTH)//中间线全黑
		{
			midpoint=returnBuff[0];
		}
		else if(returnBuff[4]==0)//中间线全白
		{

			midpoint=(int16)(0.7*returnBuff[0]+0.3*returnBuff[2]);
		}
		else
		{
			midpoint=(int16)(0.6*returnBuff[0]+0.4*returnBuff[1]);
		}

	}
	last_midpoint=midpoint;
}
//===========================================================================
//函数名称：deal_ccd_line
//函数返回：转向PWM值
//参数说明：point-ccd采集到的数据帧数组
//功能概要：赛道识别算法函数，根据图像信息，得出舵机的PWM相对于舵机打正需要调整的数值
//       具体步骤为：（1）寻找赛道边界
//                （2）对可能存在的图像元素进行判断，分析对应的赛道状况
//                （3）PWM值的确定
//===========================================================================
void deal_ccd_line(uint8 *point,int16 returnBuff[2])
{
	int16 midpoint;//返回的中点值
	//两次遍历，第一次遍历的左右边界，及遍历点数
	int16 leftborder=0;
	int16 rightborder=0;
	int16 leftbreak_num=0;
	int16 rightbreak_num=0;
	//第二次遍历，左右边界，及遍历点数
	int16 leftborder2;
	int16 rightborder2;
	int16 leftbreak_num2;
	int16 rightbreak_num2;
	//黑白双丢flag
	int16 flag=0;

	static int16 lastmidpoint=0;

	//
	int16 temp1,temp2;

    //循环使用的临时下标
	uint8 i=0;

    //1）先找凹槽点，左边的凹槽点和右边的凹槽点
    for(i=78;i>8;i--)          //从78点开始往左边寻线
    {
          if((point[i+4]-point[i]> D_value)||((point[i]+DELTA1<point[i+1])&&
        	(point[i+1]+DELTA2<point[i+2])&&(point[i+2]+DELTA3<point[i+3])))  //左边的凹槽点
          {
              leftborder=i;
              break;
          }
        leftbreak_num++;      //判断左边是否全白或全黑
    }

    for(i=49;i<119;i++)
    {
         if((point[i-4]-point[i]>D_value)||((point[i]+DELTA1<point[i-1])&&
        	(point[i-1]+DELTA2<point[i-2])&&(point[i-2]+DELTA3<point[i-3])))
          {
              rightborder=i;//右边的凹槽点
              break;
          }
        rightbreak_num++;      //判断右边是否全白
    }


    //2)左右两边均识别出赛道,
    if(leftbreak_num!=70&&rightbreak_num!=70)   //左右两边均识别出赛道
    {
			//____-------______
		  if((rightborder-leftborder)>0)
		   {
			  midpoint=(leftborder+rightborder)/2;
		   }
		  //____-----___-----_____
		  else
		  {
			  //再次遍历
				for(i=rightborder-1;i>8;i--)          //从85点开始往左边寻线
				{

					  if((point[i+4]-point[i]>D_value)||((point[i]+DELTA1<point[i+1])&&
						(point[i+1]+DELTA2<point[i+2])&&(point[i+2]+DELTA3<point[i+3])))  //左边的凹槽点
					  {
						  leftborder2=i;
						  break;
					  }
					leftbreak_num2++;      //判断左边是否全白
				}

				for(i=leftborder+1;i<119;i++)
				{
					 if((point[i-4]-point[i]>D_value)||((point[i]+DELTA1<point[i-1])&&
						(point[i-1]+DELTA2<point[i-2])&&(point[i-2]+DELTA3<point[i-3])))
					  {
						  rightborder2=i;//右边的凹槽点
						  break;
					  }
					rightbreak_num2++;      //判断右边是否全白
				}


			    //第二次遍历存在丢线，补一条边线
				if(leftbreak_num2==rightborder-9)
				  {
					leftborder2=0;
				  }
				if(rightbreak_num2==118-leftborder)
				{
					rightborder2=LINEWIDTH;
				}
				//_--__--__的左边和右边白道宽度
			   temp1=rightborder-leftborder2;
			   temp2=rightborder2-leftborder;
			   //仅左边丢线
			   if(leftbreak_num2==rightborder-9&&rightbreak_num2!=118-leftborder)
			   {
				   temp1=TRACKWIDTH-(rightborder2-rightborder);
			   }
			   //仅右边丢线
			   if(leftbreak_num2!=rightborder-9&&rightbreak_num2==118-leftborder)
			   {
			   	   temp2=TRACKWIDTH-(leftborder-leftborder2);
			   }
				//左边白道宽度大
				if(temp1>temp2)
				{
					midpoint=(rightborder+leftborder2)/2;
				}
				//右边白道宽度大
				else
				{
					midpoint=(rightborder2+leftborder)/2;
				}
			}
     }

    //3）第一次遍历存在丢线
  if((leftbreak_num==70)||(rightbreak_num==70))   //存在丢线的情况
    {
          if((leftbreak_num==70)&&(rightbreak_num!=70))          //左丢线
          {
        		 leftborder=rightborder-TRACKWIDTH;
          }

          if((leftbreak_num!=70)&&(rightbreak_num==70))           //右丢线
          {
        		 rightborder=leftborder+TRACKWIDTH;
          }
          midpoint=(leftborder+rightborder)/2;
          //全丢线，保持原先状态
          if((leftbreak_num==70)&&(rightbreak_num==70))                         //全白
			{
				  if(ccd_pix_ave(30, &point[48])>POINTAVGVALUE)
				  {
					  //白双丢
						midpoint=MIDPOINT;
						flag=1;
				  }
				  else
				  {
					  //黑双丢
					  midpoint=lastmidpoint;
					  flag=2;
				  }
        	 }
	}
  returnBuff[0]=midpoint;
  returnBuff[1]=flag;
}


//===========================================================================
//函数名称：ccd_pix_ave
//函数返回：返回平均值
//参数说明：len：数据长度
//       data：数据存储区
//功能概要：计算128个像素点的平均值
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

