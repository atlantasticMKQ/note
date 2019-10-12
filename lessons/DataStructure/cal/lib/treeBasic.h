//state
#define FUNC	0
#define EVAL	1
#define ELEM	2
#define LINK	3
char *stateToStr(int state)
{
	switch(state)
		{
		case FUNC:
			return "FUNC";
		case EVAL:
			return "EVAL";
		case ELEM:
			return "ELEM";
		case LINK:
			return "LINK";
		default :
			return "UNKNOWN";
		}
}
struct treeNode
{
	int type;
	int state;
	char *elem;
	struct treeNode *left;
	struct treeNode *right;
};
typedef struct treeNode tree;

tree *treeInit()
{
	tree *root=MALLOC(tree);
	IF_NULL_RET_NULL(root);
	root->type=UNSET;
	root->state=EVAL;
	root->elem=NULL;
	root->left=NULL;
	root->right=NULL;
	
	return root;
}
int treeAddL(tree *root,int type,int state,char *elem)
{
	tree *left=MALLOC(tree);
	IF_NULL_RET_OF(left);
	left->type=type;
	left->state=state;
	left->elem=elem;
	root->left=left;
	left->left=NULL;
	left->right=NULL;
       	//printf("treeAdd->\ntype:%s\nstate:%s\nelem:%s\n<<<<\n",typeToStr(type),stateToStr(state),elem);
	return OK;
}
int treeAddR(tree *root,int type,int state,char *elem)
{
	tree *tmp;
	for(tmp=root;tmp->right!=NULL;tmp=tmp->right);
	tmp->right=MALLOC(tree);
	IF_NULL_RET_OF(tmp->right);
	tmp=tmp->right;
	tmp->left=NULL;
	tmp->right=NULL;
	tmp->type=UNSET;
	tmp->state=LINK;
	tmp->elem=NULL;
	int err=treeAddL(tmp,type,state,elem);
	if(err==OF)
		{
			free(tmp);
			return OF;
		}
	return OK;
}
		
		
int freeTree(tree *root)
{
	if(root==NULL)
		return OK;
	else
		{
			freeTree(root->left);
			freeTree(root->right);
			free(root->elem);
			free(root);
			return OK;
		}
}
int copyTree(tree *root,tree **copyPtr)
{
	tree *copy;
	tree *left;
	tree *right;
	if(root==NULL)
		{
			printf("(null)\n");
			*copyPtr=NULL;
			return OK;
		}
	int err=copyTree(root->left,&left);
	IF_NOT_OK_RET(err);
	err=copyTree(root->right,&right);	
	IF_NOT_OK_RET(err);
	copy=MALLOC(tree);
	IF_NULL_DO_RET_OF(copy,do{freeTree(left);freeTree(right);}while(0));
	
	copy->type=root->type;
	copy->state=root->state;
	char *copyElem=copyStr(root->elem);
	if(copyElem==NULL&&root->elem!=NULL)
		{
			freeTree(left);
			freeTree(right);
			free(copy);
			return OF;
		}
	copy->elem=copyElem;
	*copyPtr=copy;
	return OK;
}
