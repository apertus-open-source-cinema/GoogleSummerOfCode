#include"lodepng.h"
#include<iostream>
#include<stdio.h>
#include<fstream>
#include<vector>
#define BUF_SIZE 16;

// COMPILATION g++ Main.cpp lodepng.cpp -std=c++11

//program call ./a.out filename.raw12 [-g or greyscale] in std run the ouput png file will be rgb

using namespace std;


void encoding(const char * inputFileName, const string type);

int main(int argc, char ** argv){
	
	string type;
	if(argc>2) type=argv[2];
	cout<<!type.compare("-g")<<endl;
	encoding(argv[1],type);
	
  cout<<endl;
}

void encoding(const char * inputFileName,const string type){
	
	const unsigned rawBitDPTH=12;
	const float rawByteDPTH=rawBitDPTH/8.0;
	const unsigned COL=4096;
	const unsigned ROW=3072;
	const unsigned col12bit=(unsigned)COL*rawByteDPTH;
	const unsigned col16bit=COL*2;

	const unsigned totalSpace=(COL*ROW*rawByteDPTH);
	const unsigned totalSpaceForPNG=(COL*ROW/4); //2048*1536*1 byte=2048*3072/4

	LodePNGColorType colorType;
	char filename[]="test.png";
	unsigned char * firstRowData=new unsigned char[col16bit];
	unsigned char * secondRowData=new unsigned char[col16bit];
	unsigned char * image;
	if(!type.compare("grey") || !type.compare("-g")){
		colorType=LCT_GREY;
		image=new unsigned char[totalSpaceForPNG]();
		} 	
	else{
		colorType=LCT_RGB;
		image=new unsigned char[totalSpaceForPNG*3]();	
	}
	
	unsigned char readedVal1;
	unsigned char readedVal2;
	unsigned char readedVal3;
	fstream file;
	
	if(inputFileName!= NULL ){ 
		file.open(inputFileName, ios::in | ios::binary);
	}else{ 
		file.open("test.raw12", ios::in | ios::binary);
	}
	for(int i=0,rawOffSet=0;i<ROW;i++){
		for(int j=0, offSet=0;j<col12bit;j+=3){
			file.read((char*)(&readedVal1),1);
			file.read((char*)(&readedVal2),1);
			file.read((char*)(&readedVal3),1);
			//int pos=(i*col16bit)+(j+offSet);
			int pos=j+offSet;
			if(i%2==0){
				firstRowData[pos]=readedVal1;
				firstRowData[pos+1]=readedVal2 & 0xF0;
				firstRowData[pos+2]=((readedVal2 & 0x0F) << 4) | ((readedVal3 & 0xF0) >> 4 );
				firstRowData[pos+3]=(readedVal3 & 0x0F)<<4;
			
			}
			if(i%2==1){
				secondRowData[pos]=readedVal1;
				secondRowData[pos+1]=readedVal2 & 0xF0;
				secondRowData[pos+2]=((readedVal2 & 0x0F) << 4) | ((readedVal3 & 0xF0) >> 4 );
				secondRowData[pos+3]=(readedVal3 & 0x0F)<<4;
				
				if(!type.compare("grey") || !type.compare("-g")){	
					//unsigned char average=(firstRowData[pos]+firstRowData[pos+2]+secondRowData[pos]+secondRowData[pos+2])/4; // not using rawOffset+1 and rawOffset+3 cause conversion from 16 bit to 8 bit
					unsigned char average=(firstRowData[pos+2]);//green 1 color selected
					image[rawOffSet]= average; //conversion from 16 bit to 8 bit alreay done	
					if(rawOffSet==totalSpaceForPNG) cout<<"error";
					rawOffSet++;
				}
				else{
					char16_t green2byte=( ( (firstRowData[pos+2] << 8) | (firstRowData[pos+3] & 0x00FF) ) + ( (secondRowData[pos]<<8) | (firstRowData[pos] & 0x00FF) ) )/2;
					unsigned char green= (unsigned char)(green2byte >> 8);
					image[rawOffSet]=firstRowData[pos];
					image[rawOffSet+1]=green;
					image[rawOffSet+2]=secondRowData[pos+2];	
					if(rawOffSet==totalSpaceForPNG*3) cout<<"error";
					rawOffSet+=3;
				}
			}
			offSet++;
		}
	}
	unsigned error=lodepng_encode_file(filename,image,COL/2,ROW/2,colorType,8);		
	cout<<lodepng_error_text(error);
		
	delete[] firstRowData;
	delete[] secondRowData;
	delete[] image;
}
