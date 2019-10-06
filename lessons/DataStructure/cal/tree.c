#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<editline.h>
/*
#include"lib/types.h"
#include"lib/status.h"
#include"lib/op.h"
#include"lib/int.h"
#include"lib/bool.h"
#include"lib/error.h"
*/
//#include"lib/gramCheck.h"

#include"lib/func.h"

#define FUNC	0
#define EVAL	1
#define ELEM	2
#define LINK	3


struct treeNode
{
	int type;
	int state;

	char *elem;
	//poly *poly;

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
tree *treeAddL(tree *root,int type,int state,char *elem)
{
	tree *left=MALLOC(tree);
	IF_NULL_RET_NULL(left);
	left->type=type;
	left->state=state;
	left->elem=elem;
	root->left=left;
	left->left=NULL;
	left->right=NULL;
	return left;
}
tree *treeAddR(tree *root,int type,int state,char *elem)
{
	tree *tmp;
	for(tmp=root;tmp->right!=NULL;tmp=tmp->right);
	tmp->right=MALLOC(tree);
	IF_NULL_RET_NULL(tmp->right);
	tmp=tmp->right;
	tmp->left=NULL;
	tmp->right=NULL;
	tmp->type=UNSET;
	tmp->state=LINK;
	tmp->elem=NULL;
	treeAddL(tmp,type,state,elem);
	return tmp->left;
}
		
		

char *readLen(char *ptr,int num)
{
	char *line=MALLOC_NUM(char,FUNC_NAME_SIZE);
	IF_NULL_RET_NULL(line);
	for(int i=0;i<num;i++)
		{
			line[i]=*ptr;
			ptr++;
		}
	line[num]='\0';
	return line;
}
int plantTree(tree *root,char *str,elem *head)
{
	printf("----enterPlantTree----\n");
	char *ptr=str;
	char *subPtr=str;
	for(ptr=str;!ifLeft(*ptr);ptr++);
	printf("left:%c\n",*ptr);
	for(;ifBlank(*ptr)||ifLeft(*ptr);ptr++);
	subPtr=ptr;
	int functag=0;
	while(TRUE)
		{
			printf(">loop\n");
			if(ifLetter(*ptr))
				{

					for(subPtr=ptr;*subPtr!=' '&&!ifRight(*subPtr);subPtr++)
						{
							if(!ifLetter(*subPtr))
								return NAMEERR;
						}
					char *name=readLen(ptr,subPtr-ptr);
					printf("name:%s\n",name);
					if(functag==0)
						{
							treeAddL(root,FUNC,ELEM,name);
							functag=1;
						}
					else
						{
							elem *elem=findElem(head,name);
							int type;
							if(elem==NULL)
								type=UNSET;
							else
								type=elem->type;
							treeAddR(root,type,ELEM,name);
						}
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
					
				}
			else if(ifNum(*ptr)||*ptr=='-')
				{

					int point=0;
					for(subPtr=ptr+1;*subPtr!=' '&&!ifRight(*subPtr);subPtr++)
						{
							if(*subPtr=='.'&&point==0)
								point=1;
							else if(*subPtr=='.'&&point==1)
								return NUMERR;
							if(!ifNum(*subPtr)&&*subPtr!='.')
								return NUMERR;
						}
					char *num=readLen(ptr,subPtr-ptr);
					printf("num:%s\n",num);
					if(functag!=1)
						return NAMEERR;
					int type;
					if(point==0)
						type=INT;
					else if(point==1)
						type=FLOAT;
					treeAddR(root,type,ELEM,num);
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			else if(ifLeft(*ptr))
				{

					if(functag!=1)
						{
							return ERR;
						}
					tree *newRoot=treeAddR(root,UNSET,LINK,NULL);
					plantTree(newRoot,ptr,head);
					int brac=1;
					for(subPtr=ptr+1;!(ifRight(*subPtr)&&brac==1);subPtr++)
						{
							
							if(ifRight(*subPtr))
								brac--;
							else if(ifLeft(*subPtr))
								brac++;
						}

					subPtr++;
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			else if(*ptr=='"')
				{
					if(functag!=1)
						{
							return ERR;
						}
					for(subPtr=ptr+1;*subPtr!='"';subPtr++);
						
					char *str=readLen(ptr+1,subPtr-ptr-1);
					printf("str:%s\n",str);
					treeAddL(root,STR,ELEM,str);
					subPtr++;
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			else if(ifRight(*ptr))
				{
					printf("right:%c\n",*ptr);
					printf("----exitPlantTree----\n");
					return OK;
				}
		}
	
	return OK;
}
	
	
			
			

int main()
{
	char *line=readline("mkq> ");
	int i=gramCheck(line);
	printf("%s\n",statToStr(i));
	tree *root=treeInit();
	elem *head=elemInit();
	int b=plantTree(root,line,head);
	printf("%s\n",statToStr(b));
	free(NULL);
}
