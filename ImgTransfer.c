#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <opencv/highgui.h>


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

//transfer floating point to unsigned int
typedef union RGB_DATA{
	struct{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t junk;
	};
	unsigned int rgb;
	float f;
}RGB_DATA;

//14 Bytes for bitmap File Header
typedef struct bitmapFileHeader{
	uint16_t bfType;	//0X0000
	uint32_t bfSize;	//0X0002
	uint32_t reserved;	//0X0006
	uint32_t bfOffSet;	//0X000A
}__attribute__((packed,aligned(1))) BITMAP_FILE_HEADER;
//using __attribute__((packed,aligned(1)))  for removing data aligment

//40 Bytes for bitmap Info Header
typedef struct bitmapInfoHeader{
	uint32_t biHeaderSize;		//0X000E
	uint32_t biWidth;			//0X0012
	uint32_t biHeight;			//0X0016
	uint16_t biPlanes;			//0X001A
	uint16_t biPerPix;			//0X001C
	uint32_t biCompression;		//0X001E
	uint32_t biImgSize;			//0X0022
	uint32_t biXPixPerMeter;	//0X0026
	uint32_t biYPixPerMeter;	//0X002A
	uint32_t biUsedClr;			//0X002E
	uint32_t biImportantClr;	//0X0032
}__attribute__((packed,aligned(1))) BITMAP_INFO_HEADER;

//Header , total 54 Bytes
typedef struct bitmapHeader
{
	BITMAP_FILE_HEADER bmpfh;
	BITMAP_INFO_HEADER bmpih;
}__attribute__((packed,aligned(1))) BITMAP_HEADER;

int select_flag=0;
CvPoint rect[2];
CvPoint origin;
uint8_t *img=NULL;
IplImage *ori_ipimg=NULL;
IplImage *mod_ipimg=NULL;

//set rectangle info
CvScalar Color;
int Thickness=1;
int Shift=0;
	
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))


void ReadBMPFile(BITMAP_HEADER *bmph,char FileName[200])
{
	FILE *fr=fopen(FileName,"rb");
	fread(bmph,1,54,fr);
	
	int w=bmph->bmpih.biWidth;
	int h=bmph->bmpih.biHeight;
	int N=w*h;
	int DataSize=3*N;

	if(img) free(img);
	img=(uint8_t *)malloc(DataSize);
	
	//read RGBData if mode==1
	uint8_t pad[3];
	int i;
	for(i=0;i<h;i++)
	{
		fread(img+(w*i*3),3,w,fr);
		//padding bits
		fread(pad,1,(4-(w*3)%4)%4,fr);
	}
	fclose(fr);
}

void WriteBMPFile(BITMAP_HEADER bmph,char FileName[200])
{
	FILE *fw=fopen(FileName,"wb");

	fwrite(&bmph,1,54,fw);
	
	int w=bmph.bmpih.biWidth;
	int h=bmph.bmpih.biHeight;
	int N=w*h;
	int DataSize=3*N;
	uint8_t pad[3]={0,0,0};
	int i;
	for(i=0;i<h;i++)
	{
		fwrite(img+(w*i*3),3,w,fw);
		//padding bits
		fwrite(pad,1,(4-(w*3)%4)%4,fw);
	}
	fclose(fw);
//	printf("[200,200]=%d\n",img[(200+(h-1-200)*w)*3+2]);
//	printf("write file over\n");
}

void Reverse(int w,int h)
{
	int N=w*h;
	int DataSize=3*N;
	uint8_t *tmp=(uint8_t *)malloc(DataSize);
	//copy
	int i;
	for(i=0;i<DataSize;i++)
		tmp[i]=img[i];
	
	//reverse
	for(i=0;i<N;i++)
	{
		int x=i%w;
		int y=i/w;
		img[(x+y*w)*3+2]=tmp[(x+(h-1-y)*w)*3+2];
		img[(x+y*w)*3+1]=tmp[(x+(h-1-y)*w)*3+1];
		img[(x+y*w)*3+0]=tmp[(x+(h-1-y)*w)*3+0];
	}
	free(tmp);
}

void TurnGrayLevel(BITMAP_HEADER bmph)
{
	int w=bmph.bmpih.biWidth;
	int h=bmph.bmpih.biHeight;
	int N=w*h;
	int DataSize=3*N;
	

	int i;

	int cnt=0;

	//gray level
	for(i=0;i<N;i++)
	{
		int x=i%w;
		int y=i/w;
		
		//turn gray
		if(x>=rect[0].x && x<rect[1].x && y>(h-1-rect[1].y) && y<=(h-1-rect[0].y))
		{
			cnt++;
			int r=img[(x+y*w)*3+2];
			int g=img[(x+y*w)*3+1];
			int b=img[(x+y*w)*3+0];
			
			int gl=(r+g+b)/3;
			img[(x+y*w)*3+2]=gl;
			img[(x+y*w)*3+1]=gl;
			img[(x+y*w)*3+0]=gl;
		}
	}
//	printf("w=%d h=%d\n",w,h);
//	printf("in Turn Gray w=%d h=%d from (%d,%d) to (%d,%d) cnt=%d\n",w,h,rect[0].x,rect[0].y,rect[1].x,rect[1].y,cnt);
}

void onMouse(int event,int x,int y,int flag,void *param)
{
	if(select_flag)
	{
		//choose first and third points
		int change=0;
	
		//origin point will not move
		//first point
		rect[0].x=min(x,origin.x);
		rect[0].y=min(y,origin.y);
		//third point
		rect[1].x=max(x,origin.x);
		rect[1].y=max(y,origin.y);
		//clone and draw
		cvReleaseImage(&mod_ipimg);
		mod_ipimg=cvCloneImage(ori_ipimg);
		cvRectangle(mod_ipimg,rect[0],rect[1],Color,Thickness,CV_AA,Shift);
		cvShowImage("image",mod_ipimg);
	}

	if(event==CV_EVENT_LBUTTONDOWN)
	{
		select_flag=1;

		//first point of rectangle
		origin=rect[0]=rect[1]=cvPoint(x,y);
		
		//clone and draw
		cvReleaseImage(&mod_ipimg);
		mod_ipimg=cvCloneImage(ori_ipimg);
		cvRectangle(mod_ipimg,rect[0],rect[1],Color,Thickness,CV_AA,Shift);
		cvShowImage("image",mod_ipimg);
	}
	//rectangle complete
	else if(event==CV_EVENT_LBUTTONUP) 
	{
		select_flag=0;
		BITMAP_HEADER *bmph=(BITMAP_HEADER *)param;
		TurnGrayLevel(*bmph);
		WriteBMPFile(*bmph,"img.bmp");
		//release
		cvReleaseImage(&ori_ipimg);
		ori_ipimg=cvLoadImage("img.bmp",1);
		cvShowImage("image",ori_ipimg);

	}
}

int main(void)
{
	FILE *fr=fopen("islab3.pcd","r");
	char str_data[30]="DATA";
	char str_width[30]="WIDTH";
	char str_height[30]="HEIGHT";
	char str_tmp[100];
	char *p_str;
	float tmp;
	int width,height;
	RGB_DATA RGBdata;
	
	//Read until "DATA aascii"
	while(fgets(str_tmp,sizeof(str_tmp),fr)!=NULL)
	{
		//Get width
		if( (p_str=strstr(str_tmp,str_width))!=NULL)
		{
			sscanf(str_tmp,"WIDTH %d",&width);
		}
		//Get height
		else if((p_str=strstr(str_tmp,str_height))!=NULL)
		{
			sscanf(str_tmp,"HEIGHT %d",&height);
		}
		//Get Data type ->askii
		else if((p_str=strstr(str_tmp,str_data))!=NULL)
		{
			break;
		}
	}

	int i;
	int N=width*height;
	int DataSize=N*3;
	int file_size=54+DataSize;	//54 for File Header & Info Header
	FILE *fw;
	
	img=(uint8_t *)malloc(DataSize);
	memset(img,0,sizeof(img));

	//start read RGB info
	for(i=0;i<N;i++)
	{
		fscanf(fr,"%f%f%f%f",&tmp,&tmp,&tmp,&RGBdata.f);
		int x=i%width;
		int y=(height-1)-i/width; //
			
		img[(x+y*width)*3+2]=RGBdata.r;
		img[(x+y*width)*3+1]=RGBdata.g;
		img[(x+y*width)*3+0]=RGBdata.b;
	}
	fclose(fr);

	//Fill File Header

	/* using array
	uint8_t bmp_file_header[14]={0};
	memset(bmp_file_header,0,sizeof(bmp_file_header));
	bmp_file_header[0]='B';
	bmp_file_header[1]='M';
	bmp_file_header[2]=(uint8_t)(file_size);
	bmp_file_header[3]=(uint8_t)(file_size>>8);
	bmp_file_header[4]=(uint8_t)(file_size>>16);
	bmp_file_header[5]=(uint8_t)(file_size>>24);
	bmp_file_header[10]=54;
	*/
	BITMAP_HEADER bmph;
//	BITMAP_FILE_HEADER bmpfh;
	bmph.bmpfh.bfType=0x4d42; // 'BM'
	bmph.bmpfh.bfSize=file_size;
	bmph.bmpfh.reserved=0;
	bmph.bmpfh.bfOffSet=sizeof(BITMAP_FILE_HEADER)+sizeof(BITMAP_INFO_HEADER);//54?

	//Fill Info Header
	/*using array
	uint8_t bmp_info_header[40]={0};
	memset(bmp_info_header,0,sizeof(bmp_info_header));
	bmp_info_header[0]=40;
	bmp_info_header[4]=(uint8_t)(width);
	bmp_info_header[5]=(uint8_t)(width>>8);
	bmp_info_header[6]=(uint8_t)(width>>16);
	bmp_info_header[7]=(uint8_t)(width>>24);
	bmp_info_header[8]=(uint8_t)(height);
	bmp_info_header[9]=(uint8_t)(height>>8);
	bmp_info_header[10]=(uint8_t)(height>>16);
	bmp_info_header[11]=(uint8_t)(height>>24);
	bmp_info_header[12]=1;
	bmp_info_header[14]=24;
	bmp_info_header[20]=(uint8_t)(DataSize);
	bmp_info_header[21]=(uint8_t)(DataSize>>8);
	bmp_info_header[22]=(uint8_t)(DataSize>>16);
	bmp_info_header[23]=(uint8_t)(DataSize>>24);
	*/

//	BITMAP_INFO_HEADER bmpih;
	bmph.bmpih.biHeaderSize=sizeof(BITMAP_INFO_HEADER);
	bmph.bmpih.biWidth=width;
	bmph.bmpih.biHeight=height;
	bmph.bmpih.biPlanes=1;
	bmph.bmpih.biPerPix=24;
	bmph.bmpih.biCompression=0;//only for 16 or 32 Bits
	bmph.bmpih.biImgSize=DataSize;
	bmph.bmpih.biXPixPerMeter=0;
	bmph.bmpih.biYPixPerMeter=0;
	bmph.bmpih.biUsedClr=0;
	bmph.bmpih.biImportantClr=0;


	WriteBMPFile(bmph,"img.bmp");
	

	Color=CV_RGB(255,0,0);
	cvNamedWindow("image",1);
	cvSetMouseCallback("image",onMouse,&bmph);
	//Diasplay
	while(1)
	{
		//show image
		ori_ipimg=cvLoadImage("img.bmp",1);
		cvShowImage("image",ori_ipimg);
		
		char key_input=cvWaitKey(0);
		//do reverse
		if(key_input=='R' || key_input=='r')
		{
			ReadBMPFile(&bmph,"img.bmp");
			Reverse(bmph.bmpih.biWidth,bmph.bmpih.biHeight);
			WriteBMPFile(bmph,"img.bmp");
		}
		else if(key_input=='q' || key_input=='Q')
		{
			WriteBMPFile(bmph,"img.bmp");
			break;
		}
	}
	cvDestroyWindow("image");
	cvReleaseImage(&ori_ipimg);
	cvReleaseImage(&mod_ipimg);

	return 0;
}

