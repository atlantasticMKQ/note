#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<editline.h>

#include"lib/defines.h"
#include"lib/tools.h"
#include"lib/bool.h"
#include"lib/double.h"
#include"lib/status.h"
#include"lib/types.h"
#include"lib/str.h"
#include"lib/file.h"


#include"lib/elem.h"
#include"lib/treeBasic.h"
#include"lib/func.h"
#include"lib/gramCheck.h"
#include"lib/plantTree.h"
#define EXIT	100
	
		

//this method is design to call self-defined funcs
//via copy eval tree from func table and replace args with the real args
//mirror is the eval tree copied 
int mirrorSetup(tree *mirror,struct funcNode *f,tree *root)
{
	int err;
	tree *newMirror;
	//以newMirror为临时变量遍历mirror的参数
	for(newMirror=mirror->right;newMirror!=NULL;newMirror=newMirror->right)
		{
			//mirror的参数:newMirror->left
			//如果发现需要求值的其他参数,就把它setup
			if(newMirror->left->type==UNSET&&newMirror->left->state==EVAL)
				mirrorSetup(newMirror->left,f,root);
			//如果是元素的话,就把它替换掉
			if(newMirror->left->state==ELEM)
				{
					//遍历参数表,寻找和出现元素名称相同的参数的位置
					for(struct argNode *a=f->args->next;a!=NULL;a=a->next)
						{
							if(strcmp(a->name,newMirror->left->elem)==0)
								{
									//node->left最后会指向那个位置的实参
									tree *node=root;
									for(int i=0;i<a->index;node=node->right)
										i++;
									//把mirror的参数设置成了实参的样子
									err=setTree(newMirror->left,node->left->elem,node->left->type,node->left->state);
									IF_NOT_OK_RET(err);
								}
						}
				}
		}
	return OK;
}
//(defIn foo (arg INT mkq INT a INT b ...))
int defIn(tree *root,func *fhead,elem *ehead)
{
	int err=OK;
	err=argCheck(root,2);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,1,UNSET,ELEM);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,2,UNSET,EVAL);
	IF_NOT_OK_RET(err);
			
	tree *arg=ARG_2(root);
	IF_NEQ(ARG_0(arg)->elem,arg)
		return FUNCNAMEERR;

	arg *argHead=argInit();
	IF_NULL_RET_OF(argHead);
	//save if this arg is type or elem
	int type=TRUE;
	for(tree *tmp=arg->right;tmp!=NULL;tmp=tmp->right)
		{
			if(type==TRUE)
				{
					err=argTypeCheck(tmp,0,UNSET,ELEM);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
					err=argTypeCheck(tmp,1,UNSET,ELEM);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
					err=addArg(argHead,strToType(ARG_0(tmp)->elem),ARG_1(tmp)->elem);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
					type=FALSE;
				}
			else
				type=TRUE;
		}
	err=addFunc(fhead,ARG_1(root)->elem,0,argHead,NULL);
	IF_NOT_OK_DO_RET(err,freeArg(argHead));

	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int def(tree *root,func *fhead,elem *ehead)
{
	int err=OK;
	err=argCheck(root,3);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,1,UNSET,ELEM);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,2,UNSET,EVAL);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,3,UNSET,EVAL);
	IF_NOT_OK_RET(err);
			
	tree *arg=ARG_2(root);
	IF_NEQ(ARG_0(arg)->elem,arg)
		return FUNCNAMEERR;
	arg *argHead=argInit();
	IF_NULL_RET_OF(argHead);

	int type=TRUE;
	for(tree *tmp=arg->right;tmp!=NULL;tmp=tmp->right)
		{
			if(type==TRUE)
				{
					err=argTypeCheck(tmp,0,UNSET,ELEM);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
					err=argTypeCheck(tmp,1,UNSET,ELEM);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
					err=addArg(argHead,strToType(ARG_0(tmp)->elem),ARG_1(tmp)->elem);
					IF_NOT_OK_DO_RET(err,freeArg(argHead));
							
					type=FALSE;
				}
			else
				type=TRUE;
		}
	tree *eval;
	err=copyTree(ARG_3(root),&eval);
	IF_NOT_OK_DO_RET(err,freeArg(argHead));
			
	err=addFunc(fhead,ARG_1(root)->elem,getArgNum(argHead),argHead,eval);
	IF_NOT_OK_DO_RET(err,freeArg(argHead));

	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int help(tree *root)
{
	int err=OK;
	err=argCheck(root,1);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,1,UNSET,ELEM);
	IF_NOT_OK_RET(err);
			
	char *path;			
	path=copyStr(HELP_DIR);
	IF_NULL_RET_OF(path);
			
	strcat(path,ARG_1(root)->elem);
	err=printFile(path);
	if(err!=NOSUCHFILE)
		IF_NOT_OK_RET(err);

	char *elem;
	if(err!=OK)
		elem="FALSE";
	else
		elem="TRUE";
	err=setTree(root,elem,BOOL,ELEM);
	IF_NOT_OK_RET(err);
	free(path);
	return OK;
}
int listFunc(tree *root,func *fhead)
{
	int err=OK;
	err=argCheck(root,0);
	IF_NOT_OK_RET(err);
	foreachFunc(fhead,printFunc);
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int listElem(tree *root,elem *ehead)
{
	int err=OK;
	err=argCheck(root,0);
	IF_NOT_OK_RET(err);
	foreachElem(ehead,printElem);
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int eval(tree *root,func *fhead,elem *ehead);
int loop(tree *root,func *fhead,elem *ehead)
{
	int err=OK;
	err=argCheck(root,2);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,2,UNSET,EVAL);
	IF_NOT_OK_RET(err);

	tree *evalBack,*tmp;
	//把右半边树全部复制到evalBack
	err=copyTree(root->right,&evalBack);
	IF_NOT_OK_RET(err);
	while(TRUE)
		{
			freeTree(root->right);
			err=copyTree(evalBack,&tmp);
			root->right=tmp;
			IF_NOT_OK_DO_RET(err,freeTree(evalBack));
			//eval arg 1
			if(argTypeCheck(root,1,UNSET,EVAL)==OK)
				err=eval(ARG_1(root),fhead,ehead);
			IF_NOT_OK_RET(err);
					
			if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					IF_EQ(ARG_1(root)->elem,TRUE)
						{
							err=eval(ARG_2(root),fhead,ehead);
							IF_NOT_OK_DO_RET(err,freeTree(evalBack));
							continue;
						}
					else
						break;
				}
			else if(argTypeCheck(root,1,UNSET,ELEM)==OK)
				{
					elem *e=findElem(ehead,ARG_1(root)->elem);
					if(e==NULL)
						{
							freeTree(evalBack);
							return ELEMNOTFOUND;
						}
					if(e->type!=BOOL)
						{
							freeTree(evalBack);
							return ARGTYPEERR;
						}
					if(e->bool==TRUE)
						{
							err=eval(ARG_2(root),fhead,ehead);
							IF_NOT_OK_DO_RET(err,freeTree(evalBack));
							continue;
						}
					else
						break;
				}
			else
				return ARGTYPEERR;
		}
	freeTree(evalBack);
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int judge(tree *root,func *fhead,elem *ehead)
{
	int err=OK;
	err=argCheck(root,3);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,2,UNSET,EVAL);
	IF_NOT_OK_RET(err);
	err=argTypeCheck(root,3,UNSET,EVAL);
	IF_NOT_OK_RET(err);
			
	if(argTypeCheck(root,1,UNSET,EVAL)==OK)
		err=eval(ARG_1(root),fhead,ehead);
	IF_NOT_OK_RET(err);
	//返回值为最后执行语句的返回值
	if(argTypeCheck(root,1,BOOL,ELEM)==OK)
		{
			IF_EQ(ARG_1(root)->elem,TRUE)
				{
					err=eval(ARG_2(root),fhead,ehead);
					IF_NOT_OK_RET(err);
							
					err=setTree(root,ARG_2(root)->elem,ARG_2(root)->type,ARG_2(root)->state);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=eval(ARG_3(root),fhead,ehead);
					IF_NOT_OK_RET(err);
							
					err=setTree(root,ARG_3(root)->elem,ARG_3(root)->type,ARG_3(root)->state);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	if(argTypeCheck(root,1,UNSET,ELEM)==OK)
		{
			elem *e=findElem(ehead,ARG_1(root)->elem);
			if(e==NULL)
				return ELEMNOTFOUND;
			if(e->type!=BOOL)
				return ARGTYPEERR;
			if(e->bool==TRUE)
				{
					err=eval(ARG_2(root),fhead,ehead);
					IF_NOT_OK_RET(err);
							
					err=setTree(root,ARG_2(root)->elem,ARG_2(root)->type,ARG_2(root)->state);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=eval(ARG_3(root),fhead,ehead);
					IF_NOT_OK_RET(err);
					err=setTree(root,ARG_3(root)->elem,ARG_3(root)->type,ARG_3(root)->state);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	else
		return ARGTYPEERR;
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int print(tree *root,elem *ehead)
{
	int err=OK;
	int aNum=argNum(root);
	tree *tmp=root->right;
	for(int i=0;i<aNum;i++)
		{
			if(argTypeCheck(root,i+1,UNSET,ELEM)==OK)
				{
					elem *e=findElem(ehead,tmp->left->elem);
					if(e==NULL)
						printf(PRINT_NULL_ELEM);
					else
						{
							switch(e->type)
								{
								case INT:
									printf(PRINT_INT_ELEM,e->num);
									break;
								case FLOAT:
									printf(PRINT_FLOAT_ELEM,e->val);
									break;
								case BOOL:
									printf(PRINT_BOOL_ELEM,boolToStr(e->bool));
									break;
								case STR:
									printf(PRINT_STR_ELEM,e->str);
									break;
								}
						}
				}
			else 
				printf("%s",tmp->left->elem);
			tmp=tmp->right;
		}
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;

}
int br(tree *root)
{
	int err=OK;
	err=argCheck(root,0);
	IF_NOT_OK_RET(err);
	printf("\n");
	err=setTree(root,"TRUE",BOOL,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int multi(tree *root)
{
	int err=OK;
	int aNum=argNum(root);
	if(aNum==0)
		return ARGNUMERR;
	tree *ret=root;
	for(int i=0;i<aNum;i++)
		ret=ret->right;
	ret=ret->left;
			
	err=setTree(root,ret->elem,ret->type,ELEM);
	IF_NOT_OK_RET(err);
	return OK;
}
int eval(tree *root,func *fhead,elem *ehead)
{
	int err=OK;

	err=argTypeCheck(root,0,FUNC,FUNC);
	IF_NOT_OK_RET(err);

	//
	//build-in funcs
	//
	
	//(defIn foo (arg INT mkq INT a INT b ...))
	IFFUNC(root,defIn)
		err=defIn(root,fhead,ehead);
	IF_NOT_OK_RET(err);

	//(def foo (arg INT mkq INT a INT b ...) (add mkq (add a b)))
	IFFUNC(root,def)
		err=def(root,fhead,ehead);
	IF_NOT_OK_RET(err);

	//help (help index)
	IFFUNC(root,help)
		err=help(root);
	IF_NOT_OK_RET(err);
	
	//(listFunc)
	IFFUNC(root,listFunc)
		err=listFunc(root,fhead);
	IF_NOT_OK_RET(err);

	//(listElem)
	IFFUNC(root,listElem)
		err=listElem(root,ehead);
	IF_NOT_OK_RET(err);

	//(quit)
	IFFUNC(root,quit)
		return EXIT;

	//(loop (true) (multi (print "Hello m-lisp") (br)))
	IFFUNC(root,loop)
		err=loop(root,fhead,ehead);
	IF_NOT_OK_RET(err);

	//(if (true) (multi (print "true") (br)) (multi (print "false") (br)))
	IFFUNC(root,if)
		err=judge(root,fhead,ehead);
	IF_NOT_OK_RET(err);
	
	//eval args
	for(tree *tmp=root->right;tmp!=NULL;tmp=tmp->right)
		{
			IF_NULL_RET(tmp->left,ERR)
			if(tmp->left->state==EVAL)
				{
					err=eval(tmp->left,fhead,ehead);
					IF_NOT_OK_RET(err);
				}
			else if(tmp->left->state!=ELEM)
				return ERR;
		}

	//(print elemName "is " "xxxx")
	IFFUNC(root,print)
		err=print(root,ehead);
	IF_NOT_OK_RET(err);

	//(br)
	IFFUNC(root,br)
		err=br(root);
	IF_NOT_OK_RET(err);

	//所以必须保证上面对参数求值是从左到右顺序的
	//(multi (print "mkq") (br) (print "is") (br) (print "best"))
	IFFUNC(root,multi)
		err=multi(root);
	IF_NOT_OK_RET(err);
	
	IFFUNC(root,and)
		{
			int bool1,bool2;
			elem *e1,*e2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=BOOL)
						return ELEMTYPEERR;
					bool1=e1->bool;
				}
			else if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->left->elem,"TRUE")==0)
						bool1=TRUE;
					else
						bool1=FALSE;
				}
			else
				return argTypeCheck(root,1,BOOL,ELEM);
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=BOOL)
						return ELEMTYPEERR;
					bool2=e2->bool;
				}
			else if(argTypeCheck(root,2,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->right->left->elem,"TRUE")==0)
						bool2=TRUE;
					else
						bool2=FALSE;
				}
			else
				return argTypeCheck(root,2,BOOL,ELEM);
			if(bool1&&bool2==TRUE)
				{
					err=setTree(root,"TRUE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=setTree(root,"FALSE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	IFFUNC(root,or)
		{
			int bool1,bool2;
			elem *e1,*e2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=BOOL)
						return ELEMTYPEERR;
					bool1=e1->bool;
				}
			else if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->left->elem,"TRUE")==0)
						bool1=TRUE;
					else
						bool1=FALSE;
				}
			else
				return argTypeCheck(root,1,BOOL,ELEM);
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=BOOL)
						return ELEMTYPEERR;
					bool2=e2->bool;
				}
			else if(argTypeCheck(root,2,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->right->left->elem,"TRUE")==0)
						bool2=TRUE;
					else
						bool2=FALSE;
				}
			else
				return argTypeCheck(root,2,BOOL,ELEM);
			if(bool1||bool2==TRUE)
				{
					err=setTree(root,"TRUE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=setTree(root,"FALSE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	IFFUNC(root,xor)
		{
			int bool1,bool2;
			elem *e1,*e2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=BOOL)
						return ELEMTYPEERR;
					bool1=e1->bool;
				}
			else if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->left->elem,"TRUE")==0)
						bool1=TRUE;
					else
						bool1=FALSE;
				}
			else
				return argTypeCheck(root,1,BOOL,ELEM);
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=BOOL)
						return ELEMTYPEERR;
					bool2=e2->bool;
				}
			else if(argTypeCheck(root,2,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->right->left->elem,"TRUE")==0)
						bool2=TRUE;
					else
						bool2=FALSE;
				}
			else
				return argTypeCheck(root,2,BOOL,ELEM);
			if(bool1!=bool2)
				{
					err=setTree(root,"TRUE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=setTree(root,"FALSE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	IFFUNC(root,not)
		{
			int bool;
			elem *e;
			err=argCheck(root,1);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e=findElem(ehead,root->right->left->elem);
					if(e==NULL)
						return ELEMNOTFOUND;
					if(e->type!=BOOL)
						return ELEMTYPEERR;
					bool=e->bool;
				}
			else if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->left->elem,"TRUE")==0)
						bool=TRUE;
					else
						bool=FALSE;
				}
			else
				return argTypeCheck(root,1,BOOL,ELEM);
			if(!bool==TRUE)
				{
					err=setTree(root,"TRUE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else
				{
					err=setTree(root,"FALSE",BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
		}
	
	IFFUNC(root,true)
		{
			err=argCheck(root,0);
			IF_NOT_OK_RET(err);
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,false)
		{
			err=argCheck(root,0);
			IF_NOT_OK_RET(err);
			err=setTree(root,"FLASE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,poly)
		{
			int anum=argNum(root);
			if(anum%2!=0)
				return ARGNUMERR;
			for(int i=1;i<=anum;i++)
				{
					int type;
					if(i%2==1)
						type=FLOAT;
					else
						type=INT;
					err=argTypeCheck(root,i,INT,ELEM);
					if(err!=OK)
						err=argTypeCheck(root,i,type,ELEM);
					if(err!=OK)
						{
							err=argTypeCheck(root,i,UNSET,ELEM);
							if(err==OK)
								{
									tree *thisArg=root;
									for(int j=0;j<i;j++)
										{
											thisArg=thisArg->right;
										}
									thisArg=thisArg->left;
									elem *e=findElem(ehead,thisArg->elem);
									if(e==NULL)
										return ELEMNOTFOUND;
									if(e->type!=INT&&e->type!=type)
										return ARGTYPEERR;
									char *elem=MALLOC_NUM(char,NAME_SIZE);
									if(elem==NULL)
										return OF;
									if(e->type==FLOAT)
										sprintf(elem,"%lf",e->val);
									else
										sprintf(elem,"%i",e->num);
									err=setTree(thisArg,elem,type,ELEM);
									IF_NOT_OK_DO_RET(err,free(elem));
									free(elem);
								}
							else
								return ARGTYPEERR;
						}
				}
			double val;
			int exp;
			poly *head=polyInit();
			IF_NULL_RET_OF(head);
			for(int i=1;i<=anum;i++)
				{
					tree *thisArg=root;
					for(int j=0;j<i;j++)
						{
							thisArg=thisArg->right;
						}
					thisArg=thisArg->left;
					if(i%2==1)
						{
							val=atof(thisArg->elem);
						}
					else
						{
							exp=atoi(thisArg->elem);
							err=addPolyNode(head,exp,val);
							if(err!=OK)
								{
									freePoly(head);
									return err;
								}
						}
				}
			char *elem=polyToStr(head);
			if(elem==NULL)
				return OF;
			err=setTree(root,elem,POLY,ELEM);
			if(err!=OK)
				{
					freePoly(head);
					free(elem);
					return err;
				}
			return OK;
		}
	IFFUNC(root,let)
		{
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			elem *eBack=findElem(ehead,root->right->left->elem);
			elem *e;

			char *name=root->right->left->elem;
			int type;
			int num=0;
			double val=0;
			int bool=FALSE;
			char *str=NULL;
			char *elem=root->right->right->left->elem;
			//if(e!=NULL)
			//	{
					
			//	return ELEMREDEF;
			if(argTypeCheck(root,2,STR,ELEM)==OK)
				{
					type=STR;
					str=elem;
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,NULL,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,BOOL,ELEM)==OK)
				{
					type=BOOL;
					if(elem==NULL)
						return ERR;
					//printf("elem:%s\n",elem);
					bool=strToBool(elem);
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,NULL,val,name);
					IF_NOT_OK_RET(err);
				}
			
			else if(argTypeCheck(root,2,INT,ELEM)==OK)
				{
					type=INT;
					num=atoi(elem);
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,NULL,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					type=FLOAT;
					val=atof(elem);
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,NULL,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					type=POLY;
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,elem,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,UNSET,ELEM)==OK)
				{
					e=findElem(ehead,root->right->right->left->elem);
					if(e==NULL)
						return ELEMNOTFOUND;
					if(e==eBack)
						return OK;
					char *polyElem=NULL;
					if(e->type==POLY&&e->head!=NULL)
						{
							polyElem=polyToStr(e->head);
							if(polyElem==NULL)
								return OF;
						}
					err=addElem(ehead,e->type,e->num,e->bool,e->str,polyElem,e->val,name);
					free(polyElem);
					IF_NOT_OK_RET(err);
				}

			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,delElem)
		{
			err=argCheck(root,1);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,1,UNSET,ELEM);
			IF_NOT_OK_RET(err);
			elem *e=findElem(ehead,root->right->left->elem);
			if(e==NULL)
				return ELEMNOTFOUND;
			delElem(ehead,e);
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,add)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			poly *p1,*p2;
			int p1Free=0,p2Free=0;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT&&e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
					else
						{
							p1=e1->head;
							type1=POLY;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK||argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
					else
						{
							p1Free=1;
							p1=strToPoly(root->right->left->elem);
							if(p1==NULL)
								return OF;
							type1=POLY;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT&&e2->type!=POLY)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
					else
						{
							p2=e2->head;
							type2=FLOAT;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK||argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
					else
						{
							p2Free=1;
							p2=strToPoly(root->right->right->left->elem);
							if(p2==NULL)
								return OF;
							type2=POLY;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int re=num1+num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					double re=(double)num1+val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					double re=val1+val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1+(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==POLY&&type2==INT);
			else if(type1==POLY&&type2==FLOAT);
			else if(type1==POLY&&type2==POLY)
				{
					//printf("mkq\n");
					//foreachPoly(p1,printPoly);
					//foreachPoly(p2,printPoly);					
					poly *sum=addPoly(p1,p2);
					//printf("\n");
					//foreachPoly(sum,printPoly);
					if(sum==NULL)
						{
							if(p1Free)
								freePoly(p1);
							if(p2Free)
								freePoly(p2);
							return OF;
						}
					char *reStr=polyToStr(sum);
					err=setTree(root,reStr,POLY,ELEM);
					free(reStr);
					if(p1Free)
						freePoly(p1);
					if(p2Free)
						freePoly(p2);
					freePoly(sum);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==POLY);
			else if(type1==FLOAT&&type2==POLY);
				

		}
	IFFUNC(root,cross)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			poly *p1,*p2;
			int p1Free=0,p2Free=0;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT&&e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
					else
						{
							p1=e1->head;
							type1=POLY;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK||argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
					else
						{
							p1Free=1;
							p1=strToPoly(root->right->left->elem);
							if(p1==NULL)
								return OF;
							type1=POLY;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT&&e2->type!=POLY)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
					else
						{
							p2=e2->head;
							type2=POLY;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK||argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
					else
						{
							p2Free=1;
							p2=strToPoly(root->right->right->left->elem);
							if(p2==NULL)
								return OF;
							type2=POLY;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int re=num1*num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					double re=(double)num1*val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					double re=val1*val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1*(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
						else if(type1==POLY&&type2==INT);
			else if(type1==POLY&&type2==FLOAT);
			else if(type1==POLY&&type2==POLY)
				{
					//printf("mkq\n");
					//foreachPoly(p1,printPoly);
					//foreachPoly(p2,printPoly);					
					poly *sum=crossPoly(p1,p2);
					//printf("\n");
				       	//foreachPoly(sum,printPoly);
					if(sum==NULL)
						{
							if(p1Free)
								freePoly(p1);
							if(p2Free)
								freePoly(p2);
							return OF;
						}
					char *reStr=polyToStr(sum);
					err=setTree(root,reStr,POLY,ELEM);
					free(reStr);
					if(p1Free)
						freePoly(p1);
					if(p2Free)
						freePoly(p2);
					freePoly(sum);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==POLY);
			else if(type1==FLOAT&&type2==POLY);


		}
	IFFUNC(root,sub)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			poly *p1,*p2;
			int p1Free=0,p2Free=0;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT&&e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
					else
						{
							p1=e1->head;
							type1=POLY;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK||argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
					else
						{
							p1Free=1;
							p1=strToPoly(root->right->left->elem);
							if(p1==NULL)
								return OF;
							type1=POLY;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT&&e2->type!=POLY)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
					else
						{
							p2=e2->head;
							type2=POLY;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK||argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
					else
						{
							p2Free=1;
							p2=strToPoly(root->right->right->left->elem);
							if(p2==NULL)
								return OF;
							type2=POLY;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int re=num1-num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					double re=(double)num1-val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					double re=val1-val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1-(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
						else if(type1==POLY&&type2==INT);
			else if(type1==POLY&&type2==FLOAT);
			else if(type1==POLY&&type2==POLY)
				{
					//printf("mkq\n");
					//foreachPoly(p1,printPoly);
					//foreachPoly(p2,printPoly);					
					poly *sum=subPoly(p1,p2);
					//printf("\n");
				       	foreachPoly(sum,printPoly);
					if(sum==NULL)
						{
							if(p1Free)
								freePoly(p1);
							if(p2Free)
								freePoly(p2);
							return OF;
						}
					char *reStr=polyToStr(sum);
					err=setTree(root,reStr,POLY,ELEM);
					free(reStr);
					if(p1Free)
						freePoly(p1);
					if(p2Free)
						freePoly(p2);
					freePoly(sum);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==POLY);
			else if(type1==FLOAT&&type2==POLY);


		}
	IFFUNC(root,div)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			poly *p1,*p2;
			int p1Free=0,p2Free=0;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT&&e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
					else
						{
							p1=e1->head;
							type1=POLY;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK||argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
					else
						{
							p1Free=1;
							p1=strToPoly(root->right->left->elem);
							if(p1==NULL)
								return OF;
							type1=POLY;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT&&e2->type!=POLY)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
					else
						{
							p2=e2->head;
							type2=POLY;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK||argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
					else
						{
							p2Free=1;
							p2=strToPoly(root->right->right->left->elem);
							if(p2==NULL)
								return OF;
							type2=POLY;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int re=num1/num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					double re=(double)num1/val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					double re=val1/val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1/(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==POLY&&type2==INT);
			else if(type1==POLY&&type2==FLOAT);
			else if(type1==POLY&&type2==POLY)
				{
					//printf("mkq\n");
					//foreachPoly(p1,printPoly);
					//foreachPoly(p2,printPoly);					
					poly *sum=divPoly(p1,p2);
					//printf("\n");
				       	//foreachPoly(sum,printPoly);
					if(sum==NULL)
						{
							if(p1Free)
								freePoly(p1);
							if(p2Free)
								freePoly(p2);
							return OF;
						}
					char *reStr=polyToStr(sum);
					err=setTree(root,reStr,POLY,ELEM);
					free(reStr);
					if(p1Free)
						freePoly(p1);
					if(p2Free)
						freePoly(p2);
					freePoly(sum);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==POLY);
			else if(type1==FLOAT&&type2==POLY);


		}
	IFFUNC(root,mod)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			poly *p1,*p2;
			int p1Free=0,p2Free=0;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT&&e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
					else
						{
							p1=e1->head;
							type1=POLY;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK||argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
					else
						{
							p1Free=1;
							p1=strToPoly(root->right->left->elem);
							if(p1==NULL)
								return OF;
							type1=POLY;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT&&e2->type!=POLY)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
					else
						{
							p2=e2->head;
							type2=POLY;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK||argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
					else
						{
							p2Free=1;
							p2=strToPoly(root->right->right->left->elem);
							if(p2==NULL)
								return OF;
							type2=POLY;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int re=num1%num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					double re=(double)num1/val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					double re=val1/val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1/(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("float:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==POLY&&type2==INT);
			else if(type1==POLY&&type2==FLOAT);
			else if(type1==POLY&&type2==POLY)
				{
					//printf("mkq\n");
					//foreachPoly(p1,printPoly);
					//foreachPoly(p2,printPoly);					
					poly *sum=modPoly(p1,p2);
					//printf("\n");
					//foreachPoly(sum,printPoly);
					if(sum==NULL)
						{
							if(p1Free)
								freePoly(p1);
							if(p2Free)
								freePoly(p2);
							return OF;
						}
					char *reStr=polyToStr(sum);
					err=setTree(root,reStr,POLY,ELEM);
					free(reStr);
					if(p1Free)
						freePoly(p1);
					if(p2Free)
						freePoly(p2);
					freePoly(sum);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==POLY);
			else if(type1==FLOAT&&type2==POLY);
				

		}
	
	IFFUNC(root,equ)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int bool;
					if(num1==num2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					int bool=equDouble((double)num1,val2);
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					int bool=equDouble(val1,val2);
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					int bool=equDouble(val1,(double)num2);
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}

		}
	IFFUNC(root,less)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int bool;
					if(num1<num2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					int bool;
					if(num1<val2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					int bool;
					if(val1<val2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					int bool;
					if(val1<num2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}

		}
	IFFUNC(root,bigger)
		{
			int num1=0;
			int num2=0;
			double val1=0;
			double val2=0;
			elem *e1,*e2;
			int type1,type2;
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=INT&&e1->type!=FLOAT)
						return ELEMTYPEERR;
					if(e1->type==INT)
						{
							num1=e1->num;
							type1=INT;
						}
					else if(e1->type==FLOAT)
						{
							val1=e1->val;
							type1=FLOAT;
						}
				}
			else if(argTypeCheck(root,1,INT,ELEM)==OK||argTypeCheck(root,1,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,1,INT,ELEM)==OK)
						{
							num1=atoi(root->right->left->elem);
							type1=INT;
						}
					else if(argTypeCheck(root,1,FLOAT,ELEM)==OK)
						{
							val1=atof(root->right->left->elem);
							type1=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							num2=e2->num;
							type2=INT;
						}
					else if(e2->type==FLOAT)
						{
							val2=e2->val;
							type2=FLOAT;
						}
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK||argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					if(argTypeCheck(root,2,INT,ELEM)==OK)
						{
							num2=atoi(root->right->right->left->elem);
							type2=INT;
						}
					else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
						{
							val2=atof(root->right->right->left->elem);
							type2=FLOAT;
						}
				}
			else
				return ARGTYPEERR;
			if(type1==INT&&type2==INT)
				{
					int bool;
					if(num1>num2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==INT&&type2==FLOAT)
				{
					int bool;
					if(num1>val2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					int bool;
					if(val1>val2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					int bool;
					if(val1>num2)
						bool=TRUE;
					else
						bool=FALSE;
					err=setTree(root,boolToStr(bool),BOOL,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}

		}
	IFFUNC(root,calPoly)
		{
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			elem *e1,*e2;
			poly *p;
			double x;
			int pFree=0;
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==POLY)
						{
							p=e1->head;
						}
				}
			else if(argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					pFree=1;
					p=strToPoly(root->right->left->elem);
					IF_NULL_RET_OF(p);
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT&&e2->type!=FLOAT)
						return ELEMTYPEERR;
					if(e2->type==INT)
						{
							x=(double)e2->num;
						}
					else
						x=e2->val;
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK)
				{
					
					x=(double)atoi(root->right->right->left->elem);
				}
			else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					
					x=atof(root->right->right->left->elem);
				}

			else
				return ARGTYPEERR;
			char strRet[STR_SIZE];
			sprintf(strRet,"%lf",calPoly(p,x));
			if(pFree)
				freePoly(p);
			err=setTree(root,strRet,FLOAT,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,expPoly)
		{
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			elem *e1,*e2;
			poly *p;
			int x;
			int pFree=0;
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==POLY)
						{
							p=e1->head;
						}
				}
			else if(argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					pFree=1;
					p=strToPoly(root->right->left->elem);
					IF_NULL_RET_OF(p);
				}
			else
				return ARGTYPEERR;
			err=argTypeCheck(root,2,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e2=findElem(ehead,root->right->right->left->elem);
					if(e2==NULL)
						return ELEMNOTFOUND;
					if(e2->type!=INT)
						return ELEMTYPEERR;
					x=e2->num;
				}
			else if(argTypeCheck(root,2,INT,ELEM)==OK)
				{
					
					x=atoi(root->right->right->left->elem);
				}
			else
				return ARGTYPEERR;
			poly *tmp=expPoly(p,x);
			//printf("x:%i\n",x);
			//foreachPoly(p,printPoly);
			//foreachPoly(tmp,printPoly);
			//printf("\n");
			if(tmp==NULL)
				{
					if(pFree)
						freePoly(p);
					return OF;
				}
			char *strRet=polyToStr(tmp);
			if(strRet==NULL)
				{
					if(pFree)
						freePoly(p);
					freePoly(tmp);
					return OF;
				}
			err=setTree(root,strRet,POLY,ELEM);
			if(pFree)
				freePoly(p);
			freePoly(tmp);
			free(strRet);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,sPoly)
		{
			err=argCheck(root,1);
			IF_NOT_OK_RET(err);
			elem *e1;
			poly *p;
			int pFree=1;
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==POLY)
						{
							p=clonePoly(e1->head);
							if(p==NULL)
								return OF;
						}
				}
			else if(argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					pFree=1;
					p=strToPoly(root->right->left->elem);
					IF_NULL_RET_OF(p);
				}
			else
				return ARGTYPEERR;

			s1Poly(p);
			
			//printf("x:%i\n",x);
			//foreachPoly(p,printPoly);
			//foreachPoly(tmp,printPoly);
			//printf("\n");
			if(p==NULL)
				{
					if(pFree)
						freePoly(p);
					return OF;
				}
			char *strRet=polyToStr(p);
			if(strRet==NULL)
				{
					if(pFree)
						freePoly(p);
					return OF;
				}
			err=setTree(root,strRet,POLY,ELEM);
			if(pFree)
				freePoly(p);
			free(strRet);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,dPoly)
		{
			err=argCheck(root,1);
			IF_NOT_OK_RET(err);
			elem *e1;
			poly *p;
			int pFree=1;
			err=argTypeCheck(root,1,UNSET,ELEM);
			if(err!=OK&&err!=ARGTYPEERR)
				return err;
			if(err==OK)
				{
					e1=findElem(ehead,root->right->left->elem);
					if(e1==NULL)
						return ELEMNOTFOUND;
					if(e1->type!=POLY)
						return ELEMTYPEERR;
					if(e1->type==POLY)
						{
							p=clonePoly(e1->head);
							if(p==NULL)
								return OF;
						}
				}
			else if(argTypeCheck(root,1,POLY,ELEM)==OK)
				{
					pFree=1;
					p=strToPoly(root->right->left->elem);
					IF_NULL_RET_OF(p);
				}
			else
				return ARGTYPEERR;

			dPoly(p);
			
			//printf("x:%i\n",x);
			//foreachPoly(p,printPoly);
			//foreachPoly(tmp,printPoly);
			//printf("\n");
			if(p==NULL)
				{
					if(pFree)
						freePoly(p);
					return OF;
				}
			char *strRet=polyToStr(p);
			if(strRet==NULL)
				{
					if(pFree)
						freePoly(p);
					return OF;
				}
			err=setTree(root,strRet,POLY,ELEM);
			if(pFree)
				freePoly(p);
			free(strRet);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,clear)
		{
			if(argNum(root)!=0)
				return ARGNUMERR;
			system("clear");
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
			
		}
	IFFUNC(root,ret)
		{
			if(argNum(root)!=1)
				return ARGNUMERR;
			
		}
	//not a basic func
	//find the func defined
	func *f=findFunc(fhead,root->left->elem);
	if(f==NULL)
		return FUNCNAMEERR;
	if(f->argNum!=argNum(root))
		return ARGNUMERR;
	tree *mirror;
	err=copyTree(f->eval,&mirror);
	IF_NOT_OK_RET(err);
	err=mirrorSetup(mirror,f,root);
	IF_NOT_OK_RET(err);
	err=eval(mirror,fhead,ehead);
	IF_NOT_OK_RET(err);
	return OK;
}
int loadFile(char *path,func *fhead,elem *ehead)
{
	FILE *fp;
	char line[STR_SIZE];
	if((fp=fopen(path,"r"))==NULL)
		{
			return ERR;
		}
	while(!feof(fp))
		{
			char line[512];
			fgets(line,512,fp);
			printf("\e[1;33mline:%s\e[0m",line);
			if(gramCheck(line)!=OK)
				{
					continue;
				}
			printf("");
			tree *root=treeInit();			


			if(plantTree(root,line,ehead)!=OK)
				{
					continue;
				}

			int err=eval(root,fhead,ehead);
			//printf("\n\n%s\n\n",statToStr(err));
			if(err==EXIT)
				return 0;
			freeTree(root);
		}


	fclose(fp);
	return OK;
}
	
int main()
{
	func *fhead=funcInit();
	elem *ehead=elemInit();
	loadFile("./init.ml",fhead,ehead);
	while(TRUE)
		{
			printf("\n");
			char *line=readline("mkq> ");
			printf("\n");
			add_history(line);
			if(gramCheck(line)!=OK)
				{
					printf("\e[1;31mgramCheck not ok!\e[0m\n");
					free(line);
					continue;
				}
			
			tree *root=treeInit();			


			if(plantTree(root,line,ehead)!=OK)
				{
					printf("plant not OK\n");
					free(line);
					continue;
				}

			int err=eval(root,fhead,ehead);
			printf("\n\n\e[1;31m%s\e[0m\n\n",statToStr(err));
			if(err==EXIT)
				return 0;
			freeTree(root);
			free(line);
		}
}
