/****************************************
/**此代码写于2018/9/1，作者石小星-stxhotstar@163.com**
*图片双线性插值程序（纯c语言代码），实现彩色bmp图像的快速几何变换
*输入任意多个图片的pathname，输入平移数，旋转角度，缩放倍数，输出各相应图像
*算法主要是行最近邻插值的平移，旋转，缩放，双线性插值的平移，旋转，缩放功能
*实现方法分为
*第一步：打开图片，获得图片的头信息和图像数据信息
*第二步：将获得的一维图像数据格式转换成二维数据格式并申请变换后的图像内存
*第三步：运用矩阵线性运算对图像进行插值变换
*第四步：更改变换后图像的头信息，将变换后图像的头信息和图像数据写入
*
 *****************************************/
#include <stdio.h>
#include <sys/malloc.h>
#include <string.h>
#include <math.h>
#include "picture.h"
#include <stdlib.h>

void array_multiply( float(*model)[3], float *input_date,float *output_date);//矩阵线性运算
void translation(picture_info *picture,float xin_translation,float yin_translation);	//进行图像平行操作
void spin(picture_info *picture,float angle);		//进行图像旋转操作
void amplify(picture_info *picture,float xin,float yin);		//进行图像缩放操作
void read_picture(char *pathname,picture_info *picture);//图片读取函数
void write_picture(picture_info *picture);//图片写入函数
void Double_liner(float *output_date,picture_info *picture,unsigned int i,unsigned int j);//双线性插值
void Single_liner(float *output_date,picture_info *picture,unsigned int i,unsigned int j);//最近邻插值
void dimensions_new_free(picture_info *picture);//二维动态内存释放函数
void dimensions_free(picture_info *picture);//二维动态内存释放函数

int main(int argc,char *argv[])//主函数需要传参数
{
	if(argc == 1)
	{
		printf("argc 1= 2,\n");
		return -1;
	}
    //int kkk = 3;
    //char table[3][20] = {{"chutian.bmp"},{"grass.bmp"},{"toer.bmp"}};//如果是在命令台下进行运行，就不需要手动设置图像的pathname，
	picture_info picture;//定义图像结构体
    float xin = 0.5,yin = 0.5,angle = 30,xin_translation = 100,yin_translation = 100;
    printf("输入放大倍数，xin yin\n");
    scanf("%f %f",&xin,&yin);
    printf("输入旋转角度，angle\n");
    scanf("%f",&angle);
    printf("输入平移数，xin yin\n");
    scanf("%f %f",&xin_translation,&yin_translation);
    //while(--K != 0)//windows下IDE下运行用这个
    while(--argc != 0)//循环处理图片，命令台下运行用这个
    {
	read_picture(argv[argc],&picture);//读取图像信息
    picture.flag = Double_Liner;//设定图像的插值类型（最近邻插值或双线性插值）

    /*双线性插值图像双线性平移*/
    memset(picture.name,0,40);
    strcpy(picture.name,"double_liner_translation_");
    strcat(picture.name,argv[argc]);//设置变换后图像的存储名字。命令台下运行用这个
    //strcat(picture.name,argv[argc]);//设置变换后图像的存储名字，windows下IDE下运行用这个，下面类似设置，
    translation(&picture,xin_translation,yin_translation);//平移变换
    //printf("pppppp\n");
    write_picture(&picture);//写入变换后的图像
    dimensions_new_free(&picture);//释放内存
    
    /*双线性插值图像双线性旋转*/
    memset(picture.name,0,40);
    strcpy(picture.name,"double_liner_spin_");
    strcat(picture.name,argv[argc]);
    spin(&picture,angle);
    write_picture(&picture);
    dimensions_new_free(&picture);

    /*双线性插值图像双线性放大*/
    memset(picture.name,0,40);
    strcpy(picture.name,"double_liner_amplify_");
    strcat(picture.name,argv[argc]);
    amplify(&picture,xin,yin);
    write_picture(&picture);
    dimensions_new_free(&picture);
    
    picture.width = picture.rwidth;
    picture.height = picture.rheight;
    *((unsigned int*)&picture.head[18]) = picture.rwidth;
    *((unsigned int*)&picture.head[22]) = picture.rheight;
        
    picture.flag = Single_Liner;//设置图像的插值类型为最近邻插值
    
    /*最近邻插值图像最近邻插值平移*/
    memset(picture.name,0,40);
    strcpy(picture.name,"single_liner_translation_");
    strcat(picture.name,argv[argc]);
       // printf("dasdsads\n");
    translation(&picture,xin_translation,yin_translation);//平移变换
        //printf("aaaaaaaaa\n");
    write_picture(&picture);
    dimensions_new_free(&picture);
    
    /*最近邻插值图像最近邻插值旋转*/
    memset(picture.name,0,40);
    strcpy(picture.name,"single_liner_spin_");
    strcat(picture.name,argv[argc]);
    spin(&picture,angle);
    write_picture(&picture);
    dimensions_new_free(&picture);
    
    /*最近邻插值图像最近邻插值放大*/
    memset(picture.name,0,40);
    strcpy(picture.name,"single_liner_amplify_");
    strcat(picture.name,argv[argc]);
    amplify(&picture,xin,yin);
    write_picture(&picture);
    dimensions_new_free(&picture);
    dimensions_free(&picture);
}
	return 0;
}


/*图片读取函数*/
void read_picture(char *pathname,picture_info *picture)
{
	FILE *file;
	int flag = -1,num = 0,i,j;
	file = fopen(pathname, "rb");//打开图片
	if(file == NULL)
	printf("fopen error\n");
	flag = fread(picture->head, 1, 54, file);//读取图像的头文件信息，并存储在结构体中
	if(flag != 54)
	printf("fread picture.head error\n");
	picture->width = *((unsigned int*)&picture->head[18]);
	picture->height = *((unsigned int*)&picture->head[22]);//从头文件信息中读取图像的分辨率，并存储在结构体中
    picture->rwidth = *((unsigned int*)&picture->head[18]);
    picture->rheight = *((unsigned int*)&picture->head[22]);//存储图像原始的分辨率
    printf("width = %d, height = %d\n",picture->width,picture->height);
	printf("offset date is  = %d\n",*((unsigned int*)&picture->head[10]));
	picture->dimension = (unsigned char *)malloc(sizeof(unsigned char)*picture->width*picture->height*3);//申请动态内存存储图像的原始数据（原始数据格式为RGB格式，字节位单位），一维图像数据
	if(NULL == picture->dimension)
	printf("picture.dimension malloc error \n");
	flag = fread(picture->dimension,1,picture->height*picture->width*3,file);
	//将图像的数据存储为一维图像数据
	if(flag != picture->height*picture->width*3)
	printf("picture.dimension fread errpr flag = %d\n",flag);
   /********将一维图像数据转换成二维图像数据，一维转换成二维************/
	picture->dimensions = (unsigned char **)malloc(sizeof(unsigned char *)*picture->height);  
	for(i=0;i<picture->height;i++)  
	picture->dimensions[i]=(unsigned char *)malloc(sizeof(unsigned char)*picture->width*3); //申请动态内存，存储变换后的二维数据	
	for(i =0,num = 0;i<picture->height;i++)
	{
		for(j = 0;j<picture->width;j++)
		{
			picture->dimensions[i][j*3+0] = (picture->dimension[num+0]);
			picture->dimensions[i][j*3+1] = (picture->dimension[num+1]);
			picture->dimensions[i][j*3+2] = (picture->dimension[num+2]);
			num += 3;
		}
	}
    /*********************************************************/
    free(picture->dimension);
	fclose(file);//关闭文件
}

void array_multiply( float(*model)[3], float *input_date,float *output_date)//矩阵运算函数
{
	output_date[0] = input_date[0]*model[0][0]+input_date[1]*model[0][1]+input_date[2]*model[0][2];
	output_date[1] = input_date[0]*model[1][0]+input_date[1]*model[1][1]+input_date[2]*model[1][2];
	
}

/*********************************
*平移变换函数						
*a(x)= x+x0,b(x)=y+y0;则变换矩阵为 [1 0 -x0;0 1 -y0;0 0 1]
*输入为变换后的坐标，输出为原始坐标。输入值对于输出值。则得到变换后的数据		
**********************************/
void translation(picture_info *picture,float xin_translation,float yin_translation)
{
	int x,y,i,j,num = 0;
	float model[3][3] = { {1,0,20},{0,1,66},{0,0,1}};//定义变换矩阵，设置成向下平移20个像素，向左平移66个像素单位
    model[0][2] = yin_translation;
    model[1][2] = xin_translation;
	float input_date[3] ={1,2,1},output_date[3] ={0};//定义输入矩阵模板
  //  printf("%d %d \n",picture->height,picture->width);
	picture->dimensions_new = (unsigned char **)malloc(sizeof(unsigned char *)*picture->height);
	for(i=0;i<picture->height;i++)  
	picture->dimensions_new[i]=(unsigned char *)malloc(sizeof(unsigned char)*picture->width*3); //申请动态内存，存储变换后的二维数据
	
	for(i =0;i<picture->height;i++)
	{
		for(j = 0;j<picture->width*3;j++)
		{
			picture->dimensions_new[i][j] = 0;
		}
	}//内存清零
	for(i =0,num = 0;i<picture->height;i++)//循环输入变换后每个像素的位置，进行几何变换
	{
		for(j = 0;j<picture->width;j++)
		{
			input_date[0] = i;
			input_date[1] = j;
			array_multiply(model,input_date,output_date);//进行矩阵变换运算
            if(picture->flag == Double_Liner)//选择进行相应的插值运算
                Double_liner(output_date,picture,i,j);
            else
                Single_liner(output_date,picture,i,j);
		}
	}
}

/*图像缩放函数*/
void amplify(picture_info *picture,float xin,float yin)
{
    unsigned int i,j,num = 0,new_height,new_width;
    float temp = 0;
	float model[3][3] = { {0.3,0,0},{0,0.7,0},{0,0,1}};//定义变换矩阵，设置为图像放大二倍
	float input_date[3] ={1,2,1},output_date[3] ={0};//定义输入矩阵模板
    model[0][0] = yin;
    model[1][1] = xin;
    new_width= (unsigned int)((float)picture->width/xin);
	new_height = (unsigned int)((float)picture->height/yin);//设置新图像的图像分辨率printf("%f %f \n",tempx,tempy);
    num = new_width;
    temp = (float)((float)num*3/4);
    temp = temp - (unsigned int)(num*3/4);//每个地址存储一个字节，但是由于内存物理特性，实际存储是按照4字节对齐存储，因此，申请的内存必须是按照4字节对齐
    while(temp != 0)//否则在进行数据处理是会出现数据错位等现象，这段代码就是使申请的内存4字节对齐。
    {
        
        new_width++;
        num = new_width;
        temp = (float)((float)num*3/4);
        temp = temp - (unsigned int)(num*3/4);
    }
   // printf("new_width = %d new_higth = %d\n",new_width,new_height);
    new_width+=0;
    picture->dimensions_new = (unsigned char **)malloc(sizeof(unsigned char *)*new_height);
	for(i=0;i<new_height;i++)  
	picture->dimensions_new[i]=(unsigned char *)malloc(sizeof(unsigned char)*new_width*3); //申请动态内存，存储变换后的二维数据
		for(i =0;i<new_height;i++)
		{
		for(j = 0;j<new_width*3;j++)
			{
				picture->dimensions_new[i][j] = 250;
			}
		}	//清零
	for(i =0;i<new_height;i++)//循环输入每个像素的位置，进行几何变换
	{
		for(j = 0;j<=new_width;j++)
		{
			input_date[0] = i;
			input_date[1] = j;
			array_multiply(model,input_date,output_date);//进行矩阵变换运算
            if(picture->flag == Double_Liner)
                Double_liner(output_date,picture,i,j);
            else
                Single_liner(output_date,picture,i,j);
		}		
	}
	*((unsigned int*)&picture->head[18]) = new_width;
	*((unsigned int*)&picture->head[22]) = new_height;
	picture->width = *((unsigned int*)&picture->head[18]);
	picture->height = *((unsigned int*)&picture->head[22]);//存储新图像的图像分辨率
	*((unsigned int*)&picture->head[2]) = picture->width*picture->height*3+54;//存储新图像的图像大小

}

/*图像数据写函数*/
void write_picture(picture_info *picture)
{
	int i ,j,num=0,flag;
    unsigned int Width = picture->width,Height = picture->height;
	FILE *file;
    picture->dimension = (unsigned char *)malloc(sizeof(unsigned char)*Width*Height*3);//申请动态内存存储图像的原始数据（原始数据格式为RGB格式，字节位单位），一维图像数据
	if(NULL == picture->dimension)
	printf("picture.dimension malloc error \n");	
	for(i =0;i<Height;i++)//循环输入每个像素的位置，进行几何变换
	{
		for(j = 0;j<Width;j++)
		{
            picture->dimension[num+0]	= picture->dimensions_new[i][j*3+0];
            picture->dimension[num+1]	= picture->dimensions_new[i][j*3+1];
            picture->dimension[num+2]	= picture->dimensions_new[i][j*3+2];
			num +=3;
		}
	}//图像数据从二维格式转换为一维格式
	file = fopen(picture->name, "wb");
	if(file == NULL)
	printf("fopen erroe\n");
	flag = fwrite(picture->head,1,54,file);
	if(flag != 54)
		printf("fwrite0 failed \n");
	flag = fwrite(picture->dimension,1,Width*Height*3,file);
	if(flag != Width*Height*3)
		printf("fwrite1 failed \n");
   // printf("picture_width = %d picture_higth = %d\n",picture->width,picture->height);
    //printf("width = %d higth = %d\n",*((unsigned int*)&picture->head[18]),*((unsigned int*)&picture->head[22]));
    free(picture->dimension);
	fclose(file);
}

/*图像旋转函数*/
void spin(picture_info *picture,float angle)
{
	int x,y,i,j;
	float model[3][3] = {{cos((3.14*angle)/180),-sin((3.14*angle)/180),0},{sin((3.14*angle)/180),cos((3.14*angle)/180),0},{0,0,1}};//定义变换矩阵，逆时针旋转30度
	float input_date[3] ={1,2,1},output_date[3] ={0};//定义输入矩阵模板
    picture->dimensions_new = (unsigned char **)malloc(sizeof(unsigned char *)*picture->height);
    for(i=0;i<picture->height;i++)
        picture->dimensions_new[i]=(unsigned char *)malloc(sizeof(unsigned char)*picture->width*3); //申请动态内存，存储变换后的二维数据
    for(i =0;i<picture->height;i++)
    {
        for(j = 0;j<picture->width*3;j++)
        {
            picture->dimensions_new[i][j] = 0;
        }
    }
	for(i =0;i<picture->height;i++)//循环输入每个像素的位置，进行几何变换
	{
		for(j = 0;j<picture->width;j++)
		{
			input_date[0] = i;
			input_date[1] = j;
			array_multiply(model,input_date,output_date);//进行矩阵变换运算
            if(picture->flag == Double_Liner)
                Double_liner(output_date,picture,i,j);
            else
                Single_liner(output_date,picture,i,j);
		}	
		
	}	
	
}


/*双线性插值函数*/
void Double_liner(float *output_date,picture_info *picture,unsigned int i,unsigned int j)
{
    int position0,position1;
	double ar,br,cr,dr,x,y,ag,bg,cg,dg,ab,bb,cb,db,r0,g0,b0;//图像的像素是RGB格式，因此进行插值的时候，也需要把RGB变量分开进行插值，abcd表示【0，0】，【0，1】，【1，0】，【1，1】点
	position0 = (int)output_date[0];
	position1 = (int)output_date[1];
	
	if(position0>=0&(position0)<picture->height&position1>=0&(position1)<picture->width)
	{
		if(position0+1 == picture->height)
			position0 --;
		if(position1+1 == picture->width)
			position1 --;
		ar = picture->dimensions[position0][(position1*3)+0];
		br = picture->dimensions[position0][((position1+1)*3)+0];
		cr = picture->dimensions[position0+1][(position1*3)+0];
		dr = picture->dimensions[position0+1][((position1+1)*3)+0];
	
		ag = picture->dimensions[position0][(position1*3)+1];
		bg = picture->dimensions[position0][((position1+1)*3)+1];
		cg = picture->dimensions[position0+1][(position1*3)+1];
		dg = picture->dimensions[position0+1][((position1+1)*3)+1];
	
		ab = picture->dimensions[position0][(position1*3)+2];
		bb = picture->dimensions[position0][((position1+1)*3)+2];
		cb = picture->dimensions[position0+1][(position1*3)+2];
		db = picture->dimensions[position0+1][((position1+1)*3)+2];
	
		y = output_date[1]-(int)output_date[1];
		x = output_date[0]-(int)output_date[0];//类型转换
		if(x<0.01)
			x =0;
		if(y<0.01)
			y = 0;
		r0 = (cr-ar)*x+(br-ar)*y+(ar+dr-br-cr)*x*y+ar;	
		g0 = (cg-ag)*x+(bg-ag)*y+(ag+dg-bg-cg)*x*y+ag;
		b0 = (cb-ab)*x+(bb-ab)*y+(ab+db-bb-cb)*x*y+ab;
        picture->dimensions_new[i][j*3+0] = r0;
        picture->dimensions_new[i][j*3+1] = g0;
        picture->dimensions_new[i][j*3+2] = b0;
	}
}

void Single_liner(float *output_date,picture_info *picture,unsigned int i,unsigned int j)
{
    int position0,position1;
    position0 = (unsigned int)output_date[0];
    position1 = (unsigned int)output_date[1];
    if(position0>=0&(position0)<picture->height&position1>=0&(position1)<picture->width)
    {
        picture->dimensions_new[i][j*3+0] = picture->dimensions[position0][position1*3+0];
        picture->dimensions_new[i][j*3+1] = picture->dimensions[position0][position1*3+1];
        picture->dimensions_new[i][j*3+2] = picture->dimensions[position0][position1*3+2];
        
    }
}

/*释放二维动态内存*/
void dimensions_new_free(picture_info *picture)
{
    int i ;
    for(i = 0;i<picture->height;i++)
    {
        free(picture->dimensions_new[i]);
    }
    free(picture->dimensions_new);
    
}

/*释放二维动态内存*/
void dimensions_free(picture_info *picture)
{
    int i ;
    for(i = 0;i<picture->rheight;i++)
    {
        free(picture->dimensions[i]);
    }
    free(picture->dimensions);
}













