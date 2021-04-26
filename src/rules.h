#include "data.h"

#ifndef _RULESH_
#define _RULESH_

typedef struct RuleTree
{
	struct RuleTree* parent;
	Queue* children;
	int level;
	int index;
	int label;
	int attri_selected;
} RuleTree;

RuleTree* createRuleTree();

RuleTree* addRule(RuleTree* parent, int level, int index, int label, int attri);

void pruneRuleTree (RuleTree* tree);

void printRuleTree(DataFrame* dataframe, RuleTree* tree, int* leaf, int* size);

void printRuleTreeToFile(FILE* file, DataFrame* dataframe, RuleTree* tree, int* leaf, int* size);

void destroyRuleTree(RuleTree* tree);

#endif
