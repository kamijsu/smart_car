//======================================================================
//文件名称：uart.c
//功能概要：uart底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2015-7-14 V1.0
//======================================================================
#include "uart.h"

//=====串口1、2号地址映射====
static const UART_MemMapPtr UART_ARR[] = {UART1_BASE_PTR, UART2_BASE_PTR};
//====定义串口IRQ号对应表====
static IRQn_Type table_irq_uart[3] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};


//======================================================================
//函数名称：uart_init
//功能概要：初始化uart模块                                                
//参数说明：uartNo:串口号：UART_0、UART_1、UART_2 
//          baud:波特率：300、600、1200、2400、4800、9600、19200、115200...
//函数返回：无
//======================================================================
 void  uart_init (uint_8 uartNo, uint_32 baud_rate)
 {      
    //局部变量声明
    uint_16 sbr;
    uint_8 temp;  
    UART0_MemMapPtr uartch1=UART0_BASE_PTR;//声明uartch1为UARTLP_MemMapPtr结构体类型指针
    UART_MemMapPtr uartch2;     //声明uartch2为UART_MemMapPtr结构体类型指针 
    
    //根据带入参数uartNo，给局部变量uartch1赋值
    
    if(uartNo==0)
    {

        SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);   //UART0选择MCGFLLCLK_khz=48000Khz时钟源
        SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
        
#ifdef UART_0_GROUP
        //依据选择配置对应引脚为UART0
        switch(UART_0_GROUP)
        {
        case 0:
            PORTE_PCR20 |= PORT_PCR_MUX(0x4);    //使能UART0_TXD
            PORTE_PCR21 |= PORT_PCR_MUX(0x4);    //使能UART0_RXD
            break;
        case 1:
            PORTA_PCR2 |= PORT_PCR_MUX(0x2);     //使能UART0_TXD
            PORTA_PCR1 |= PORT_PCR_MUX(0x2);     //使能UART0_RXD
            break;
        case 2:
            PORTA_PCR14 |= PORT_PCR_MUX(0x3);    //使能UART0_TXD
            PORTA_PCR15 |= PORT_PCR_MUX(0x3);    //使能UART0_RXD
            break;
        case 3:
            PORTB_PCR17 |= PORT_PCR_MUX(0x3);    //使能UART0_TXD
            PORTB_PCR16 |= PORT_PCR_MUX(0x3);    //使能UART0_RXD
            break;
        case 4:
        default:
            PORTD_PCR7 |= PORT_PCR_MUX(0x3);     //使能UART0_TXD
            PORTD_PCR6 |= PORT_PCR_MUX(0x3);     //使能UART0_RXD
            break;
        }
#endif
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;    //启动串口0时钟
        
        //暂时关闭串口0发送与接收功能  
        uartch1->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK); 
   
        //配置串口工作模式:8位无校验模式
        sbr = (uint_16)((SYSTEM_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART_BDH_REG(uartch1) & ~(UART_BDH_SBR(0x1F));
        UART_BDH_REG(uartch1) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART_BDL_REG(uartch1) = (uint_8)(sbr & UART_BDL_SBR_MASK);

        //初始化控制寄存器、清标志位
        UART0_C4 = 0x0F;
        UART0_C1 = 0x00;
        UART0_C3 = 0x00;
        UART0_MA1 = 0x00;
        UART0_MA2 = 0x00;
        UART0_S1 |= 0x1F;
        UART0_S2 |= 0xC0;    
           
        //启动发送接收
        uartch1->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);    
    }
    else
    {
         switch (uartNo)
               {
               case UART_1:
#ifdef UART_1_GROUP
                   switch(UART_1_GROUP)
                   {
                   case 0:
                       PORTE_PCR0 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                       PORTE_PCR1 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                       break;
                   case 1:
                       PORTA_PCR14 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                       PORTA_PCR15 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                       break;
                   case 2:
                   default:
                       PORTC_PCR4 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                       PORTC_PCR5 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                       break;
                   }
#endif
                   SIM_SCGC4 |= SIM_SCGC4_UART1_MASK; //启动串口1时钟
                   break;
               case UART_2:
#ifdef UART_2_GROUP
                   switch(UART_2_GROUP)
                   {
                   case 0:
                       PORTE_PCR22 = PORT_PCR_MUX(0x4); //使能UART2_TXD
                       PORTE_PCR23 = PORT_PCR_MUX(0x4); //使能UART2_RXD
                       break;
                   case 1:
                       PORTD_PCR3 = PORT_PCR_MUX(0x3); //使能UART2_TXD
                       PORTD_PCR2 = PORT_PCR_MUX(0x3); //使能UART2_RXD
                       break;
                   case 2:
                   default:
                       PORTD_PCR5 = PORT_PCR_MUX(0x3); //使能UART2_TXD
                       PORTD_PCR6 = PORT_PCR_MUX(0x3); //使能UART2_RXD
                       break;
                   }
#endif
                   SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;//启动串口2时钟
                   break;
               default:
                   break;  //传参错误，返回
               }

        uartch2 = UART_ARR[uartNo-1]; //获得UART1、2模块相应口基地址

         //暂时关闭串口1、2发送与接收功能  
        uartch2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
                 
        //配置波特率,KL25串口1、2时钟频率使用 Bus clock = 24M 总线时钟
        //配置串口工作模式,8位无校验模式
        uartch2->C1 = 0;          
        sbr = (uint_16)((BUS_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART_BDH_REG(uartch2) & ~(UART_BDH_SBR(0x1F));
        UART_BDH_REG(uartch2) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART_BDL_REG(uartch2) = (uint_8)(sbr & UART_BDL_SBR_MASK);
                   
        //初始化控制寄存器、清标志位
        uartch2->C1 = 0x00;
        uartch2->C3 = 0x00;
//        uartch2->S1 = 0x1F;
        uartch2->S2 = 0x00;                      
        //启动发送接收
        uartch2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);                               
    }
 }
 //======================================================================
//函数名称：uart_send1
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//          ch:要发送的字节
//函数返回：函数执行状态：0=正常；非0=异常。
//功能概要：串行发送1个字节
 //======================================================================
uint_8 uart_send1(uint_8 uartNo, uint_8 ch)
{
    uint_32 t;       
    UART0_MemMapPtr uartch1=UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址
    
    for (t = 0; t < 0xFBBB; t++)//查询指定次数
    {
        
        if(0==uartNo)     //判断使用的哪个串口
        {
            if ( (uartch1->S1) & UART_S1_TDRE_MASK )//判断发送缓冲区是否为空
            {
                uartch1->D = ch;   //获取数据并发送
                break;             //跳出循环
            }
        }
        else
        {
            if ( (uartch2->S1) & UART_S1_TDRE_MASK )//判断发送缓冲区是否为空
            {
                uartch2->D = ch;   //获取数据并发送
                break;             //跳出循环
            }
        }
        
    }//end for
    if (t >= 0xFBBB)
        return 1; //发送超时，发送失败
    else
        return 0; //成功发送
        
}

//======================================================================
//函数名称：uart_sendN
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//          buff: 发送缓冲区
//          len:发送长度
//函数返回： 函数执行状态：0=正常；1=异常
//功能概要：串行 接收n个字节   
//======================================================================
uint_8 uart_sendN (uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;
    for (i = 0; i < len; i++)
    {
        if (uart_send1(uartNo, buff[i])) //发送一个字节数据，失败则跳出循环
        {
            break;        
        }
    }
    if (i < len)  
        return 1;   //发送len个字节出错
    else 
        return 0;   //发送len个字节成功
}

//======================================================================
//函数名称：uart_send_string
//参数说明：uartNo:UART模块号:UART_0、UART_1、UART_2 
//          buff:要发送的字符串的首地址
//函数返回： 函数执行状态：0=正常；非0=异常。
//功能概要：从指定UART端口发送一个以'\0'结束的字符串
//======================================================================
uint_8 uart_send_string(uint_8 uartNo, void *buff)
{
    uint_16 i = 0;
    uint_8 *buff_ptr = (uint_8 *)buff;    //定义指针指向要发送字符串首地址
    for(i = 0; buff_ptr[i] != '\0'; i++)  //遍历字符串里的字符
    {
        if (uart_send1(uartNo,buff_ptr[i]))//发送指针对应的字符
            return 1;  //发送失败
    }
    return 0;         //发送成功
}

//======================================================================
//函数名称：uart_re1
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//          fp:接收成功标志的指针:*fp=0，成功接收；*fp=1，接收失败
//函数返回：接收返回字节
//功能概要：串行接收1个字节
//======================================================================
uint_8 uart_re1 (uint_8 uartNo,uint_8 *fp)
{
    uint_32 t;
    uint_8  dat;
    UART0_MemMapPtr uartch1=UART0_BASE_PTR;    //获取UART0基地址
    UART_MemMapPtr uartch2 = UART_ARR[uartNo-1];//获取UART1或者2基地址

    for (t = 0; t < 0xFBBB; t++)//查询指定次数
    {
        if(0==uartNo)  //判断使用的哪个串口
        {
            //判断接收缓冲区是否满
            if ((uartch1->S1) & UART_S1_RDRF_MASK )
            {
                dat = uartch1->D; //获取数据 
                *fp= 0;  //接受成功
                break;
            }
        }
        else
        {
            //判断接收缓冲区是否满
            if ((uartch2->S1) & UART_S1_RDRF_MASK )
            {
                dat = uartch2->D; //获取数据 
                *fp= 0;  //接受成功
                break;
            }
        }

    }//end for
    if(t >= 0xFBBB) 
    {
        dat = 0xFF; 
        *fp = 1;    //未收到数据
    }
    return dat;    //返回接收到的数据
    
}

//======================================================================
//函数名称：uart_reN
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2                                                   
//          buff: 接收缓冲区
//          len:接收长度
//函数返回：函数执行状态 0=正常;非0=异常
//功能概要：串行 接收n个字节
//======================================================================
uint_8 uart_reN (uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;
    uint_8 flag = 0;
    
    //判断是否能接受数据
    for (i = 0; i < len && 0 == flag; i++)
    {
        buff[i] = uart_re1(uartNo, &flag); //接受数据
    }
    if (i < len)
        return 1; //接收失败
    else
        return 0; //接收成功
}

//======================================================================
//函数名称：uart_enable_re_int
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//函数返回：无
//功能概要：开串口接收中断
//======================================================================
void uart_enable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch1=UART0_BASE_PTR;
    UART_MemMapPtr uartch2 = UART_ARR[uartNo-1];
    if(0 == uartNo)
      uartch1->C2 |= UART_C2_RIE_MASK;        //开放UART接收中断
    else 
      uartch2->C2 |= UART_C2_RIE_MASK;        //开放UART接收中断
    NVIC_EnableIRQ(table_irq_uart[uartNo]);   //开中断控制器IRQ中断
}

//======================================================================
//函数名称：uart_disable_re_int
//参数说明：uartNo: 串口号 :UART_0、UART_1、UART_2 
//函数返回：无
//功能概要：关串口接收中断
//======================================================================
void uart_disable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch1=UART0_BASE_PTR;
    UART_MemMapPtr uartch2 = UART_ARR[uartNo-1];
    if(0==uartNo) 
      uartch1->C2 &= ~UART_C2_RIE_MASK;               //禁止UART接收中断
    else 
      uartch2->C2 &= ~UART_C2_RIE_MASK;               //禁止UART接收中断    
    NVIC_DisableIRQ(table_irq_uart[uartNo]);          //禁止中断控制器IRQ中断
}

 
 
 
 
 
 
 
 
 
