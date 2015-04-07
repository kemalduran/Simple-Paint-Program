#ifdef WIN32 
#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>

// szPathName : Specifies the pathname
// lpBits	 : Specifies the bitmap bits
// w	: Specifies the image width
// h	: Specifies the image height
bool static SaveImage(char* szPathName, void* lpBits, int w, int h)
{
	//Create a new file for writing
	FILE *pFile = fopen(szPathName, "wb");

	if (pFile == NULL){
		printf("file not found---\n");
		return false;
	}

	BITMAPINFOHEADER BMIH;

	BMIH.biSize = sizeof(BITMAPINFOHEADER);

	BMIH.biSizeImage = w * h * 3;

	// Create the bitmap for this OpenGL context

	BMIH.biSize = sizeof(BITMAPINFOHEADER);

	BMIH.biWidth = w;

	BMIH.biHeight = h;

	BMIH.biPlanes = 1;

	BMIH.biBitCount = 24;

	BMIH.biCompression = BI_RGB;

	BMIH.biSizeImage = w * h * 3;

	BITMAPFILEHEADER bmfh;

	int nBitsOffset = sizeof(BITMAPFILEHEADER) +BMIH.biSize;

	LONG lImageSize = BMIH.biSizeImage;

	LONG lFileSize = nBitsOffset + lImageSize;

	bmfh.bfType = 'B' + ('M' << 8);

	bmfh.bfOffBits = nBitsOffset;

	bmfh.bfSize = lFileSize;

	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	//Write the bitmap file header

	UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);

	//And then the bitmap info header

	UINT nWrittenInfoHeaderSize = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);

	//Finally, write the image data itself 

	//-- the data represents our drawing

	UINT nWrittenDIBDataSize = fwrite(lpBits, 1, lImageSize, pFile);

	fclose(pFile);
	return true;
}