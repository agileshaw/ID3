#include "rules.h"

RuleTree* createRuleTree()
{
    return addRule(NULL, 0, 0, -1, 0);
}

RuleTree* addRule(RuleTree* parent, int level, int index, int label, int attri)
{
    RuleTree* node = malloc(sizeof(RuleTree));

	node->parent = parent;
	node->children = createQueue();
	node->level = level;
	node->index = index;
	node->label = label;
    node->attri_selected = attri;

    return node;
}

void printRuleTree(DataFrame* dataframe, RuleTree* tree, int* leaf, int* size)
{
    int i, j;
	Node* cur = tree->children->first;
	RuleTree* node;

    for (j = 0; j < tree->children->count; j++) {
		node = (RuleTree*)(cur->data);

		if (!(node->parent)) {
			cur = cur->next;
			node =(RuleTree*)(cur->data);
			j++;
			(*size)++;
		}

		for (i = 1; i < node->level; i++)
			printf("|   ");

		printf("%s = %s", dataframe->attributes[node->parent->attri_selected]->name, dataframe->attributes[node->parent->attri_selected]->values[node->index]);

		if (node->label != -1) {
			(*leaf)++;
			printf(": %s\n", dataframe->attributes[node->attri_selected]->values[node->label]);
		}
		else
			printf("\n");

		(*size)++;
		printRuleTree(dataframe, node, leaf, size);
		cur = cur->next;
	}
}

void printRuleTreeToFile(FILE* file, DataFrame* dataframe, RuleTree* tree, int* leaf, int* size)
{
    int i, j;
	Node* cur = tree->children->first;
	RuleTree* node;

    for (j = 0; j < tree->children->count; j++) {
		node = (RuleTree*)(cur->data);

		if (!(node->parent)) {
			cur = cur->next;
			node =(RuleTree*)(cur->data);
			j++;
			(*size)++;
		}

		for (i = 1; i < node->level; i++)
			fprintf(file, "|   ");

		fprintf(file, "%s = %s", dataframe->attributes[node->parent->attri_selected]->name, dataframe->attributes[node->parent->attri_selected]->values[node->index]);

		if (node->label != -1) {
			(*leaf)++;
			fprintf(file, ": %s\n", dataframe->attributes[node->attri_selected]->values[node->label]);
		}
		else
			fprintf(file, "\n");

		(*size)++;
		printRuleTreeToFile(file, dataframe, node, leaf, size);
		cur = cur->next;
	}
}

void destroyRuleTree(RuleTree* tree)
{
    if (tree == NULL)
		return;

	RuleTree* node;

	while ((node = dequeue(tree->children)) != NULL)
		destroyRuleTree(node);
	destroyQueue(tree->children);

    if (tree != NULL)
		free(tree);
}
