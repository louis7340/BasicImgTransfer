#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

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
//total 54 Bytes


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
	printf("read head over\n");
	printf("width=%d height=%d\n",width,height);

	int i;
	int N=width*height;
	int DataSize=N*3;
	int file_size=54+DataSize;	//54 for File Header & Info Header
	FILE *fw;
	uint8_t *img=NULL;
	
	img=(uint8_t *)malloc(3*N);
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
	BITMAP_FILE_HEADER bmpfh;
	bmpfh.bfType=0x4d42; // 'BM'
	bmpfh.bfSize=file_size;
	bmpfh.reserved=0;
	bmpfh.bfOffSet=sizeof(BITMAP_FILE_HEADER)+sizeof(BITMAP_INFO_HEADER);//54?

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

	BITMAP_INFO_HEADER bmpih;
	bmpih.biHeaderSize=sizeof(BITMAP_INFO_HEADER);
	bmpih.biWidth=width;
	bmpih.biHeight=height;
	bmpih.biPlanes=1;
	bmpih.biPerPix=24;
	bmpih.biCompression=0;//only for 16 or 32 Bits
	bmpih.biImgSize=DataSize;
	bmpih.biXPixPerMeter=0;
	bmpih.biYPixPerMeter=0;
	bmpih.biUsedClr=0;
	bmpih.biImportantClr=0;

	/*
	   Write File include headers and RGB data
	  */
	fw=fopen("img.bmp","wb");
	//write headers
	fwrite(&bmpfh,1,14,fw);
	fwrite(&bmpih,1,40,fw);
//	fwrite(bmp_file_header,1,14,fw);
//	fwrite(bmp_info_header,1,40,fw);
	printf("FILE HEADER=%lu INFO HEADER=%lu\n",sizeof(bmpfh),sizeof(bmpih));	
	
	//padding bits
	uint8_t pad[3]={0,0,0};

//	fwrite(img,1,height*width*3,fw);
	for(i=0;i<height;i++)
	{
		fwrite(img+(width*i*3),3,width,fw);
		//padding bits
		fwrite(pad,1,(4-(width*3)%4)%4,fw);
	}
	fclose(fw);
}

