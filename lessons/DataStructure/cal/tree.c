#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<editline.h>

#include"lib/tools.h"
#include"lib/bool.h"
#include"lib/status.h"
#include"lib/types.h"
#include"lib/str.h"

#include"lib/elem.h"
#include"lib/func.h"
#include"lib/gramCheck.h"

//state
#define FUNC	0
#define EVAL	1
#define ELEM	2
#define LINK	3


#define EXIT	100
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
       	printf("treeAdd->\ntype:%s\nstate:%s\nelem:%s\n<<<<\n",typeToStr(type),stateToStr(state),elem);
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
		
		


int plantTree(tree *root,char *str,elem *head)
{
	printf("----enterPlantTree----\n");
	char *ptr=str;
	char *subPtr=str;
	for(ptr=str;!ifLeft(*ptr);ptr++);
	//printf("left:%c\n",*ptr);
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
					//printf("name:%s\n",name);
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
					//printf("num:%s\n",num);
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
					tree *newRoot=treeAddR(root,UNSET,EVAL,NULL);
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
					//printf("str:%s\n",str);
					treeAddL(root,STR,ELEM,str);
					subPtr++;
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			else if(ifRight(*ptr))
				{
					//printf("right:%c\n",*ptr);
					printf("----exitPlantTree----\n");
					return OK;
				}
		}
	
	return OK;
}
int eval(tree *root,func *fhead,elem *ehead)
{
	
	if(root->state!=EVAL||root->left->type!=FUNC)
		return MATCHERR;
	if(strcmp(root->left->elem,"def")==0)
		{
			arg *argHead=argInit();
			IF_NULL_RET_OF(argHead);
			if(root->right==NULL||root->right->left==NULL||root->right->right->left==NULL)
				return ERR;
			
			tree *arg=root->right->right->left;
			if(strcmp(arg->left->elem,"arg")!=0)
				return ERR;
			int type=1;
			for(tree *tmp=arg->right;tmp!=NULL;tmp=tmp->right)
				{
					if(type==1)
						{
							if(tmp->right==NULL||tmp->right->left==NULL)
								return ERR;
							addArg(argHead,strToType(tmp->left->elem),tmp->right->left->elem);
							type=0;
						}
					else
						type=1;
				}
			addFunc(fhead,UNSET,root->right->left->elem,0,argHead);
			root->state=ELEM;
			root->elem="TRUE";
			root->type=BOOL;
			
			return OK;
		}
	
	if(strcmp(root->left->elem,"help")==0)
		{
			printf("here is some help\n");
			root->state=ELEM;
			root->elem="TRUE";
			root->type=BOOL;
			return OK;
		}
	if(strcmp(root->left->elem,"funclist")==0)
		{
			foreachFunc(fhead,printFunc);
			root->state=ELEM;
			root->elem="TRUE";
			root->type=BOOL;
			return OK;
		}
	if(strcmp(root->left->elem,"quit")==0)
		return EXIT;
		    
	//eval args
	for(tree *tmp=root->right;tmp!=NULL;tmp=tmp->right)
		{
			if(tmp->left==NULL)
				return ERR;
			if(tmp->left->state==EVAL)
				eval(tmp->left,fhead,ehead);
			else if(tmp->left->state!=ELEM)
				return ERR;
		}
	return OK;
}
	
			
			

int main()
{
	func *fhead=funcInit();
	elem *head=elemInit();
	while(TRUE)
		{
			
			char *line=readline("mkq> ");
			if(gramCheck(line)!=OK)
				{
					printf("gramCheck not ok!\n");
				}
			
			tree *root=treeInit();			


			if(plantTree(root,line,head)!=OK)
				{
					printf("plant not OK\n");
					continue;
				}
			
			int err=eval(root,fhead,NULL);
			if(err==EXIT)
				return 0;
		}
}
