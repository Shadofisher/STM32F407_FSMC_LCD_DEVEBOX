/*
 * lcd.c
 *
 *  Created on: May 26, 2021
 *      Author: graeme
 */

#include "lcd.h"
#include "main.h"
#include "font.h"

uint16_t POINT_COLOR=0x0000;
uint16_t BACK_COLOR=0xFFFF;


void delay_us(uint32_t delt)
{
	uint32_t i;

	for (i = 0; i < delt; i++);
}


void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;
 	delay_us(1);
}

void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	delay_us(1);
	LCD->LCD_REG = LCD_Reg;
	delay_us(1);
	LCD->LCD_RAM = LCD_RegValue;
}

void LCD_WR_REG(uint16_t regval)
{
	regval=regval;

	delay_us(1);
	LCD->LCD_REG=regval;
}
void LCD_WR_DATA(uint16_t data)
{
	data=data;
	delay_us(1);
	LCD->LCD_RAM=data;
}


uint16_t LCD_RD_DATA(void)
{
	uint16_t ram;			//防止被优化
	delay_us(1);
	ram=LCD->LCD_RAM;
	return ram;
}

uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	//delay_us(5);
	delay_us(1);
	return LCD_RD_DATA();		//返回读到的值
}


void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
 	if(lcddev.id==0X9341)
	{
 		delay_us(1);
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
		delay_us(1);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
		delay_us(1);
	}
}


void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1;
	height=ey-sy+1;
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);
		LCD_WriteRAM_Prepare();
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];
	}
}

void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	uint16_t i,j;
	uint16_t xlen=0;
	xlen=ex-sx+1;
	for(i=sy;i<=ey;i++)
	{
	 	LCD_SetCursor(sx,i);      				//ÉèÖÃ¹â±êÎ»ÖÃ
		LCD_WriteRAM_Prepare();     			//¿ªÊŒÐŽÈëGRAM
		for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//ÏÔÊŸÑÕÉ«
	}
}
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//µÃµœ×ÜµãÊý
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804ºáÆÁµÄÊ±ºòÌØÊâŽŠÀí
	{
 		lcddev.dir=0;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		LCD_SetCursor(0x00,0x0000);		//ÉèÖÃ¹â±êÎ»ÖÃ
 		lcddev.dir=1;
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
 	}else LCD_SetCursor(0x00,0x0000);	//ÉèÖÃ¹â±êÎ»ÖÃ
	LCD_WriteRAM_Prepare();
	delay_us(5);
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;
	}
}

void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;
	uint16_t xsize,ysize;
	if(lcddev.dir==1&&lcddev.id!=0X6804)//ºáÆÁÊ±£¬¶Ô6804²»žÄ±äÉšÃè·œÏò£¡
	{
		switch(dir)//·œÏò×ª»»
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;
		}
	}
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510,ºÜÌØÊâ
	{
		switch(dir)
		{
			case L2R_U2D://ŽÓ×óµœÓÒ,ŽÓÉÏµœÏÂ
				regval|=(0<<7)|(0<<6)|(0<<5);
				break;
			case L2R_D2U://ŽÓ×óµœÓÒ,ŽÓÏÂµœÉÏ
				regval|=(1<<7)|(0<<6)|(0<<5);
				break;
			case R2L_U2D://ŽÓÓÒµœ×ó,ŽÓÉÏµœÏÂ
				regval|=(0<<7)|(1<<6)|(0<<5);
				break;
			case R2L_D2U://ŽÓÓÒµœ×ó,ŽÓÏÂµœÉÏ
				regval|=(1<<7)|(1<<6)|(0<<5);
				break;
			case U2D_L2R://ŽÓÉÏµœÏÂ,ŽÓ×óµœÓÒ
				regval|=(0<<7)|(0<<6)|(1<<5);
				break;
			case U2D_R2L://ŽÓÉÏµœÏÂ,ŽÓÓÒµœ×ó
				regval|=(0<<7)|(1<<6)|(1<<5);
				break;
			case D2U_L2R://ŽÓÏÂµœÉÏ,ŽÓ×óµœÓÒ
				regval|=(1<<7)|(0<<6)|(1<<5);
				break;
			case D2U_R2L://ŽÓÏÂµœÉÏ,ŽÓÓÒµœ×ó
				regval|=(1<<7)|(1<<6)|(1<<5);
				break;
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510²»ÐèÒªBGR
		if(lcddev.id==0X6804)regval|=0x02;//6804µÄBIT6ºÍ9341µÄ·ŽÁË
		LCD_WriteReg(dirreg,regval);
 		if((regval&0X20)||lcddev.dir==1)
		{
			if(lcddev.width<lcddev.height)//œ»»»X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}else
		{
			if(lcddev.width>lcddev.height)//œ»»»X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}
		if(lcddev.dir==1&&lcddev.id!=0X6804)
		{
			xsize=lcddev.height;
			ysize=lcddev.width;
		}else
		{
			xsize=lcddev.width;
			ysize=lcddev.height;
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((xsize-1)>>8);
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((xsize-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((ysize-1)>>8);
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((ysize-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd);
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((xsize-1)>>8);LCD_WR_DATA((xsize-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd);
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((ysize-1)>>8);LCD_WR_DATA((ysize-1)&0XFF);
		}
  	}else
	{
		switch(dir)
		{
			case L2R_U2D://ŽÓ×óµœÓÒ,ŽÓÉÏµœÏÂ
				regval|=(1<<5)|(1<<4)|(0<<3);
				break;
			case L2R_D2U://ŽÓ×óµœÓÒ,ŽÓÏÂµœÉÏ
				regval|=(0<<5)|(1<<4)|(0<<3);
				break;
			case R2L_U2D://ŽÓÓÒµœ×ó,ŽÓÉÏµœÏÂ
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://ŽÓÓÒµœ×ó,ŽÓÏÂµœÉÏ
				regval|=(0<<5)|(0<<4)|(0<<3);
				break;
			case U2D_L2R://ŽÓÉÏµœÏÂ,ŽÓ×óµœÓÒ
				regval|=(1<<5)|(1<<4)|(1<<3);
				break;
			case U2D_R2L://ŽÓÉÏµœÏÂ,ŽÓÓÒµœ×ó
				regval|=(1<<5)|(0<<4)|(1<<3);
				break;
			case D2U_L2R://ŽÓÏÂµœÉÏ,ŽÓ×óµœÓÒ
				regval|=(0<<5)|(1<<4)|(1<<3);
				break;
			case D2U_R2L://ŽÓÏÂµœÉÏ,ŽÓÓÒµœ×ó
				regval|=(0<<5)|(0<<4)|(1<<3);
				break;
		}
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K
	 	}else//ÆäËûÇý¶¯IC
		{
			dirreg=0X03;
			regval|=1<<12;
		}
		LCD_WriteReg(dirreg,regval);
	}
}


void LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)			//ÊúÆÁ
	{
		lcddev.dir=0;	//ÊúÆÁ
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00;
			lcddev.width=480;
			lcddev.height=800;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R32;
			lcddev.setycmd=R33;
		}
	}else 				//ºáÆÁ
	{
		lcddev.dir=1;	//ºáÆÁ
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
		}else if(lcddev.id==0X6804)
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A;
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00;
			lcddev.width=800;
			lcddev.height=480;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.width=480;
			lcddev.height=320;
		}
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ä¬ÈÏÉšÃè·œÏò
}

void LCD_Init(void)
{
	  delay_us(50000); // delay 50 ms

	  LCD_WriteReg(0x0000,0x0001);
	  delay_us(50); // delay 50 ms
	  lcddev.id = LCD_ReadReg(0x0000);
	  LCD_WR_REG(0XD3);
	  delay_us(50);
	  lcddev.id=LCD_RD_DATA();	//dummy read
	  lcddev.id=LCD_RD_DATA(); 	//读到0X00
	  lcddev.id=LCD_RD_DATA();   	//读取93
	  lcddev.id<<=8;
	  lcddev.id|=LCD_RD_DATA();  	//读取41
	  FSMC_Bank1E->BWTR[6]&=~(0XF<<0);
	  FSMC_Bank1E->BWTR[6]&=~(0XF<<0);
	  FSMC_Bank1E->BWTR[6]&=~(0XF<<8);
	  FSMC_Bank1E->BWTR[6]|=3<<0;
	  FSMC_Bank1E->BWTR[6]|=2<<8;

		LCD_WR_REG(0xCF);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC1);
		LCD_WR_DATA(0X30);
		LCD_WR_REG(0xED);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x03);
		LCD_WR_DATA(0X12);
		LCD_WR_DATA(0X81);
		LCD_WR_REG(0xE8);
		LCD_WR_DATA(0x85);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x7A);
		LCD_WR_REG(0xCB);
		LCD_WR_DATA(0x39);
		LCD_WR_DATA(0x2C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x34);
		LCD_WR_DATA(0x02);
		LCD_WR_REG(0xF7);
		LCD_WR_DATA(0x20);
		LCD_WR_REG(0xEA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0xC0);    //Power control
		LCD_WR_DATA(0x1B);   //VRH[5:0]
		LCD_WR_REG(0xC1);    //Power control
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
		LCD_WR_REG(0xC5);    //VCM control
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2
		LCD_WR_DATA(0XB7);
		LCD_WR_REG(0x36);    // Memory Access Control
		LCD_WR_DATA(0x48);
		LCD_WR_REG(0x3A);
		LCD_WR_DATA(0x55);
		LCD_WR_REG(0xB1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1A);
		LCD_WR_REG(0xB6);    // Display Function Control
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0xA2);
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0x26);    //Gamma curve selected
		LCD_WR_DATA(0x01);
		LCD_WR_REG(0xE0);    //Set Gamma
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x2A);
		LCD_WR_DATA(0x28);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x0E);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x54);
		LCD_WR_DATA(0XA9);
		LCD_WR_DATA(0x43);
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0XE1);    //Set Gamma
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x15);
		LCD_WR_DATA(0x17);
		LCD_WR_DATA(0x07);
		LCD_WR_DATA(0x11);
		LCD_WR_DATA(0x06);
		LCD_WR_DATA(0x2B);
		LCD_WR_DATA(0x56);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x0F);
		LCD_WR_REG(0x2B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);
		LCD_WR_REG(0x11); //Exit Sleep
		delay_us(120);
//		LCD_WR_REG(0x29); //display on
		LCD_Display_Dir(0);		 	//Ä¬ÈÏÎªÊúÆÁ
		//LCD_LED=1;					//µãÁÁ±³¹â
		LCD_Clear(WHITE);
		delay_us(120);

		//LCD_Fill(1,1,230,310,BLUE);
		//LCD_Fill(50,50,130,110,GREEN);
		LCD_ShowString(30,40,210,24,24,"Stm32F4");
		LCD_ShowString(30,40,210,24,24,"STM32F4");
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
		LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
 		LCD_ShowString(30,110,200,16,16,"LCD: ILI9431");
		LCD_ShowString(30,130,200,12,12,"2021/05/28");
		LCD_ShowString(30,130,200,12,12,"Bug Fix: Graeme");
		LCD_WR_REG(0x29); //display on

}


void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(x>>8);
	LCD_WR_DATA(x&0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0XFF);

	LCD->LCD_REG=lcddev.wramcmd;
	LCD->LCD_RAM=color;
}

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
    uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);
	num=num-' ';
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=asc2_1206[num][t];
		else if(size==16)temp=asc2_1608[num][t];
		else if(size==24)temp=asc2_2412[num][t];
		else return;
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;
				break;
			}
		}
	}
}

void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}



