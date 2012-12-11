#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
//#include <math.h>
#include <cstring>
/////////////////////////////WIKIPEDIA////////////////////////////////////////////
	typedef unsigned short WORD; //2b
	typedef unsigned int DWORD;  //4b
	typedef int LONG;

	typedef struct tagBITMAPFILEHEADER { 
  		WORD    bfType;        // тип файла
  		DWORD   bfSize;        // размер всего файла
  		WORD    bfReserved1; 	//reservred, zeros
  		WORD    bfReserved2; 	//reservred, zeros
	  	DWORD   bfOffBits;     // смещение от начала BITMAPFILEHEADER до начала изображения
	}	BITMAPFILEHEADER, *PBITMAPFILEHEADER;													//why????

	typedef struct tagBITMAPINFOHEADER{
		DWORD  biSize; //размер данной структуры в байтах(для определения версии bmp)
		LONG   biWidth; //ширина в пикселях
		LONG   biHeight; // высота
		WORD   biPlanes; //кол-во цветовых плоскостей, в bmp = 1
		WORD   biBitCount; //бит на пиксель. >=16 без палитры
		DWORD  biCompression; //сжатие, 0=без
		DWORD  biSizeImage; //
		LONG   biXPelsPerMeter; //пикселей на метр 
		LONG   biYPelsPerMeter; //пикселей на метр
		DWORD  biClrUsed; //тоже про палитру
		DWORD  biClrImportant; //и снова
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER;														//why?
/////////////////////////////WIKIPEDIA////////////////////////////////////////////

	struct pixel{
		signed char R;
		signed char G;
		signed char B;
	};
	typedef std::vector< std::vector<pixel> > matrix;

	struct parameter {
		std::string name;
		//consecutive number - to recognize incorrect order or make specify order of running
		int queue;
		bool isTrue;
	};
	typedef std::vector< parameter > parameters;

	void readBMP(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image, char &source_file_name) {
		FILE *input;
		input = fopen(&source_file_name,"rb");
		pixel onePix;
		int garbage;

		//header reading
		fread(&BITMAPFILEHEADER.bfType,2,1,input);
		fread(&BITMAPFILEHEADER.bfSize,4,1,input);
		fread(&BITMAPFILEHEADER.bfReserved1,2,1,input);
		fread(&BITMAPFILEHEADER.bfReserved2,2,1,input);
		fread(&BITMAPFILEHEADER.bfOffBits,4,1,input);

		fread(&BITMAPINFOHEADER.biSize,4,1,input);
		fread(&BITMAPINFOHEADER.biWidth,4,1,input);
		fread(&BITMAPINFOHEADER.biHeight,4,1,input);
		fread(&BITMAPINFOHEADER.biPlanes,2,1,input);
		fread(&BITMAPINFOHEADER.biBitCount,2,1,input);
		fread(&BITMAPINFOHEADER.biCompression,4,1,input);
		fread(&BITMAPINFOHEADER.biSizeImage,4,1,input);
		fread(&BITMAPINFOHEADER.biXPelsPerMeter,4,1,input);
		fread(&BITMAPINFOHEADER.biYPelsPerMeter,4,1,input);
		fread(&BITMAPINFOHEADER.biClrUsed,4,1,input);
		fread(&BITMAPINFOHEADER.biClrImportant,4,1,input);
		
		int garbageSize = (BITMAPINFOHEADER.biWidth % 4); ///strange, must be .biWidth 
		//image reading
		for(int j=0; j<BITMAPINFOHEADER.biHeight; ++j) {			//if 4o: построчно снизу-вверх
			image.push_back(std::vector<pixel>());
			for(int i=0; i<BITMAPINFOHEADER.biWidth; ++i) {
				fread(&onePix.B, 1, 1, input);
				fread(&onePix.G, 1, 1, input);
				fread(&onePix.R, 1, 1, input);
				image[j].push_back(onePix);
			};
			if ( garbageSize!= 0 ) {fread(&garbage, garbageSize, 1, input); }
		}
		fclose(input);	
	}

	void writeBMP(const tagBITMAPFILEHEADER &BITMAPFILEHEADER, const tagBITMAPINFOHEADER &BITMAPINFOHEADER, const matrix &image) {
		FILE *output;
		output = fopen("output.bmp","wb");
		int garbage = 0;
		int garbageSize = (BITMAPINFOHEADER.biWidth % 4); ///strange .biWidth
		
		fwrite(&BITMAPFILEHEADER.bfType,2,1,output);
		fwrite(&BITMAPFILEHEADER.bfSize,4,1,output);
		fwrite(&BITMAPFILEHEADER.bfReserved1,2,1,output);
		fwrite(&BITMAPFILEHEADER.bfReserved2,2,1,output);
		fwrite(&BITMAPFILEHEADER.bfOffBits,4,1,output);

		fwrite(&BITMAPINFOHEADER.biSize,4,1,output);
		fwrite(&BITMAPINFOHEADER.biWidth,4,1,output);
		fwrite(&BITMAPINFOHEADER.biHeight,4,1,output);
		fwrite(&BITMAPINFOHEADER.biPlanes,2,1,output);
		fwrite(&BITMAPINFOHEADER.biBitCount,2,1,output);
		fwrite(&BITMAPINFOHEADER.biCompression,4,1,output);
		fwrite(&BITMAPINFOHEADER.biSizeImage,4,1,output);
		fwrite(&BITMAPINFOHEADER.biXPelsPerMeter,4,1,output);
		fwrite(&BITMAPINFOHEADER.biYPelsPerMeter,4,1,output);
		fwrite(&BITMAPINFOHEADER.biClrUsed,4,1,output);
		fwrite(&BITMAPINFOHEADER.biClrImportant,4,1,output);

		for(int j=0; j<BITMAPINFOHEADER.biHeight; ++j){
			for(int i=0; i<BITMAPINFOHEADER.biWidth; ++i){
				fwrite(&image[j][i].B,1,1,output);
				fwrite(&image[j][i].G,1,1,output);
				fwrite(&image[j][i].R,1,1,output);
			};
			if ( garbageSize!= 0 ) {fwrite(&garbage, garbageSize,1,output); }
		}

		fclose(output);
	}

/////////////////////////////////////////FILTERS/////////////////////////////////////////
	void rgb100(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		for (int i=1; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				if (image[i][j].G<100) {image[i][j].G=0;}
				if (image[i][j].B<100) {image[i][j].B=0;}
				if (image[i][j].R<100) {image[i][j].R=0;}
			}
		}
	}

	void main_diagonal(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		double max =BITMAPINFOHEADER.biWidth*BITMAPINFOHEADER.biHeight;
		double k;
		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				k=i*j/max;
				image[i][j].R = image[i][j].R * (k);
				image[i][j].G = image[i][j].G * (k);
				image[i][j].B = image[i][j].B * (k);
			}
		}		
	}

	void negative(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image){
		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				image[i][j].R =255 - image[i][j].R;
				image[i][j].G =255 - image[i][j].G;
				image[i][j].B =255 - image[i][j].B;				
			}
		}		
	}

	//incrorrect using of k. it's not comparable with 0..255 size of palitra
	void frame(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		int k,l;
		int mI = BITMAPINFOHEADER.biHeight/2;
		int mJ = BITMAPINFOHEADER.biWidth/2;
		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				k = mI - abs(i);
				l = mJ - abs(j);
				//k = (k+j)/2;
				image[i][j].R -= k;
				image[i][j].G -= k;
				image[i][j].B -= k;				
			}
		}		
	}
/////////////////////////////////////////FILTERS/////////////////////////////////////////

	void initParameters(parameters &inputParameters) {
		//tmp
		parameter one;
		//number 0
		one.name = "negative";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 1
		one.name = "rgb100";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 2
		one.name = "diagonal";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 3
		one.name = "frame";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);	
	}

	void readParameters(const int argc, char **argv, parameters &inputParameters, char &source_file_name) {
		if (argc == 2) {
			strcpy(&source_file_name, argv[1]);
		}else {
			for (int i = 1; i < (argc-1); ++i) {
				if (strncmp(argv[i], "--negative", 10) == 0) {
					inputParameters[0].isTrue = true;
					inputParameters[0].queue = i;
				}if (strncmp(argv[i], "--rgb100",8) == 0 ) {
					inputParameters[1].isTrue = true;
					inputParameters[1].queue = i;
				}if (strncmp(argv[i], "--diagonal",8) == 0 ) {
					inputParameters[2].isTrue = true;
					inputParameters[2].queue = i;
				}if (strncmp(argv[i], "--frame",5) == 0 ) {
					inputParameters[3].isTrue = true;
					inputParameters[3].queue = i;
				}
			}strcpy(&source_file_name, argv[argc-1]);
		}	
	}

	//change for a true-order(/consecutive number/queue) runner
	void runFilters( parameters &inputParameters, tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		if (inputParameters[0].isTrue == true) { negative(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[1].isTrue == true) { rgb100(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[2].isTrue == true) { main_diagonal(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[3].isTrue == true) { frame(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }	
	}



int main(int argc, char **argv) {
	tagBITMAPFILEHEADER BITMAPFILEHEADER; 
	tagBITMAPINFOHEADER BITMAPINFOHEADER;
	matrix image;
	char source_file_name[100];
	parameters inputParameters;

	initParameters(inputParameters);
	readParameters(argc, argv, inputParameters, source_file_name[0]);

	//sort parameters by queue --> user can combine it as he want

	readBMP(BITMAPFILEHEADER, BITMAPINFOHEADER, image, source_file_name[0]);

	runFilters(inputParameters, BITMAPFILEHEADER, BITMAPINFOHEADER, image);

	writeBMP(BITMAPFILEHEADER, BITMAPINFOHEADER, image);
}