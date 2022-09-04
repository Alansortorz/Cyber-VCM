/**
  ******************************************************************************
  * @file oled.c
  * @version V1.0
  * @brief 本程序为参考中景园电子历程改写而成的四针OLED驱动程序
  * @time 2020.1.7
  ******************************************************************************
  */
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"

/** @brief 屏幕尺寸 128x80
  * @note 竖屏情况下二维容器为96x16(16表示横向，96表示竖直)
  *		  横屏情况下二维容器为
  *
  */
uint8_t OLED_GRAM[96][16];

//反显函数
void OLED_ColorTurn(uint8_t i){
	if(i == 0)
		OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
	if(i == 1)
		OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
}

//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i){
	if(i == 0){
		OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/
		OLED_WR_Byte(0x68,OLED_CMD); /*18*/
		OLED_WR_Byte(0xC0,OLED_CMD);//正常显示
		OLED_WR_Byte(0xA0,OLED_CMD);
	}
	if(i == 1){
		OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/
		OLED_WR_Byte(0x18,OLED_CMD); /*18*/
		OLED_WR_Byte(0xC8,OLED_CMD);//反转显示
		OLED_WR_Byte(0xA1,OLED_CMD);
	}
}

//延时
void IIC_delay(void){
    uint8_t t = 10;
    while(t--);
}

//起始信号
void I2C_Start(void){
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//结束信号
void I2C_Stop(void){
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//等待信号响应
void I2C_WaitAck(void){
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//写入一个字节
void Send_Byte(uint8_t dat){
	uint8_t i;
	for(i=0; i<8; i++){
		if(dat & 0x80)
			OLED_SDA_Set();//将dat的8位从最高位依次写入
		else
			OLED_SDA_Clr();
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();//将时钟信号设置为低电平
		dat <<= 1;
	}
}

//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t mode){
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode)
		Send_Byte(0x40);
	else
		Send_Byte(0x00);
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//开启OLED显示 
void OLED_DisPlay_On(void){
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void){
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}

//更新显存到OLED	
void OLED_Refresh(void){
	uint8_t i, n;
	for(i=0; i<16; i++){
		OLED_WR_Byte(0xB0+i, OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00, OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10, OLED_CMD);   //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck(); 
		for(n=0; n<80; n++){
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
}

//清屏函数
void OLED_Clear(void)
{
	uint8_t i, n;
	for(i=0; i<16; i++){
		for(n=0; n<80; n++)
			OLED_GRAM[n][i]=0;//清除所有数据
	}
	OLED_Refresh();//更新显示
}

/** @brief 绘制点
  * @param x:0-128	y:0-80	t:填充模式 1黑底白字 0表示白底黑字
  * @note 
  */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
	uint8_t i, m, n;
	i = y / 8;		//取整数
	m = y % 8;		//取余数
	n = 1 << m;
	 if(t)	//黑底白字
		OLED_GRAM[x][i] |= n;
	else{	//白底黑字
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
}

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
	uint16_t t; 
	int xerr=0, yerr=0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
    uint8_t temp;
    if(x1>=128)
        x1 = 128-1;
    else if(x2>=128)
        x2 = 128-1;
    else if(y1>=64)
        y1 = 64-1;
    else if(y2>=64)
        y2 = 64-1;
    if(y1 > y2){
        temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
    }
	delta_x = x2 - x1; 			//计算坐标增量 
	delta_y = y2 - y1;
	uRow = x1;					//画线起点坐标
	uCol = y1;
	if(delta_x > 0)
		incx = 1; 				//设置单步方向 
	else if(delta_x == 0)
		incx=0;	//垂直线 
	else{
		incx=-1;
		delta_x = -delta_x;
	}
	if(delta_y > 0)
		incy = 1;
	else if(delta_y == 0)
		incy=0;	//水平线 
	else{
		incy=-1;
		delta_y = -delta_x;
	}
	if(delta_x > delta_y)
		distance = delta_x; 	//选取基本增量坐标轴 
	else 
		distance = delta_y;
	for(t=0; t<distance+1; t++){
		OLED_DrawPoint(uRow,uCol,mode);		//画点
		xerr += delta_x;
		yerr += delta_y;
		if(xerr > distance){
			xerr -= distance;
			uRow += incx;
		}
		if(yerr>distance){
			yerr -= distance;
			uCol += incy;
		}
	}
}

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r){
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0){
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode)
{
	uint8_t i, m, temp, size2, chr1;
	uint8_t x0 = x, y0 = y;
	if(size1 == 8)
		size2 = 6;
	else 
		size2 = (size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1 = chr-' ';  //计算偏移后的值
	for(i=0; i<size2; i++){
		if(size1 == 8)
			temp = asc2_0806[chr1][i];//调用0806字体
		else if(size1 == 12)
			temp = asc2_1206[chr1][i];//调用1206字体
		else if(size1 == 16)
			temp = asc2_1608[chr1][i];//调用1608字体
		else if(size1 == 24)
			temp = asc2_2412[chr1][i];//调用2412字体
		else 
			return;
		for(m=0; m<16; m++){
			if(temp&0x01)
				//OLED_DrawPoint(x, y, mode);
				OLED_DrawPoint(y, x, mode);
			else 
				//OLED_DrawPoint(x, y, !mode);
				OLED_DrawPoint(y, x, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2)){
			x=x0;
			y0=y0+8;
		}
		y=y0;
	}
}

//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1, uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x, y, *chr, size1, mode);
		if(size1 == 8)
			x += 6;
		else 
			x += size1 / 2;
		chr++;
  }
}

//m^n
uint32_t OLED_Pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
	uint8_t t, temp, m=0;
	if(size1 == 8)
		m=2;
	for(t=0; t<len; t++){
		temp = (num/OLED_Pow(10,len-t-1))%10;
		if(temp == 0)
			OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
		else 
			OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
	}
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1, uint8_t mode)
{
	uint8_t m, temp;
	uint8_t x0=x, y0=y;
	uint16_t i, size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
	for(i=0; i<size3; i++){
		if(size1 == 16)
			temp = Hzk1[num][i];//调用16*16字体
		else if(size1 == 24)
			temp=Hzk2[num][i];	//调用24*24字体
		else if(size1 == 32)  
			temp=Hzk3[num][i];	//调用32*32字体
		else if(size1 == 64)
			temp=Hzk4[num][i];	//调用64*64字体
		else 
			return;
		
		for(m=0; m<8; m++){
			if(temp & 0x01)
				OLED_DrawPoint(y, x, mode);
				//OLED_DrawPoint(x, y, mode);
			else 
				//OLED_DrawPoint(x, y,!mode);
				OLED_DrawPoint(y, x, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if((x-x0)==size1){
			x = x0;
			y0 = y0 + 8;
		}
		y=y0;
	}
}

/** @brief 滚屏效果
  * @param num 显示汉字的个数
  *		   space 每一遍显示的间隔
  *        mode:0,反色显示;1,正常显示
  *
  */
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode)
{
	uint8_t i, n, t=0, m=0, r;
	while(1){
		//显示要显示文字的位置和文字个数
		if(m==0){
			OLED_ShowChinese(112, 64, t, 16, mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		
		if(t == num){
			for(r=0; r<16*space; r++){      //显示间隔
				for(i=1; i<96; ++i){	//
					for(n=0; n<16; n++){
						OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
					}
				}
				OLED_Refresh();
			}
			t=0;
		}
		
		m++;
		if(m == 16)
			m = 0;
		 
		for(i=1; i<96; ++i){   //横屏状态下实现上移
			for(n=0; n<16; n++)
				OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
		}
		OLED_Refresh();
	}
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[], uint8_t mode)
{
	uint16_t j = 0;
	uint8_t i, n, temp, m;
	uint8_t x0 = x, y0 = y;
	sizey = sizey/8 + ((sizey%8) ? 1:0);
	for(n=0; n<sizey; n++){
		for(i=0; i<sizex; i++){
			temp = BMP[j];
			j++;
			for(m=0; m<8; m++){
				if(temp & 0x01)
					//OLED_DrawPoint(x, y, mode);
					OLED_DrawPoint(y, x, mode);
				else 
					//OLED_DrawPoint(x, y, !mode);
					OLED_DrawPoint(y, x, !mode);
				temp >>= 1;
				y++;
			}
			x++;
			if((x-x0) == sizex){
				x=x0;
				y0=y0+8;
			}
			y=y0;
		}
	}
}

////////////////////////////////////
/**
 * @brief 波形绘制函数-设置起始位置
 * */
void Wave_SetPos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xB0+x, OLED_CMD);
    OLED_WR_Byte((y&0x0F)|0x00, OLED_CMD);
    OLED_WR_Byte(((y&0xF0)>>4)|0x10, OLED_CMD);
}

/**
 * @brief 波形绘制函数-描点函数
 * */
void Wave_DrawPoint(uint8_t x, uint8_t y)
{
    if(x>127 & y>63)
        return;
    uint8_t  page, move, data;
    page = 7-y/8;
    move = 7-y%8;
    data = 0x01<<move;
    Wave_DrawPoint(page, x);
    OLED_WR_Byte(data, OLED_DATA);
}

uint8_t Bef[3], Cur[3];
void Before_State_Update(uint8_t y)
{
	Bef[0] = 7-y/8;
	Bef[1] = 7-y%8;
	Bef[2] = 1<<Bef[1];
}

void Current_State_Update(uint8_t y)
{
	Cur[0] = 7-y/8;
	Cur[1] = 7-y%8;
	Cur[2] = 1<<Cur[1];
}

void OLED_DrawWave(int8_t x, int8_t y)
{
    int8_t page_sub;
    uint8_t page_buff, i, j;
    y += 32;
    if(x>127 | y>63){
        x = 127;
        y = 63;
    }
    if(x<0 | y<0){
        x = 0;
        y = 0;
    }
    Current_State_Update(y);
    page_sub = Bef[0]-Cur[0];
    if(page_sub>0){
        page_buff = Bef[0];
        Wave_SetPos(page_buff, x);
        OLED_WR_Byte(Bef[2]-0x01, OLED_DATA);
        page_buff--;
        for(i=0;i<page_sub-1;i++){
            Wave_SetPos(page_buff,x);
            OLED_WR_Byte(0xff, OLED_DATA);
            page_buff--;
        }
        Wave_SetPos(page_buff,x);
        OLED_WR_Byte(0xff<<Cur[1], OLED_DATA);
    }
    else if(page_sub == 0){
        if(Cur[1] == Bef[1]){
            Wave_SetPos(Cur[0], x);
            OLED_WR_Byte(Cur[2], OLED_DATA);
        }
        else if(Cur[1] > Bef[1]){
            Wave_SetPos(Cur[0], x);
            OLED_WR_Byte((Cur[2]-Bef[2])|Cur[2], OLED_DATA);
        }
        else if(Cur[1] < Bef[1]){
            Wave_SetPos(Cur[0], x);
            OLED_WR_Byte(Bef[2]-Cur[2], OLED_DATA);
        }
    }
    else if(page_sub < 0){
        page_buff = Cur[0];
        Wave_SetPos(page_buff, x);
        OLED_WR_Byte((Cur[2]<<1)-0x01, OLED_DATA);
        page_buff--;
        for(i=0; i<0-page_sub-1; i++){
            Wave_SetPos(page_buff, x);
            OLED_WR_Byte(0xff, OLED_DATA);
            page_buff--;
        }
        Wave_SetPos(page_buff, x);
        OLED_WR_Byte(0xff<<(Bef[1]+1), OLED_DATA);
    }
    Before_State_Update(y);
    //数据清除
    for(i=0; i<8; ++i){
        Wave_SetPos(i, x+1) ;
        for(j=0; j<1; j++)
            OLED_WR_Byte(0x00, OLED_DATA);
    }
}


//OLED的初始化
void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	__GPIOB_CLK_ENABLE();
	//GPIO初始化设置  SDA 
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO初始化设置  SCL  
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    HAL_Delay(50);

	OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
	OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/
	OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/
	OLED_WR_Byte(0x20,OLED_CMD); /* Set Memory addressing mode (0x20/0x21) */
	OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/
	OLED_WR_Byte(0x6f,OLED_CMD); /*b0*/
	OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/
	OLED_WR_Byte(0xC0,OLED_CMD); /*Com scan direction*/
	OLED_WR_Byte(0xA4,OLED_CMD); /*Disable Entire Display On (0xA4/0xA5)*/
	OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/
	OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/
	OLED_WR_Byte(0x91,OLED_CMD);
	OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/
	OLED_WR_Byte(0x22,OLED_CMD);
	OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/
	OLED_WR_Byte(0x3f,OLED_CMD);
	OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/
	OLED_WR_Byte(0x4F,OLED_CMD); /*duty = 1/80*/
	OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/
	OLED_WR_Byte(0x68,OLED_CMD); /*18*/
	OLED_WR_Byte(0xdc,OLED_CMD); /*Set Display Start Line*/
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0xad,OLED_CMD); /*set charge pump enable*/
	OLED_WR_Byte(0x8a,OLED_CMD); /*Set DC-DC enable (a=0:disable; a=1:enable) */
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/

    HAL_Delay(20);
}

void OLED_Windows_Style1(void){
    OLED_ShowString(0, 0, (uint8_t*)"Cyber", 24, 1);
	OLED_DrawLine(0, 64, 26, 64, 1);
	OLED_DrawLine(1, 64, 27, 64, 1);
	OLED_DrawLine(26, 0, 80, 0, 1);
	OLED_DrawLine(26, 1, 80, 1, 1);
	OLED_ShowString(4, 28, (uint8_t*)"P:", 16, 1);
	OLED_ShowString(64, 28, (uint8_t*)"AP:", 16, 1);
	OLED_ShowString(4, 46, (uint8_t*)"V:", 16, 1);
	OLED_ShowString(4, 62, (uint8_t*)"C:", 16, 1);
	OLED_ShowString(118, 10, (uint8_t*)"V", 16, 1);
    OLED_Refresh();
}


