/*
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <cstdio>
#include <cmath>

typedef unsigned char uint8;

struct octNode
{
	long long cnt, rSum, gSum, bSum;
	bool isLeaf;
	int depth;
	octNode* child[8];
};

class octTree
{
public:
	octTree() {};
	octTree(int maxColorNum) { maxColors = maxColorNum; }
	~octTree() {};
	void insertColor(uint8 r, uint8 g, uint8 b) {};						//����һ����ɫ
	uint8 generatePalette(RGBQUAD *pal) { return 0; }						//���ɵ�ɫ��
private:
	octNode *root;														//�˲����ĸ�
	int colors;															//��ǰ����ɫ����
	int maxColors;														//�����ɫ��
};


//�ӵ�ɫ����ѡ���������ɫ��ӽ�����ɫ
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD *pal)
{
	return (uint8)0;//����ĳ��ɫ���������ڵ�ɫ�����������ɫ������ֵ��
}
*/
#include <cstdio>
#include <cmath>
#include<iostream>
using namespace std;
#include"octTree.h"

//�ӵ�ɫ����ѡ���������ɫ��ӽ�����ɫ
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD *pal)
{
	int suitableIndex = 0;
	int distance = abs(r-pal[0].rgbRed) + abs(g-pal[0].rgbGreen) + abs(b-pal[0].rgbBlue);
	for (int i = 1; i < 256; i++) {
		double newDistance= abs(r - pal[i].rgbRed) + abs(g - pal[i].rgbGreen) + abs(b - pal[i].rgbBlue);
		if (newDistance < distance) {
			suitableIndex = i;
			distance = newDistance;
		}
	}
	//printf("Choose a new color! Distance:%d\tR:%d\tG:%d\tB:%d\tBoardR:%d\tBoardG:%d\tBoardB:%d\n",distance,r,g,b,pal[suitableIndex].rgbRed, pal[suitableIndex].rgbGreen, pal[suitableIndex].rgbBlue);
	return (uint8)suitableIndex;//����ĳ��ɫ���������ڵ�ɫ�����������ɫ������ֵ��
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("using: exe[0], input file[1], output file[2]\n");
		return -1;

	}
	BITMAPFILEHEADER bf, *pbf;//���롢����ļ����ļ�ͷ
	BITMAPINFOHEADER bi, *pbi;//���롢����ļ�����Ϣͷ
	RGBQUAD *pRGBQuad;//�����ɵĵ�ɫ��ָ��
	uint8 *pImage;//ת�����ͼ������
	DWORD bfSize;//�ļ���С
	LONG biWidth, biHeight;//ͼ���ȡ��߶�
	DWORD biSizeImage;//ͼ��Ĵ�С�����ֽ�Ϊ��λ��ÿ���ֽ���������4��������
	unsigned long biFullWidth;//ÿ���ֽ���������4��������

							  //�������ļ�
	char *inputName, *outputName;
	FILE *fpIn, *fpOut;
	inputName = argv[1];
	outputName = argv[2];
	//inputName = "6.bmp";
	//outputName = "666666.bmp";
	printf("Opening %s ... ", inputName);
	if (!(fpIn = fopen(inputName, "rb")))
	{
		printf("\nCan't open %s!\n", inputName);
		return -1;
	}
	printf("Success!\n");

	//��������ļ�
	printf("Creating %s ... ", outputName);
	if (!(fpOut = fopen(outputName, "wb")))
	{
		printf("\nCan't create %s!\n", outputName);
		return -1;
	}
	printf("Success!\n");

	//��ȡ�����ļ����ļ�ͷ����Ϣͷ
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fpIn);

	//��ȡ�ļ���Ϣ
	biWidth = bi.biWidth;
	biHeight = bi.biHeight;
	biFullWidth = ceil(biWidth / 4.) * 4;//bmp�ļ�ÿһ�е��ֽ���������4��������
	biSizeImage = biFullWidth*biHeight;
	bfSize = biFullWidth*biHeight + 54 + 256 * 4;//ͼ���ļ��Ĵ�С�������ļ�ͷ����Ϣͷ

												 //��������ļ���BITMAPFILEHEADER
	pbf = new BITMAPFILEHEADER;
	pbf->bfType = 19778;
	pbf->bfSize = bfSize;
	pbf->bfReserved1 = 0;
	pbf->bfReserved2 = 0;
	pbf->bfOffBits = 54 + 256 * 4;
	//д��BITMAPFILEHEADER
	if (fwrite(pbf, sizeof(BITMAPFILEHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap file header!\n");
		fclose(fpOut);
		return -1;
	}

	//��������ļ���BITMAPINFOHEADER
	pbi = new BITMAPINFOHEADER;
	pbi->biSize = 40;
	pbi->biWidth = biWidth;
	pbi->biHeight = biHeight;
	pbi->biPlanes = 1;
	pbi->biBitCount = 8;
	pbi->biCompression = 0;
	pbi->biSizeImage = biSizeImage;
	pbi->biXPelsPerMeter = 0;
	pbi->biYPelsPerMeter = 0;
	pbi->biClrUsed = 0;
	pbi->biClrImportant = 0;
	//д��BITMAPFILEHEADER
	if (fwrite(pbi, sizeof(BITMAPINFOHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap info header!\n");
		fclose(fpOut);
		return -1;
	}

	//������ɫ�˲���
	printf("Building Color OctTree ...  ");
	octTree *tree;
	tree = new octTree(256);
	uint8 RGB[3];
	//��ȡͼ����ÿ�����ص���ɫ�������������ɫ�˲���
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i*ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ�������������ɫ�˲���
			fread(&RGB, 3, 1, fpIn);
			tree->insertColor(RGB[2], RGB[1], RGB[0]);
		}
	}
	printf("Success!\n");

	//���ɲ�����ɫ��
	printf("Generating palette ... ");
	pRGBQuad = new RGBQUAD[256];
	tree->generatePalette(pRGBQuad);
	//���256ɫ��ɫ��
	if (fwrite(pRGBQuad, 256 * sizeof(RGBQUAD), 1, fpOut) != 1)
	{
		printf("\nCan't write palette!\n");
		fclose(fpOut);
		return -1;
	}
	printf("Success!\n");

	//���ͼ������
	printf("Generating the output image ... ");
	pImage = new uint8[biSizeImage];
	memset(pImage, 0, biSizeImage);
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i*ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ��������ת��λ��ɫ����ֵ
			fread(&RGB, 3, 1, fpIn);
			pImage[i*biFullWidth + j] = selectClosestColor(RGB[2], RGB[1], RGB[0], pRGBQuad);
		}
	}
	//���ͼ������
	if (fwrite(pImage, biSizeImage, 1, fpOut) != 1)
	{
		printf("\nCan't write image data!\n");
		fclose(fpOut);

		return -1;
	}
	printf("Success!\n");


	delete tree;
	delete pbf;
	delete pbi;
	delete[] pRGBQuad;
	delete[] pImage;
	fclose(fpIn);
	fclose(fpOut);
	printf("All done!\n");
	return 0;
}