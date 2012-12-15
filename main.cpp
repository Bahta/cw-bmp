#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <cstring>

	typedef unsigned short WORD; //2b
	typedef unsigned int DWORD;  //4b
	typedef int LONG;

	 struct tagBITMAPFILEHEADER { 
  		WORD    bfType;        // file type
  		DWORD   bfSize;        // file size
  		WORD    bfReserved1; 	//reservred, zeros
  		WORD    bfReserved2; 	//reservred, zeros
	  	DWORD   bfOffBits;     // the offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found.
	};

	struct tagBITMAPINFOHEADER{
		DWORD  biSize; //the size of this header
		LONG   biWidth; //width, pixels
		LONG   biHeight; // height, pixels
		WORD   biPlanes; //the number of color planes being used. Must be set to 1.
		WORD   biBitCount; //bits per pixel >=16 without color table
		DWORD  biCompression; //compression, 0 --> without
		DWORD  biSizeImage; //the image size. This is the size of the raw bitmap data (see below), and should not be confused with the file size.
		LONG   biXPelsPerMeter; //horizontal, pixels per meter
		LONG   biYPelsPerMeter; //vertical, pixels per meter
		DWORD  biClrUsed; //the number of colors in the color palette, 2^n
		DWORD  biClrImportant; //the number of important colors used, or 0 when every color is important; generally ignored.
	};

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
		
		int garbageSize = (BITMAPINFOHEADER.biWidth % 4);
		//image reading
		for(int j=0; j<BITMAPINFOHEADER.biHeight; ++j) {
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
		int garbageSize = (BITMAPINFOHEADER.biWidth % 4);
		
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
		double mn = 1.0 * BITMAPINFOHEADER.biHeight / BITMAPINFOHEADER.biWidth;
		double z = sqrt(pow(mn,2) + 1);
		double dmax = abs(BITMAPINFOHEADER.biHeight) / z;
		double d;
		
		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				d = abs(mn*j -  i) / z;
				d =1 - (d/dmax); 
				image[i][j].R = image[i][j].R * (d);
				image[i][j].G = image[i][j].G * (d);
				image[i][j].B = image[i][j].B * (d);
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

	void frame(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		double k,l;
		int mI = BITMAPINFOHEADER.biHeight/2;
		int mJ = BITMAPINFOHEADER.biWidth/2;
		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				k = 1-1.0*abs(mI - i)/mI;
				l = 1-1.0*abs(mJ - j)/mJ;
				k = k*l;
				
				image[i][j].R *= k;
				image[i][j].G *= k;
				image[i][j].B *= k;				
			}
		}		
	}

	void circle(tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		int l;
		if (BITMAPINFOHEADER.biWidth > BITMAPINFOHEADER.biHeight) {l = BITMAPINFOHEADER.biHeight;} else {l = BITMAPINFOHEADER.biWidth;}
		double x = 1.0*l / 12;
		double r1 = x/2; 
		double d1 = x/2;
		double r2 = 2*x;
		double d2 = x;
		double r3 = 4.5*x;
		double d3 = 1.5*x;
		double dc;

		for (int i=0; i<BITMAPINFOHEADER.biHeight; ++i) {
			for (int j=0; j<BITMAPINFOHEADER.biWidth; ++j) {
				dc = sqrt ( pow(1.0*BITMAPINFOHEADER.biWidth/2 - j,2) + pow(1.0*BITMAPINFOHEADER.biHeight/2 - i,2) );
				if (dc < r3+d3) {
					if (dc < r2+d2) {
						if (dc < r1+d1) {
							dc = abs(r1-dc)/d1; 
							image[i][j].R *= dc;
						}else {
							dc = abs(r2-dc)/d2; 
							image[i][j].G *= dc;
						}	
					}else {
						dc = abs(r3-dc)/d3; 
						image[i][j].B *= dc;
					}
				} 
			}
		}			  
	}
/////////////////////////////////////////FILTERS/////////////////////////////////////////

	void initParameters(parameters &inputParameters) {
		//temporary file
		parameter one;
		//number 0: vacant
		one.name = "reserved";		
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 1
		one.name = "negative";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 2
		one.name = "rgb100";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 3
		one.name = "diagonal";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);
		//number 4
		one.name = "frame";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);	
		//number 5
		one.name = "circle";
		one.isTrue = false;
		one.queue = 0;
		inputParameters.push_back(one);	
	}

	void help() {
	//	std::cout << "help is so helpful" << '\n';
		std::cout << "How to start: <parameter1> [<parameter2>] <source file>" << '\n';
		std::cout << "-n or --negative.: negative filter" << '\n';
		std::cout << "--rgb100.........: filter, that eliminate pixels with brightness less than 100" << '\n';
		std::cout << "-d or --diagonal.: now weed out and blured pixels by distance from the main diagonal" << '\n';
		std::cout << "-f or --frame....: weed out and blured pixels by distance from" << '\n';
		std::cout << "-c or --circle...: three inner circles with r-g-b bluring " <<'\n';
	};

	void readParameters(const int argc, char **argv, parameters &inputParameters, char &source_file_name, bool &dontStopMeNow) {
		if (argc == 2) {
			strcpy(&source_file_name, argv[1]);
		}else {
			for (int i = 1; i < (argc-1); ++i) {
				if ((strncmp(argv[i], "--help", 6) == 0) || (strncmp(argv[i], "-h", 2) == 0)) {
					help();
					dontStopMeNow = false;
					break;
				}if ((strncmp(argv[i], "--negative", 10) == 0) || (strncmp(argv[i], "-n", 2) == 0)) {
					inputParameters[1].isTrue = true;
					inputParameters[1].queue = i;
				}if (strncmp(argv[i], "--rgb100",8) == 0 ) {
					inputParameters[2].isTrue = true;
					inputParameters[2].queue = i;
				}if ((strncmp(argv[i], "--diagonal",8) == 0 ) || (strncmp(argv[i], "-d", 2) == 0)) {
					inputParameters[3].isTrue = true;
					inputParameters[3].queue = i;
				}if ((strncmp(argv[i], "--frame",7) == 0 ) || (strncmp(argv[i], "-f", 2) == 0)) {
					inputParameters[4].isTrue = true;
					inputParameters[4].queue = i;
				}if ((strncmp(argv[i], "--circle",8) == 0 ) || (strncmp(argv[i], "-c", 2) == 0)) {
					inputParameters[5].isTrue = true;
					inputParameters[5].queue = i;
				}
			}strcpy(&source_file_name, argv[argc-1]);
		}	
	}

	//change for a true-order(/consecutive number/queue) runner
	int runFilters( parameters &inputParameters, tagBITMAPFILEHEADER &BITMAPFILEHEADER, tagBITMAPINFOHEADER &BITMAPINFOHEADER, matrix &image) {
		if (inputParameters[0].isTrue == true) { return 0; }
		if (inputParameters[1].isTrue == true) { negative(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[2].isTrue == true) { rgb100(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[3].isTrue == true) { main_diagonal(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }
		if (inputParameters[4].isTrue == true) { frame(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }	
		if (inputParameters[5].isTrue == true) { circle(BITMAPFILEHEADER, BITMAPINFOHEADER, image); }	
	}



int main(int argc, char **argv) {
	tagBITMAPFILEHEADER BITMAPFILEHEADER; 
	tagBITMAPINFOHEADER BITMAPINFOHEADER;
	matrix image;
	char source_file_name[100];
	parameters inputParameters;
	bool dontStopMeNow = true;//flag for help calling or incorrect parameters

	initParameters(inputParameters);
	readParameters(argc, argv, inputParameters, source_file_name[0], dontStopMeNow);

	//function: sort parameters by queue --> user can combine it as he want

	if (dontStopMeNow) {
		readBMP(BITMAPFILEHEADER, BITMAPINFOHEADER, image, source_file_name[0]);

		runFilters(inputParameters, BITMAPFILEHEADER, BITMAPINFOHEADER, image);

		writeBMP(BITMAPFILEHEADER, BITMAPINFOHEADER, image);
	}	
}