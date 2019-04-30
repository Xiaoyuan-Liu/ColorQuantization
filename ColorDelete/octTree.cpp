#include"octTree.h"
#include<iostream>
using namespace std;
void octTree::insertColor(uint8 r, uint8 g, uint8 b) {
	octNode*parent = root;
	//char s[9];
	//itoa(r, s, 2);
	//printf("\nr:%s\n", s);
	//itoa(g, s, 2);
	//printf("g:%s\n", s);
	//itoa(b, s, 2);
	//printf("b:%s\n", s);
	//int currentDepth = 0;
	//插入一个颜色。
	for (int i = 0; (i < 8)&&(!parent->isLeaf); i++) {
		//RGB都是8位，因此深度为8（根为0），要生成8个子节点索引。
		//RGB按顺序放在索引的最高、中间、最低位
		int levelIndex = 0;
		if ((r&(1 << (7-i)))!=0)
			levelIndex |= 4;
		if ((g&(1 << (7 - i))) != 0) {
			levelIndex |= 2;
		}
		if ((b&(1 << (7 - i))) != 0) {
			levelIndex |= 1;
		}
		//itoa(levelIndex, s, 2);
		//printf("levelIndex:%s\n", s);
		//int levelIndex = (r&(1 << i)) >> (i - 2) | ((g&(1 << i)) >> (i - 1)) | ((b&(1 << i)) >> (i));
		if (parent->child[levelIndex] == NULL) {
			//如果索引指向的子节点还没有，就新建一个。并且要把这个节点放到该深度节点的列表中，这个列表用于加速后续删色速度
			octNode* newOctNode = new octNode(r,g,b,(i+1));
			parent->child[levelIndex] = newOctNode;
			parent->childCount++;
			if (i == 7)
				//出现了新的叶节点，意味着颜色数要增加了
				colors++;
			else
				//将节点放到该深度的节点列表中。因为叶节点肯定不需要进行合并，所以只储存非叶节点
				depthList[i].push_back(newOctNode);
			
		}
		parent->cnt++;
		parent->rSum += r;
		parent->gSum += g;
		parent->bSum += b;
		parent = parent->child[levelIndex];
	}
	//循环结束后意味着已经到达最深，此时parent指向的是叶节点。这一步实际上是早期遗留代码，在octNode结构体的带参构造函数octNode(int r, int g, int b, int depth)中已经自动根据深度对isLeaf属性进行赋值了
	parent->isLeaf = true;
}

//取色
void octTree::DFS(octNode* root, RGBQUAD* pal, int &index) {
	if (root->isLeaf) {
		pal[index].rgbBlue = root->bSum / root->cnt;
		pal[index].rgbGreen = root->gSum / root->cnt;
		pal[index].rgbRed = root->rSum / root->cnt;
		pal[index].rgbReserved = 0;
		index++;
	}
	else {
		for (int i = 0; i < 8; i++) {
			if (root->child[i] != NULL)
				DFS(root->child[i], pal, index);
		}
	}
}

//减色
octNode* octTree::BFS(octNode*root) {
	
	if (root->isLeaf||!root->mergeable) return NULL;
	else {
		octNode*mergeRoot = NULL;
		for (int i = 0; i < 8; i++) {
			if (root->child[i] != NULL) {
				if (mergeRoot == NULL) {
					mergeRoot = BFS(root->child[i]);
				}
				else {
					octNode*newMergeRoot = BFS(root->child[i]);
					if (((newMergeRoot != NULL) && (newMergeRoot->cnt < mergeRoot->cnt)))// || ((newMergeRoot != NULL) && (newMergeRoot->depth > mergeRoot->depth)))
						mergeRoot = newMergeRoot;
				}
			}
		}
		if (mergeRoot == NULL) {
			if (root->childCount > 1)
				return root;
			else {
				root->mergeable = false;
				//root->mergeChild();
			}
		}
		return mergeRoot;
	}
	/*
	if (root->isLeaf)return NULL;
	octNode*mergeRoot = NULL;
	for (int i = 0; i < 8; i++) {
		if ((root->child[i] != NULL) && (root->child[i]->mergeable)) {
			if (mergeRoot == NULL) {
				mergeRoot = BFS(root->child[i]);
			}
			else {
				octNode*newMergeRoot = BFS(root->child[i]);
				if (((newMergeRoot != NULL) && (newMergeRoot->cnt < mergeRoot->cnt)))// || ((newMergeRoot != NULL) && (newMergeRoot->depth > mergeRoot->depth)))
					mergeRoot = newMergeRoot;
			}
		}
	}
	//cout << root->childCount;
	if (mergeRoot == NULL) {
		if (root->childCount > 1)
			mergeRoot = root;
		else
			root->mergeable = false;
	}
	//if (mergeRoot == NULL && (root->childCount > 1))
	//	return root;
	//else if (mergeRoot == NULL) {
	//	root->mergeable = false;
		//root->mergeChild();
	//}
	return mergeRoot;
	*/
}
uint8 octTree::generatePalette(RGBQUAD *pal) {
	//cout << root->leafCount();
	while (colors > maxColors) {
		cout << "color:" << colors << endl;
		octNode* minParent = NULL;
		long long minParentIndex=0;
		for (int i = 6; i >= 0; i--) {
			//cout << depthList[i].size();

			for (long long j = 0; j < depthList[i].size(); j++) {
				octNode*newMinParent = BFS(depthList[i][j]);
				if (newMinParent == NULL) {
					depthList[i].erase(depthList[i].begin() + j);
					minParentIndex--;
				}
				else {
					if (minParent == NULL) {
						minParent = newMinParent;
						minParentIndex = j;
					}
					else {
						if(newMinParent->cnt < minParent->cnt) {
							minParent = newMinParent;
							minParentIndex = j;
						}
					}
				}
				if (minParent != NULL) {
					colors = colors - minParent->leafCount() + 1;
					minParent->mergeChild();
					minParent->isLeaf = true;
					//depthList[i].erase(depthList[i].begin() + minParentIndex);
					break;
				}
			}
		}
	}
	/*
	while (colors>maxColors) {
		//cout << "color:" << colors << endl;
		octNode* minParent = BFS(root);
		//cout << "1" << endl;
		if (minParent != NULL) {
			colors = colors - minParent->leafCount() + 1;
			minParent->mergeChild();
			minParent->isLeaf = true;
		}
		else
			break;
	}*/
	cout << "color:" << colors << endl;
	//pal = new RGBQUAD[maxColors];
	int index = 0;
	cout << root->leafCount();
	DFS(root, pal, index);
	cout << "total size" <<index <<endl;
	for (int i = index; i < 256; i++) {
		pal[i].rgbBlue = 255;
		pal[i].rgbGreen = 255;
		pal[i].rgbRed = 255;
		pal[i].rgbReserved = 0;
	}
	/*
	for (int i = 0; i < 256; i++) {
		cout << "No."<<i<<" r: " << (int)pal[i].rgbRed << " g:" << (int)pal[i].rgbGreen << " b:" << (int)pal[i].rgbBlue << endl;
	}*/
	return 0; 
}
void octTree::deleteTree(octNode* root) {
	if (root == NULL)return;
	else {
		for (int i = 0; i < 8; i++) {
			deleteTree(root->child[i]);
		}
		delete root;
	}
}