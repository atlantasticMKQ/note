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
#include"lib/file.h"

#include"lib/elem.h"
#include"lib/treeBasic.h"
#include"lib/func.h"
#include"lib/gramCheck.h"


#define EXIT	100
	
		


int plantTree(tree *root,char *str,elem *head)
{
	//ptr point to string's head subptr go follow chars
	char *ptr=str;
	char *subPtr=str;

	//ptr point to (
	for(ptr=str;!ifLeft(*ptr);ptr++);

	//ptr point to first letter after (
	for(;ifBlank(*ptr)||ifLeft(*ptr);ptr++);
	subPtr=ptr;

	//root->left is func node there must be a func and rights are elems
	int functag=0;
	int err;
	while(TRUE)
		{
			//name
			if(ifLetter(*ptr))
				{

					for(subPtr=ptr;*subPtr!=' '&&!ifRight(*subPtr);subPtr++)
						{
							//only letters are allowed in func/elem name
							if(!ifLetter(*subPtr))
								return NAMEERR;
						}
					//save the name
					char *name=readLen(ptr,subPtr-ptr);
					IF_NULL_DO_RET_OF(name,freeTree(root));

					//creat func node
					if(functag==0)
						{
							err=treeAddL(root,FUNC,FUNC,name);
							IF_NOT_OK_DO_RET(err,freeTree(root));
							functag=1;
						}
					else
						{
							//elem *elem=findElem(head,name);
							int type;
							
							//if the elem was quote
							//if(elem==NULL)
							type=UNSET;
							//else
							//type=elem->type;
							
							err=treeAddR(root,type,ELEM,name);
							IF_NOT_OK_DO_RET(err,freeTree(root));
						}
					//move ptr and sub ptr to next noblank char 
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
					
				}
			//number
			else if(ifNum(*ptr)||*ptr=='-')
				{
					//point tag num without point is int ,else float
					int point=0;
					
					//check if num in law
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
					IF_NULL_DO_RET_OF(num,freeTree(root));

					//this is a func!?
					if(functag!=1)
						return FUNCNAMEERR;
					
					int type;
					if(point==0)
						type=INT;
					else if(point==1)
						type=FLOAT;
					
					err=treeAddR(root,type,ELEM,num);
					IF_NOT_OK_DO_RET(err,freeTree(root));

					//same to above
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			//state
			else if(ifLeft(*ptr))
				{

					if(functag!=1)
						{
							return FUNCNAMEERR;
						}
					tree *newRoot;
					err=treeAddR(root,UNSET,EVAL,NULL);
					IF_NOT_OK_DO_RET(err,freeTree(root));

					//move to the node added
					for(newRoot=root;newRoot->right!=NULL;newRoot=newRoot->right);
					newRoot=newRoot->left;
					
					err=plantTree(newRoot,ptr,head);
					IF_NOT_OK_DO_RET(err,freeTree(root));
					//go to the brac end
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
			//string
			else if(*ptr=='"')
				{
					//func name cant be string
					if(functag!=1)
						{
							return FUNCNAMEERR;
						}
					//subptr mark the position of next "
					for(subPtr=ptr+1;*subPtr!='"';subPtr++);
						
					char *str=readLen(ptr+1,subPtr-ptr-1);
					IF_NULL_DO_RET_OF(str,freeTree(root));

					err=treeAddL(root,STR,ELEM,str);
					IF_NOT_OK_DO_RET(err,freeTree(root));

					subPtr++;
					for(;*subPtr==' ';subPtr++);
					ptr=subPtr;
				}
			//)
			else if(ifRight(*ptr))
				{
					return OK;
				}
		}
	
	return OK;
}
int argNum(tree *root)
{
	if(root==NULL)
		return -1;
	int i=0;
	for(tree *tmp=root;tmp->right!=NULL&&tmp->right->left!=NULL;tmp=tmp->right)
		i++;

	return i;
}
			
int argCheck(tree *root,int num)
{
	int i=argNum(root);
	if(i==num)
		return OK;
	else
		return ARGNUMERR;
}
int argTypeCheck(tree *root,int index,int type,int state)
{
	int num=argNum(root);
	if(num<index)
		return ARGNUMERR;

	tree *node=root;
	for(int i=0;i<index;i++)
		{
			node=node->right;
		}
	node=node->left;
	if(node->type!=type||node->state!=state)
		return ARGTYPEERR;
	return OK;
}
			
int setTree(tree *node,char *elem,int type,int state)
{
	if(elem!=NULL)
		{
			node->elem=copyStr(elem);
			IF_NULL_RET_OF(node->elem);
		}
	else
		node->elem=NULL;
	node->type=type;
	node->state=state;
	return OK;
}

int eval(tree *root,func *fhead,elem *ehead)
{
	int err=OK;
	err=argTypeCheck(root,0,FUNC,FUNC);
	IF_NOT_OK_RET(err);

	//(defIn foo (arg INT mkq INT a INT b ...))
	IFFUNC(root,defIn)
		{
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,2,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			
			arg *argHead=argInit();
			IF_NULL_RET_OF(argHead);
			
			tree *arg=root->right->right->left;
			if(strcmp(arg->left->elem,"arg")!=0)
				{
					free(argHead);
					return FUNCNAMEERR;
				}

			int type=TRUE;
			for(tree *tmp=arg->right;tmp!=NULL;tmp=tmp->right)
				{
					if(type==TRUE)
						{
							err=argTypeCheck(tmp,0,UNSET,ELEM);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
							err=argTypeCheck(tmp,1,UNSET,ELEM);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
							err=addArg(argHead,strToType(tmp->left->elem),tmp->right->left->elem);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							type=FALSE;
						}
					else
						type=TRUE;
				}
			err=addFunc(fhead,UNSET,root->right->left->elem,0,argHead,NULL);
			IF_NOT_OK_DO_RET(err,freeArg(argHead));

			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	//(defIn foo (arg INT mkq INT a INT b ...) (add mkq (add a b)))
	IFFUNC(root,def)
		{
			err=argCheck(root,3);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,2,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,3,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			
			
			arg *argHead=argInit();
			IF_NULL_RET_OF(argHead);
			
			tree *arg=root->right->right->left;
			if(strcmp(arg->left->elem,"arg")!=0)
				{
					free(argHead);
					return FUNCNAMEERR;
				}

			int type=TRUE;
			for(tree *tmp=arg->right;tmp!=NULL;tmp=tmp->right)
				{
					if(type==TRUE)
						{
							err=argTypeCheck(tmp,0,UNSET,ELEM);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
							err=argTypeCheck(tmp,1,UNSET,ELEM);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
							err=addArg(argHead,strToType(tmp->left->elem),tmp->right->left->elem);
							IF_NOT_OK_DO_RET(err,freeArg(argHead));
							type=FALSE;
						}
					else
						type=TRUE;
				}
			tree *eval;
			err=copyTree(root->right->right->right->left,&eval);
			IF_NOT_OK_DO_RET(err,freeArg(argHead));
			err=addFunc(fhead,UNSET,root->right->left->elem,0,argHead,eval);
			IF_NOT_OK_DO_RET(err,freeArg(argHead));

			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}

	//help (help index)
	IFFUNC(root,help)
		{
			err=argCheck(root,1);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			
			char *path;			
			path=copyStr("./help/");
			IF_NULL_RET_OF(path);
			
			strcat(path,root->right->left->elem);

			err=printFile(path);
			if(err!=NOSUCHFILE)
				IF_NOT_OK_RET(err);
			char *elem;
			if(err!=OK)
				{
					elem="FALSE";
				}
			else
				{
					elem="TRUE";
				}
			err=setTree(root,elem,BOOL,ELEM);
			IF_NOT_OK_RET(err);
			free(path);
			return OK;

		}
	//(listFunc)
	IFFUNC(root,listFunc)
		{
			err=argCheck(root,0);
			IF_NOT_OK_RET(err);
			foreachFunc(fhead,printFunc);
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,listElem)
		{
			err=argCheck(root,0);
			IF_NOT_OK_RET(err);
			foreachElem(ehead,printElem);
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}

	IFFUNC(root,quit)
		{
			return EXIT;
		}
	IFFUNC(root,if)
		{
			return OK;
		}
	printf("func:%s\n",root->left->elem);
	//eval args
	for(tree *tmp=root->right;tmp!=NULL;tmp=tmp->right)
		{
			printf("%s:\n",tmp->left->elem);
			if(tmp->left==NULL)
				{
					return ERR;
				}
			if(tmp->left->state==EVAL)
				{
					err=eval(tmp->left,fhead,ehead);
					if(err!=OK)
						return err;
				}
			else if(tmp->left->state!=ELEM)
				return ERR;
		}
	printf("func:%s\n",root->left->elem);
	//quote (quote mkq 1.121)
	printf("type:%i\n",root->right->left->type);
	IFFUNC(root,quote)
		{

			err=argCheck(root,2);
			IF_NOT_OK_RET(err);

			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			elem *e=findElem(ehead,root->right->left->elem);
			printf("2");

			char *name=root->right->left->elem;
			int type;
			int num=0;
			double val=0;
			int bool=FALSE;
			char *str=NULL;
			char *elem=root->right->right->left->elem;
			if(e!=NULL)
				return ELEMREDEF;
			if(argTypeCheck(root,2,STR,ELEM)==OK)
				{
					type=STR;
					str=elem;
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,BOOL,ELEM)==OK)
				{
					type=BOOL;
					bool=strToBool(elem);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			
			else if(argTypeCheck(root,2,INT,ELEM)==OK)
				{
					type=INT;
					num=atoi(elem);
					printf("%s,%s\n",name,elem);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					type=FLOAT;
					val=atof(elem);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					type=POLY;
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,UNSET,ELEM)==OK)
				{
					e=findElem(ehead,root->right->right->left->elem);
					if(e==NULL)
						return ELEMNOTFOUND;
					err=addElem(ehead,e->type,e->num,e->bool,e->str,e->val,name);
					IF_NOT_OK_RET(err);
				}
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			
		}	
	return OK;
}
	

int main()
{
	func *fhead=funcInit();
	elem *ehead=elemInit();
	while(TRUE)
		{
			
			char *line=readline("mkq> ");
			printf("\n");
			if(gramCheck(line)!=OK)
				{
					ERROR("gramCheck not ok!\n");
				}
			
			tree *root=treeInit();			


			if(plantTree(root,line,ehead)!=OK)
				{
					ERROR("plant not OK\n");
					continue;
				}

			int err=eval(root,fhead,ehead);
			printf("%i\n",err);
			printf("%s\n",statToStr(err));
			//printFile("test");
			printf("\n");
			if(err==EXIT)
				return 0;
			freeTree(root);
		}
}
