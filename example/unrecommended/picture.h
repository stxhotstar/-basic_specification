#ifndef __PICTURE__H
#define __PICTURE__H


typedef struct{
	unsigned int width;
	unsigned int height;
    unsigned int rwidth;
    unsigned int rheight;
	char head[54];
	unsigned char *dimension;
	unsigned char **dimensions;
	unsigned char **dimensions_new;
	char name[40];
	int flag;
}picture_info;

enum 
{
	Double_Liner,
	Single_Liner,

};


#endif





