#pragma once
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
//#include <cstdio>
//#include <cmath>
#include<stdio.h>
#include<vector>
using namespace std;
typedef unsigned char uint8;
struct octNode
{
	long long cnt, rSum, gSum, bSum;
	bool isLeaf;
	bool mergeable;
	int depth;
	int childCount;

	octNode* child[8];

	//�޲ι��캯����ʵ����ֻ�ڸ��ڵ��õ���
	octNode():cnt(0),rSum(0),gSum(0),bSum(0),isLeaf(false),mergeable(true),depth(0),childCount(0){ 
		for (int i = 0; i < 8; i++)
			child[i] = NULL;
	}

	//���ι��캯�����Զ����ڵ�����
	octNode(int r, int g, int b, int depth)
		:cnt(1),rSum(r),gSum(g),bSum(b),isLeaf(false), mergeable(true),depth(depth),childCount(0){
		for (int i = 0; i < 8; i++)
			child[i] = NULL;
		if (depth == 8)
			isLeaf = true;
		else
			isLeaf = false;
	}

	//�ڵ������ӽڵ�
	bool hasChild() {
		for (int i = 0; i < 8; i++) {
			if (child[i] != NULL) {
				return true;
			}
		}
		return false;
	}

	//�ڵ�������ڵ�
	/*bool hasGrandChild() {
		octNode*Child = NULL;
		for (int i = 0; i < 8; i++) {
			if (child[i] != NULL) {
				return !child[i]->isLeaf;
			}
		}
		return false;
	}*/

	//�ӽڵ���Ŀ
	int ChildCount() {
		int res = 0;
		for (int i = 0; i < 8; i++) {
			if (child[i] != NULL)
				res++;
		}
		if (res != childCount)
			printf("111 res:%d childCount:%d\n",res,childCount);
		return res;
	}

	//���ӽڵ�ϲ������ڵ�
	void mergeChild() {
		
		for (int i = 0; i < 8; i++) {
			delete child[i];
			child[i] = NULL;
		}
		isLeaf = true;
		childCount=0;
	}

	//Ҷ�ڵ���Ŀ
	int leafCount() {
		return DFS(this);
	}
	int DFS(octNode*root) {
		if (root == NULL)return 0;
		if (root->isLeaf)return 1;
		int res = 0;
		for (int i = 0; i < 8; i++) {
			res += DFS(root->child[i]);
		}
		return res;
	}
};
class octTree
{
public:
	octTree() :root(new octNode),colors(0),maxColors(256){}
	octTree(int maxColorNum): root(new octNode),colors(0),maxColors(maxColorNum){}
	~octTree() {
		deleteTree(root);
	};
	void deleteTree(octNode* root);
	void insertColor(uint8 r, uint8 g, uint8 b);						//����һ����ɫ
	uint8 generatePalette(RGBQUAD *pal);								//���ɵ�ɫ��
	void DFS(octNode* root,RGBQUAD *pal, int &index);
	octNode* BFS(octNode*root);
private:
	octNode *root;														//�˲����ĸ�
	int colors;															//��ǰ����ɫ����
	int maxColors;														//�����ɫ��
	vector<octNode*>depthList[7];										//ÿ��ڵ㵥�����б��������������ɫ����
};
