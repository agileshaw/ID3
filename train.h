#include "rules.h"

#ifndef _TRAINH_
#define _TRAINH_

typedef struct TreeNode
{
	struct TreeNode* parent;
	Queue* children;
	int level;
	int index;
	int label;
	int total_rows;
	int* avail_rows;
	int total_attris;
	int* avail_attris;
	int attri_selected;
	double entropy;
} TreeNode;

RuleTree* buildTree(DataFrame* dataframe, int* features, double* cur_accuracy);

TreeNode* createRoot(DataFrame* dataframe);

TreeNode* createLeaf(DataFrame* dataframe, TreeNode* parent, int level, int index);

void expandTree(DataFrame* dataframe, TreeNode* node, RuleTree* rule);

void findBestAttribute (DataFrame* dataframe, TreeNode* node);

double currentEntropy(DataFrame* dataframe, TreeNode* node);

double getAttributeEntropy(DataFrame* dataframe, TreeNode* node, int i);

double calcEntropy(int n, int total);

int classLabel(DataFrame* dataframe, TreeNode* node, int* count);

int checkPure(DataFrame* dataframe, TreeNode* node);

double accuracy(DataFrame* dataframe, TreeNode* tree);

int truePrediction(DataFrame* dataframe, TreeNode* tree);

void printTree(DataFrame* dataframe, TreeNode* node, int* leaf, int* size);

void destroyTree(TreeNode* tree);

#endif
