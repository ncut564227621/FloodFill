
/*
Target:
	to try out the flood fill algorithm, which looks like some kind of a BFS. 
	
Date: 2013/11/3. 
*/ 

#include <cstdio>
#include <iostream>
#include <string>
#include <queue>		    

namespace BMPinC
{
// These two loading and writing functions are copied from internet, modified a little bit. thx.

unsigned char* ReadBMP(const char* filename, unsigned int &width, unsigned int &height)
{
	int i;
	FILE* fHandle = fopen(filename, "rb");

	if(fHandle == NULL)
	{
		// during IDE debugging, file may not be found, try to use full file path.
		throw "Argument Exception";
	}

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, fHandle); // read the 54-byte header

	// extract image height and width from header
	width = *(int*)&info[18];
	height = *(int*)&info[22];
	int bits = *(short*)&info[28]; // bits per pixel

	std::cout << std::endl;
	std::cout << "  Name: " << filename << std::endl;
	std::cout << " Width: " << width << std::endl;
	std::cout << "Height: " << height << std::endl;
	std::cout << "  Bits: " << bits << std::endl;

	if (bits != 24)
	{
		std::cout << "Sorry, do not support this file, bits != 24." << std::endl; 
		return NULL;
	}

	int row_padded = (width*3 + 3) & (~3);	// cjren, make sure the (row's width after padded % 4) == 0.  
	// the other way to do this:
	int iOldLineWidth = width * 3;
	int iPadding = 0;
	while ((iOldLineWidth + iPadding) % 4 != 0)
		++iPadding;
	int iNewLineWidth = iOldLineWidth + iPadding; 
	if (row_padded != iNewLineWidth)
	{
		return 0; 
	} 	 
	 
	// to read the data once
	unsigned char *dataBufferAll = NULL; // for C, NULL is (void *) rather than 0 in C++. 
	dataBufferAll = (unsigned char *)malloc(width * height * 3);
	memset(dataBufferAll, 0, sizeof(dataBufferAll));
	for (int i = 0; i < height; ++i)
	{
		unsigned char *dataBufferRow = NULL;
		dataBufferRow = (unsigned char *)malloc(row_padded); // one row with padding.
		fread(dataBufferRow, sizeof(unsigned char), row_padded, fHandle); 
		unsigned char tmp; 
		for (int j = 0; j < width * 3; j += 3) //some padding bits may be ignored.
		{
			dataBufferAll[i * width * 3 + j + 0] = dataBufferRow[j + 2];
			dataBufferAll[i * width * 3 + j + 1] = dataBufferRow[j + 1];
			dataBufferAll[i * width * 3 + j + 2] = dataBufferRow[j];
			
			//std::cout << "RGB" << (int)dataBufferRow[j + 2] << ", " << (int)dataBufferRow[j + 1] << ", " << (int)dataBufferRow[j] << std::endl;
		} 
	} 
	std::cout << "sizeof data buffer: " << sizeof(dataBufferAll) << std::endl;

	fclose(fHandle);
	return dataBufferAll;
}
bool WriteBMP(const char*filename, unsigned int w, unsigned int h, unsigned char *pRGB)
{
	FILE *fHandle;
	int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
	unsigned char *img = NULL;
	if( img )
		free( img );
	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,sizeof(img));   

	for(int y = 0; y < h; ++y)
	{
		for(int x = 0; x < w; ++x)
		{
			img[(x+y*w)*3+2] = (unsigned char)(pRGB[(y * w + x) * 3]);
			img[(x+y*w)*3+1] = (unsigned char)(pRGB[(y * w + x) * 3 + 1]);
			img[(x+y*w)*3+0] = (unsigned char)(pRGB[(y * w + x) * 3 + 2]);
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	fHandle = fopen(filename,"wb");
	fwrite(bmpfileheader,1,14,fHandle);
	fwrite(bmpinfoheader,1,40,fHandle);
	for(int i = h-1; i >=0; --i)  // for (int i = 0; i < h; ++i) upside down.
	{
		fwrite(img+(w*(h-i-1)*3),3,w,fHandle);
		fwrite(bmppad,1,(4-(w*3)%4)%4,fHandle);
	}
	fclose(fHandle);
	return true;
}
}; 


void printHelpInfo(const char * exeFile) {
	printf("Usage: %s Steps\n\n", exeFile);
	printf("Steps: \n");
	printf("  -1    : run to finish the filling.\n");
	printf("  >= 0  : run this num steps before stop.\n");
}

int parseCLArgs(int argc, char ** argv) {

	if( argc < 2 ) {
		printHelpInfo(argv[0]);
		exit(EXIT_FAILURE);
	}

	std::string sSteps = argv[1];
	int iSteps = std::stoi(sSteps);
	if( iSteps >= -1 ) {
		printf("Steps to run: %d.\n", iSteps);
	} else {
		printf("Unknown value: %s\n", sSteps.c_str());
		printHelpInfo(argv[0]);
		exit(EXIT_FAILURE);
	}

	return iSteps;
}


int main(int argc, char *argv[])
{					   
	std::string sFileName = "G:\\Ex\\cjren_tech_blog\\2013-10-20_Shape_to_fill.bmp"; 
	//std::string sFileName = "G:\\Ex\\cjren_tech_blog\\brick1.bmp";
	unsigned int iW = 0; 
	unsigned int iH = 0; 
	unsigned char* data = BMPinC::ReadBMP(sFileName.c_str(), iW, iH); //BMPReader::load(sFileName, iW, iH); //
	if (!data)
	{
	}
	else 
	{
		std::cout << "after successfully reading the file " << sFileName << std::endl;

		// starting seed.
		unsigned int iStartX = iW / 2.0f; 
		unsigned int iStartY = iH / 2.0f;
		
		unsigned char *visited = new unsigned char[iW * iH];
		memset(visited, 0, iW * iH);

		std::queue<unsigned int> aX;
		std::queue<unsigned int> aY;
		aX.push(iStartX); aY.push(iStartY); 
		visited[iStartX + iStartY * iW] = 1;

		const int iBitsPerRow = iW * 3; // three channels per pixel.
		const int iSteps = parseCLArgs(argc, argv);
		int iTmpSteps = (iSteps == -1 ? iW * iH : iSteps); 
		while (aX.size() && iTmpSteps)
		{
			unsigned int iX = aX.front(); aX.pop();
			unsigned int iY = aY.front(); aY.pop();
			data[iBitsPerRow * iY + iX * 3] = 255;
			data[iBitsPerRow * iY + iX * 3 + 1] = 0;
			data[iBitsPerRow * iY + iX * 3 + 2] = 0;			

			// to check the four adjacent pixels, if == white(255, 255, 255)
			// then move forward.
			if (data[iBitsPerRow * iY + iX * 3 - 3] == 255 && 
				data[iBitsPerRow * iY + iX * 3 - 2] == 255 && 
				data[iBitsPerRow * iY + iX * 3 - 1] == 255 &&
				visited[iX - 1 + iW * iY] == 0)
			{
				aX.push(iX - 1);
				aY.push(iY);
				visited[iX - 1 + iW * iY] = 1;
			} 
			if (data[iBitsPerRow * iY + (iX + 1) * 3 + 0] == 255 && 
				data[iBitsPerRow * iY + (iX + 1) * 3 + 1] == 255 && 
				data[iBitsPerRow * iY + (iX + 1) * 3 + 2] == 255 &&
				visited[iX + 1 + iW * iY] == 0)
			{
				aX.push(iX + 1);
				aY.push(iY);
				visited[iX + 1 + iW * iY] = 1;
			} 
			if (data[iBitsPerRow * (iY - 1) + (iX + 0) * 3 + 0] == 255 && 
				data[iBitsPerRow * (iY - 1) + (iX + 0) * 3 + 1] == 255 && 
				data[iBitsPerRow * (iY - 1) + (iX + 0) * 3 + 2] == 255 &&
				visited[iX + iW * (iY - 1)] == 0)
			{
				aX.push(iX + 0);
				aY.push(iY - 1);
				visited[iX + iW * (iY - 1)] = 1;
			} 
			if (data[iBitsPerRow * (iY + 1) + (iX + 0) * 3 + 0] == 255 && 
				data[iBitsPerRow * (iY + 1) + (iX + 0) * 3 + 1] == 255 && 
				data[iBitsPerRow * (iY + 1) + (iX + 0) * 3 + 2] == 255 &&
				visited[iX + iW * (iY + 1)] == 0)
			{
				aX.push(iX + 0);
				aY.push(iY + 1);
				visited[iX + iW * (iY + 1)] = 1; 
			} 
				 
		}

		std::cout << "after processing, starting to write bmp file." << std::endl;
		std::string sOut = "flood_fill_out_s";
		sOut += std::to_string(iSteps);
		sOut += ".bmp";
		std::cout << "out file: " << sOut << std::endl;
		BMPinC::WriteBMP(sOut.c_str(), iW, iH, data); 
	}
	std::cout << "ok, end." << std::endl;
	return 1; 
} 
