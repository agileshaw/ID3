#include "train.h"

RuleTree* buildTree(DataFrame* dataframe, int* features, double* cur_accuracy)
{
	int i, count, num_leaf = 0, size = 1;
	TreeNode* root = createRoot(dataframe);
	RuleTree* ruletree = createRuleTree();

	printf("Total Attribute: %d\n", root->total_attris);
	for (i = 0; i < dataframe->maxsize; i++) {
		if (!strcmp(dataframe->dataset[i], "?") ||
			!strcmp(dataframe->dataset[i], " ") ||
			strlen(dataframe->dataset[i]) == 0){
		}
	}

	if (features) {
		for (i = 0; i < dataframe->col-1; i++) {
			if (features[i] == 0) {
				root->avail_attris[i] = 0;
				root->total_attris--;
			}
		}
	}

	if (root->total_attris != 0 && root->total_rows != 0 && checkPure(dataframe, root) == 0) {
		findBestAttribute(dataframe, root);
		ruletree->attri_selected = root->attri_selected;
		if (root->entropy == 0) {
			for (i = 0; i < dataframe->attributes[root->attri_selected]->num_values; i++) {
				TreeNode *leaf = createLeaf(dataframe, root, root->level+1, i);
				leaf->attri_selected = dataframe->col - 1;
				count = 0;
				leaf->label = classLabel(dataframe, leaf, &count);
				RuleTree *rule = addRule(ruletree, ruletree->level+1, i, leaf->label, root->attri_selected);
				enqueue(&(root->children), (void *)leaf);
				enqueue(&(ruletree->children), (void *)rule);
			}
		} else
			expandTree(dataframe, root, ruletree);
	} else {
		root->attri_selected = dataframe->col - 1;
		count = 0;
		root->label = classLabel(dataframe, root, &count);
		ruletree->attri_selected = root->attri_selected;
	}

	printf("\n----------------Decision Tree----------------\n");
	printTree(dataframe, root, &num_leaf, &size);
	printf("\nNumber of leaves: %d\n", num_leaf);
	printf("Size of the tree: %d\n\n", size);
	printf("Accuracy score: %f%%\n\n", accuracy(dataframe, root));

	if(cur_accuracy)
		*cur_accuracy = accuracy(dataframe, root);

	destroyTree(root);
	return ruletree;
}

TreeNode* createRoot(DataFrame* dataframe)
{
	return createLeaf(dataframe, NULL, 0, 0);
}

TreeNode* createLeaf(DataFrame* dataframe, TreeNode* parent, int level, int index)
{
	int i, j;
	TreeNode* node = malloc(sizeof(TreeNode));

	node->parent = parent;
	node->children = createQueue();
	node->level = level;
	node->index = index;
	node->label = -1;
	node->total_rows = 0;
	node->total_attris = 0;
	node->avail_attris = malloc(sizeof(int) * (dataframe->col-1));
	node->avail_rows = malloc(sizeof(int) * dataframe->row);

	for (i = 0; i < (dataframe->col-1); i++) {
		node->avail_attris[i] = 1;
		node->total_attris++;
	}
	for (j = 0; j < dataframe->row; j++) {
		node->avail_rows[j] = 1;
		node->total_rows++;
	}

	if (parent) {
		for (i = 0; i < (dataframe->col-1); i++) {
			if (parent->avail_attris[i] != 1 || parent->attri_selected == i) {
				node->avail_attris[i] = 0;
				node->total_attris--;
			}
		}
		for (j = 0; j < dataframe->row; j++) {
			if (parent->avail_rows[j] != 1 || strcmp(dataframe->dataset[j*dataframe->col+parent->attri_selected],
					dataframe->attributes[parent->attri_selected]->values[index])) {
					node->avail_rows[j] = 0;
					node->total_rows--;
			}
		}
	}

	return node;
}

void expandTree(DataFrame* dataframe, TreeNode* node, RuleTree* rule)
{
	int i, j, count;
	TreeNode* new_node;
	RuleTree* new_rule;

	for (j = 0; j < dataframe->attributes[node->attri_selected]->num_values; j++) {
		new_node = createLeaf(dataframe, node, node->level+1, j);
		if (new_node->total_attris > 0 && checkPure(dataframe, new_node) == 0) {
			findBestAttribute(dataframe, new_node);
			enqueue(&(node->children), (void *)new_node);
			new_rule = addRule(rule, rule->level+1, j, new_node->label, new_node->attri_selected);
			enqueue(&(rule->children), (void *)new_rule);

			if (new_node->entropy == 0) {
				for (i = 0; i < dataframe->attributes[new_node->attri_selected]->num_values; i++) {
					TreeNode *leaf = createLeaf(dataframe, new_node, new_node->level+1, i);
					leaf->attri_selected = dataframe->col - 1;
					count = 0;
					leaf->label = classLabel(dataframe, leaf, &count);
					RuleTree *leaf_rule = addRule(new_rule, new_rule->level+1, i, leaf->label, leaf->attri_selected);
					enqueue(&(new_node->children), (void *)leaf);
					enqueue(&(new_rule->children), (void *)leaf_rule);
				}

			} else
				expandTree(dataframe, new_node, new_rule);
		} else {
			new_node->attri_selected = dataframe->col - 1;
			count = 0;
			new_node->label = classLabel(dataframe, new_node, &count);
			enqueue(&(node->children), (void *)new_node);
			new_rule = addRule(rule, rule->level+1, j, new_node->label, new_node->attri_selected);
			enqueue(&(rule->children), (void *)new_rule);
		}
	}
}

void findBestAttribute (DataFrame* dataframe, TreeNode* node)
{
	int i, best = 0, first = 1;
	double cur_ent, child_ent, highest = 1;

	cur_ent = currentEntropy(dataframe, node);
	for (i = 0; i < (dataframe->col-1); i++) {
		if (node->avail_attris[i] == 1) {
			child_ent = getAttributeEntropy(dataframe, node, i);
			if (first) {
				highest = cur_ent - child_ent;
				best = i;
				first = 0;
			}
			//printf("Current info gain: %s - %f\n", dataframe->attributes[i]->name, cur_ent-child_ent);
			if ((cur_ent - child_ent) > highest) {
				highest = cur_ent - child_ent;
				best = i;
			}
		}
	}
	node->attri_selected = best;
	node->entropy = highest;
	//printf("Best Attribute to Split: %d - %f\n", best, highest);
}

double currentEntropy(DataFrame* dataframe, TreeNode* node)
{
	int j, target, total = 0, count = 0;
	double tmp = 0, ent = 0;

	target = dataframe->col-1;
	for (j = 0; j < dataframe->row; j++) {
		if (node->avail_rows[j] == 1) {
			total++;
			if (!strcmp(dataframe->dataset[j*dataframe->col+target], dataframe->attributes[target]->values[0]))
				count++;
		}
	}

	tmp += calcEntropy(count,total);
	tmp += calcEntropy((total-count), total);
	ent += ((double)total/(double)(node->total_rows)) * tmp;

	return ent;
}

double getAttributeEntropy(DataFrame* dataframe, TreeNode* node, int i)
{
	int j, k = 0, target, total = 0, count = 0;
	double tmp = 0, ent = 0;

	target = dataframe->col-1;
	while (k < dataframe->attributes[i]->num_values){
		for (j = 0; j < dataframe->row; j++) {
			if (node->avail_rows[j] == 1 && !strcmp(dataframe->dataset[j*dataframe->col+i], dataframe->attributes[i]->values[k])) {
				total++;
				if (!strcmp(dataframe->dataset[j*dataframe->col+target], dataframe->attributes[target]->values[0]))
					count++;
			}
		}
		tmp += calcEntropy(count,total);
		tmp += calcEntropy((total-count), total);
		ent += ((double)total/(double)(node->total_rows)) * tmp;
		total = 0;
		count = 0;
		tmp = 0;
		k++;
	}

	return ent;
}

double calcEntropy(int n, int total)
{
	double div = (double)n/(double)total;

	if (div <= 0 || div > 1 || total == 0)
		return 0;
	return (-div) * (log(div)/log(2));
}

int classLabel(DataFrame* dataframe, TreeNode* node, int* count)
{
	int j, total = 0;
	int target = dataframe->col-1;

	for (j = 0; j < dataframe->row; j++) {
		if (node->avail_rows[j] == 1) {
			total++;
			if (!strcmp(dataframe->dataset[j*dataframe->col+target], dataframe->attributes[target]->values[0]))
				(*count)++;
		}
	}

	if ((*count) >= ((double)total/(double)2))
		return 0;
	else
		return 1;
}

int checkPure(DataFrame* dataframe, TreeNode* node)
{
	int j, total = 0, count = 0;
	int target = dataframe->col-1;

	for (j = 0; j < dataframe->row; j++) {
		if (node->avail_rows[j] == 1) {
			//printf("Row: %d %s %s\n", i, dataframe->dataset[j*dataframe->col+target], dataframe->attributes[target]->values[k]);
			total++;
			if (!strcmp(dataframe->dataset[j*dataframe->col+target], dataframe->attributes[target]->values[0]))
				count++;
		}
	}

	if ((double)count/(double)total == 0 || (double)count/(double)total == 1)
		return 1;
	else
		return 0;
}

double accuracy(DataFrame* dataframe, TreeNode* tree)
{
	int count, total;

	total = tree->total_rows;
	count = truePrediction(dataframe, tree);

	//printf("Count: %d, Total: %d\n", count, total);

	if (total == 0)
		return 0;
	else
		return 100 * ((double)count/(double)total);
}

int truePrediction(DataFrame* dataframe, TreeNode* tree)
{
	int count = 0, i, j;
	int target = dataframe->col-1;
	Node* cur = tree->children->first;
	TreeNode* node;

	if (tree->label != -1) {
		for (i = 0; i < dataframe->row; i++) {
			if (tree->avail_rows[i] == 1 &&
				!strcmp(dataframe->attributes[target]->values[tree->label], dataframe->dataset[i*dataframe->col+target]))
				//printf("Row %d, Node Label: %s - Actual Label: %s\n", i, dataframe->attributes[target]->values[tree->label], dataframe->dataset[i*dataframe->col+target]);
				count++;
		}
	}

	for (j = 0; j < tree->children->count; j++) {
		node = (TreeNode*)(cur->data);
		count += truePrediction(dataframe, node);
		cur = cur->next;
	}

	return count;
}


void printTree(DataFrame* dataframe, TreeNode* tree, int* leaf, int* size)
{
	int i, j, count;
	Node* cur = tree->children->first;
	TreeNode* node;

	for (j = 0; j < tree->children->count; j++) {
		node = (TreeNode*)(cur->data);

		if (!(node->parent)) {
			cur = cur->next;
			node =(TreeNode*)(cur->data);
			j++;
			(*size)++;
		}

		for (i = 1; i < node->level; i++)
			printf("|   ");

		printf("%s = %s", dataframe->attributes[node->parent->attri_selected]->name, dataframe->attributes[node->parent->attri_selected]->values[node->index]);

		if (node->label != -1) {
			count = 0;
			classLabel(dataframe, node, &count);
			(*leaf)++;
			if (node->label == 0)
				printf(": %s (%d/%d)\n", dataframe->attributes[node->attri_selected]->values[node->label], node->total_rows, (node->total_rows - count));
			else
				printf(": %s (%d/%d)\n", dataframe->attributes[node->attri_selected]->values[node->label], node->total_rows, count);
		}
		else
			printf("\n");

		(*size)++;
		printTree(dataframe, node, leaf, size);
		cur = cur->next;
	}
}

void destroyTree(TreeNode* tree)
{
	if (tree == NULL)
		return;

	TreeNode* node;

	while ((node = dequeue(tree->children)) != NULL)
		destroyTree(node);
	destroyQueue(tree->children);

	if (tree != NULL) {
		if (tree->avail_rows != NULL)
			free(tree->avail_rows);
		if (tree->avail_attris != NULL)
			free(tree->avail_attris);
		free(tree);
	}
}
