//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 5
		 Implementing 'remove node' operation for BST*/
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _bstnode{
	int item;
	struct _bstnode *left;
	struct _bstnode *right;
} BSTNode;   // You should not change the definition of BSTNode

typedef struct _stackNode{
	BSTNode *data;
	struct _stackNode *next;
}StackNode; // You should not change the definition of StackNode

typedef struct _stack
{
	StackNode *top;
}Stack; // You should not change the definition of Stack

///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototypes of these functions
void postOrderIterativeS2(BSTNode *root);

void insertBSTNode(BSTNode **node, int value);

void push(Stack *stack, BSTNode *node);
BSTNode *pop(Stack *s);
BSTNode *peek(Stack *s);
int isEmpty(Stack *s);
void removeAll(BSTNode **node);
BSTNode* removeNodeFromTree(BSTNode *root, int value);

void printSimpleTree(BSTNode *root, int level, char side)
{
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("  "); // 들여쓰기

    if (side == 'L')
        printf("L-");
    else if (side == 'R')
        printf("R-");

    printf("%d\n", root->item);

    printSimpleTree(root->left, level + 1, 'L');
    printSimpleTree(root->right, level + 1, 'R');
}

///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	//Initialize the Binary Search Tree as an empty Binary Search Tree
	BSTNode * root;
	root = NULL;

	printf("1: Insert an integer into the binary search tree;\n");
	printf("2: Print the post-order traversal of the binary search tree;\n");
	printf("0: Quit;\n");


	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to insert into the Binary Search Tree: ");
			scanf("%d", &i);
			insertBSTNode(&root, i);
			break;
		case 2:
			printf("The resulting post-order traversal of the binary search tree is: ");
			postOrderIterativeS2(root); // You need to code this function
			printf("\n");
			break;
		case 3:
    		printf("Current tree structure:\n");
    		printSimpleTree(root, 0, ' ');
    		break;
		case 4:
    		printf("삭제하고 싶은 노드를 골라라: ");
			scanf("%d", &i);
    		root = removeNodeFromTree(root, i);
    		break;
		case 0:
			removeAll(&root);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}

	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

void postOrderIterativeS2(BSTNode *root)
{
	 BSTNode * cur;

	Stack s1;
	Stack s2;
	s1.top = NULL;
	s2.top = NULL;
	if (root == NULL)
	{
		printf("empty");
		return;
	}
	push(&s1, root);
	while(s1.top != NULL)
	{
		cur = pop(&s1);
		push(&s2, cur);
		if (cur->left != NULL)
			push(&s1, cur->left);
		if (cur->right != NULL)
			push(&s1, cur->right);
	}
	while (s2.top != NULL)
	{
		cur = pop(&s2);
		printf("%d ", cur->item);
	}
}

/* Given a binary search tree and a key, this function
   deletes the key and returns the new root. Make recursive function. */

BSTNode	*find_inorder_successor(BSTNode *node)
{
	BSTNode *cur = node;

	while (cur != NULL && cur->left != NULL)
	{
		cur = cur->left;
	}
	return cur;
}

BSTNode	*removeNodeFromTree(BSTNode *root, int value)
{
	if (root == NULL)
		return NULL;
	if (value < root->item)
		root->left = removeNodeFromTree(root->left, value);
	else if (value > root->item)
		root->right = removeNodeFromTree(root->right, value);
	else
	{
		if (root->left == NULL)
		{
			BSTNode *temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL)
		{
			BSTNode *temp = root->left;
			free(root);
			return temp;
		}
		BSTNode *temp = find_inorder_successor(root->right);
		root->item = temp->item;
		root->right = removeNodeFromTree(root->right, temp->item);
	}
	return root;
}
///////////////////////////////////////////////////////////////////////////////

void insertBSTNode(BSTNode **node, int value){
	if (*node == NULL)
	{
		*node = malloc(sizeof(BSTNode));

		if (*node != NULL) {
			(*node)->item = value;
			(*node)->left = NULL;
			(*node)->right = NULL;
		}
	}
	else
	{
		if (value < (*node)->item)
		{
			insertBSTNode(&((*node)->left), value);
		}
		else if (value >(*node)->item)
		{
			insertBSTNode(&((*node)->right), value);
		}
		else
			return;
	}
}

//////////////////////////////////////////////////////////////////////////////////

void push(Stack *stack, BSTNode * node)
{
	StackNode *temp;

	temp = malloc(sizeof(StackNode));

	if (temp == NULL)
		return;
	temp->data = node;

	if (stack->top == NULL)
	{
		stack->top = temp;
		temp->next = NULL;
	}
	else
	{
		temp->next = stack->top;
		stack->top = temp;
	}
}


BSTNode * pop(Stack * s)
{
	StackNode *temp, *t;
	BSTNode * ptr;
	ptr = NULL;

	t = s->top;
	if (t != NULL)
	{
		temp = t->next;
		ptr = t->data;

		s->top = temp;
		free(t);
		t = NULL;
	}

	return ptr;
}

BSTNode * peek(Stack * s)
{
	StackNode *temp;
	temp = s->top;
	if (temp != NULL)
		return temp->data;
	else
		return NULL;
}

int isEmpty(Stack *s)
{
	if (s->top == NULL)
		return 1;
	else
		return 0;
}


void removeAll(BSTNode **node)
{
	if (*node != NULL)
	{
		removeAll(&((*node)->left));
		removeAll(&((*node)->right));
		free(*node);
		*node = NULL;
	}
}
