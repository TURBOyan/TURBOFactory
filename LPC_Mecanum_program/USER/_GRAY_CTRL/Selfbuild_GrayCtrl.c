#include "Selfbuild_GrayCtrl.h"


PIN_enum Gray[6][6]={		//光电管位置（第一行为车头）
{B31, A0, B11, B15 , A2,B30},
{A6,0xff,0xff,0xff,0xff,B16},
{A19,0xff,0xff,0xff,0xff,A5},
{A10 ,0xff,0xff,0xff,0xff,A18},	
{B20,0xff,0xff,0xff,0xff,A17},
{B19,B18 , B6 ,A21 , A20 ,B28},
};

//PIN_enum Gray[6][6]={		//光电管位置（第一行为车头）
//{B31 , A0 , B11,0xff,0xff,0xff},
//{A6  ,0xff,0xff,0xff,0xff,0xff},
//{A19 ,0xff,0xff,0xff,0xff,0xff},
//{0xff,0xff,0xff,0xff,0xff, A18},	
//{0xff,0xff,0xff,0xff,0xff, A17},
//{0xff,0xff,0xff,A21 ,A20 , B28},
//};

void Gray_RotationClockwise90(PIN_enum a[6][6])		//将6x6数组顺时针旋转90度
{
	uint8 offect,first,last;
	PIN_enum top;
	for(uint8 layer=0;layer< 6/2;layer++)
	{
		first = layer;
		last = 6-1 - layer;
		for(uint8 i = first;i<last;i++)
		{
			offect = i - first;
			top = a[first][i];
			a[first][i] = a[last-offect][first];
			a[last-offect][first] = a[last][last - offect];	
			a[last][last - offect] = a[i][last]; 
			a[i][last] = top;
		}
	}
}

void Read_GrayData(uint8 x,uint8 y,uint8 showflag)	//读取光电管值并设定是否显示
{
	if(showflag)
	{
		if(x>92)x=92;
		if(y>2)y=2;
	}
	for(uint8 row=0;row<6;row++)
	{
		for(uint8 col=0;col<6;col++)
		{
			if((uint8)(Gray[row][col])!=0xff)
			{
				Base_Data.Gray_Data[row][col]=gpio_get(Gray[row][col]);
				if(showflag)OLED_P6x8Int(col*6+x, row+y, Base_Data.Gray_Data[row][col], 1);
			}
		}
	}
}

void Save_GrayData(uint8 data_new[6][6],uint8 data_save[6][6])	//保存光电管值
{
	
	
	for(uint8 row=0;row<6;row++)
	{
		for(uint8 col=0;col<6;col++)
		{
			data_save[row][col]=data_new[row][col];
		}
	}
}

void Judge_GrayData(void)	//跳变点检测
{
	for(uint8 row=0;row<6;row++)
	{
		for(uint8 col=0;col<6;col++)
		{
			if(Base_Data.Gray_Data[row][col]<Base_Data.Gray_Data_Last[row][col])//如果上次光电管判断到白线而这次没有则下降沿置一
				Base_Data.Gray_Data_Fall[row][col]=1;
			else if(Base_Data.Gray_Data[row][col]>Base_Data.Gray_Data_Last[row][col])//如果这次光电管判断到白线而上次没有则上升沿置一
				Base_Data.Gray_Data_Rise[row][col]=1;
			else
			{
				Base_Data.Gray_Data_Fall[row][col]=0;
				Base_Data.Gray_Data_Rise[row][col]=0;
			}
		}
	}
}

uint8 Gray_Calibration(int16 X_Dir,int16 Y_Dir,int8 Return_Flag)
{
	static uint16 Continue_flag;
	uint8 Gray_xleft[6]	 	 ,Gray_xRight[6],
				Gray_yUp[6]	 	 	 ,Gray_yDown[6],
				Gray_xleft_sum=0 ,Gray_xRight_sum=0,
				Gray_yUp_sum=0	 ,Gray_yDown_sum=0;
	
	int8 X_Dir_judge,Y_Dir_judge;	//保存运动方向
	static int8 X_Flag=0,Y_Flag=0;
	
	for(uint8 num=0;num<6;num++)
	{
		Gray_xleft[num]=Base_Data.Gray_Data[num][0];	//保存左侧光电管值
		Gray_xleft_sum+=Gray_xleft[num];
		
		Gray_xRight[num]=Base_Data.Gray_Data[num][5];	//保存右侧光电管值
		Gray_xRight_sum+=Gray_xRight[num];
		
		Gray_yUp[num]=Base_Data.Gray_Data[0][num];	//保存上侧光电管值
		Gray_yUp_sum+=Gray_yUp[num];
		
		Gray_yDown[num]=Base_Data.Gray_Data[5][num];	//保存下侧光电管值
		Gray_yDown_sum+=Gray_yDown[num];
	}
	
	if(Gray_xleft_sum  == 6
	 &&Gray_xRight_sum == 6
	 &&Gray_yUp_sum    == 6
	 &&Gray_yDown_sum  == 6)
	{
		return 1;
	}
	
	X_Dir_judge=X_Dir>0?1:(X_Dir<0?-1:0);
	Y_Dir_judge=Y_Dir>0?1:(Y_Dir<0?-1:0);
	
	if(Return_Flag == 1)X_Dir_judge=-X_Dir_judge;
	if(Return_Flag ==-1)Y_Dir_judge=-Y_Dir_judge;
	
////X	
//	if(X_Dir == 0)
//	{
//	/********************************/	
//		if(X_Flag == 0
//		 &&Gray_xleft_sum > Gray_xRight_sum)
//		{
//			X_Flag =1;
//			X_Dir_judge = 1;
//		}
//		
//		if(X_Flag == 1
//		&& Gray_xleft_sum <= Gray_xRight_sum)
//		{
//			X_Flag =0;
//			X_Dir_judge = 0;
//		}
//	/********************************/	
//		if(X_Flag == 0
//		 &&Gray_xleft_sum < Gray_xRight_sum)
//		{
//			X_Flag =-1;
//			X_Dir_judge = -1;
//		}
//		
//		if(X_Flag == -1
//		&& Gray_xleft_sum <= Gray_xRight_sum)
//		{
//			X_Flag =0;
//			X_Dir_judge = 0;
//		}
//	/********************************/	
//	}
//	
////Y	
//	if(Y_Dir == 0)
//	{
//		
//	/********************************/	
//		if(Y_Flag == 0
//		 &&Gray_yUp_sum > Gray_yDown_sum)
//		{
//			Y_Flag =1;
//			Y_Dir_judge = 1;
//		}
//		
//		if(Y_Flag == 1
//		&& Gray_yUp_sum <= Gray_yDown_sum)
//		{
//			Y_Flag =0;
//			Y_Dir_judge = 0;
//		}
//	/********************************/	
//		if(Y_Flag == 0
//		 &&Gray_yUp_sum < Gray_yDown_sum)
//		{
//			Y_Flag =-1;
//			Y_Dir_judge = -1;
//		}
//		
//		if(Y_Flag == -1
//		&& Gray_yUp_sum <= Gray_yDown_sum)
//		{
//			Y_Flag =0;
//			Y_Dir_judge = 0;
//		}
//	/********************************/	
//	}
	
	if(Gray_xleft_sum  == 6
	 &&Gray_xRight_sum == 6
	)
		MECANUM_Motor_Data.Speed_Real.x=0;
	else
		MECANUM_Motor_Data.Speed_Real.x=200*X_Dir_judge;
	
	if(Gray_yUp_sum  == 6
	 &&Gray_yDown_sum == 6
	)
		MECANUM_Motor_Data.Speed_Real.y=0;
	else
		MECANUM_Motor_Data.Speed_Real.y=200*Y_Dir_judge;

	return 0;
	
}