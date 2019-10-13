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
#include"lib/plantTree.h"

#define EXIT	100
	
		



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
	IFFUNC(root,for)
		{
			err=argCheck(root,2);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,2,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			tree *evalBack,*tmp;
			err=copyTree(root->right,&evalBack);
			IF_NOT_OK_RET(err);
			while(TRUE)
				{
					freeTree(root->right);
					err=copyTree(evalBack,&tmp);
					root->right=tmp;
					IF_NOT_OK_RET(err);
					//eval arg 1
					if(argTypeCheck(root,1,UNSET,EVAL)==OK)
						{
							err=eval(root->right->left,fhead,ehead);
							IF_NOT_OK_RET(err);
						}
					if(argTypeCheck(root,1,BOOL,ELEM)==OK)
						{
							if(strcmp(root->right->left->elem,"TRUE")==0)
								{
									err=eval(root->right->right->left,fhead,ehead);
									IF_NOT_OK_DO_RET(err,freeTree(evalBack));
									continue;
								}
							else
								break;
						}
					else if(argTypeCheck(root,1,UNSET,ELEM)==OK)
						{
							elem *e=findElem(ehead,root->right->left->elem);
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
									err=eval(root->right->right->left,fhead,ehead);
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

	IFFUNC(root,if)
		{
			err=argCheck(root,3);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,2,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			err=argTypeCheck(root,3,UNSET,EVAL);
			IF_NOT_OK_RET(err);
			if(argTypeCheck(root,1,UNSET,EVAL)==OK)
				{
					err=eval(root->right->left,fhead,ehead);
					IF_NOT_OK_RET(err);
				}
			if(argTypeCheck(root,1,BOOL,ELEM)==OK)
				{
					if(strcmp(root->right->left->elem,"TRUE")==0)
						{
							err=eval(root->right->right->left,fhead,ehead);
							IF_NOT_OK_RET(err);
							err=setTree(root,root->right->right->left->elem,root->right->right->left->type,root->right->right->left->state);
							IF_NOT_OK_RET(err);
							return OK;
						}
					else
						{
							err=eval(root->right->right->right->left,fhead,ehead);
							IF_NOT_OK_RET(err);
							err=setTree(root,root->right->right->right->left->elem,root->right->right->right->left->type,root->right->right->right->left->state);
							IF_NOT_OK_RET(err);
							return OK;
						}
				}
			if(argTypeCheck(root,1,UNSET,ELEM)==OK)
				{
					elem *e=findElem(ehead,root->right->left->elem);
					if(e==NULL)
						return ELEMNOTFOUND;
					if(e->type!=BOOL)
						{
							return ARGTYPEERR;
						}
					if(e->bool==TRUE)
						{
							err=eval(root->right->right->left,fhead,ehead);
							IF_NOT_OK_RET(err);
							err=setTree(root,root->right->right->left->elem,root->right->right->left->type,root->right->right->left->state);
							IF_NOT_OK_RET(err);
							return OK;
						}
					else
						{
							err=eval(root->right->right->right->left,fhead,ehead);
							IF_NOT_OK_RET(err);
							err=setTree(root,root->right->right->right->left->elem,root->right->right->right->left->type,root->right->right->right->left->state);
							IF_NOT_OK_RET(err);
							return OK;
						}
				}
			else
				return argTypeCheck(root,1,UNSET,ELEM);
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	//eval args
	for(tree *tmp=root->right;tmp!=NULL;tmp=tmp->right)
		{
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
	IFFUNC(root,print)
		{
			int aNum=argNum(root);
			tree *tmp=root->right;
			for(int i=0;i<aNum;i++)
				{
					if(argTypeCheck(root,i+1,UNSET,ELEM)==OK)
						{
							elem *e=findElem(ehead,tmp->left->elem);
							if(e==NULL)
								{
									printf("(null)");
								}
							else
								{
									switch(e->type)
										{
										case INT:
											printf("%i",e->num);
											break;
										case FLOAT:
											printf("%lf",e->val);
											break;
										case BOOL:
											printf("%s",boolToStr(e->bool));
											break;
										case STR:
											printf("%s",e->str);
											break;
										}
								}
						}
					else 
						{
							printf("%s",tmp->left->elem);
									
						}
					tmp=tmp->right;
				}
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,br)
		{
			err=argCheck(root,0);
			IF_NOT_OK_RET(err);
			printf("\n");
			err=setTree(root,"TRUE",BOOL,ELEM);
			IF_NOT_OK_RET(err);
			return OK;
		}
	IFFUNC(root,multi)
		{
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
					err=addElem(ehead,type,num,bool,str,val,name);
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
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			
			else if(argTypeCheck(root,2,INT,ELEM)==OK)
				{
					type=INT;
					num=atoi(elem);
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,FLOAT,ELEM)==OK)
				{
					type=FLOAT;
					val=atof(elem);
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,POLY,ELEM)==OK)
				{
					type=POLY;
					delElem(ehead,eBack);
					err=addElem(ehead,type,num,bool,str,val,name);
					IF_NOT_OK_RET(err);
				}
			else if(argTypeCheck(root,2,UNSET,ELEM)==OK)
				{
					e=findElem(ehead,root->right->right->left->elem);
					if(e==NULL)
						return ELEMNOTFOUND;
					if(e==eBack)
						return OK;
					err=addElem(ehead,e->type,e->num,e->bool,e->str,e->val,name);
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

		}
	IFFUNC(root,cross)
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

		}
	IFFUNC(root,sub)
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
					//printf("double:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					double re=val1-(double)num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%lf",re);
					//printf("double:%s\n",reStr);
					err=setTree(root,reStr,FLOAT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}

		}
	IFFUNC(root,div)
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

		}
	IFFUNC(root,mod)
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
					int re=num1%(int)val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==FLOAT)
				{
					int re=(int)val1%(int)val2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}
			else if(type1==FLOAT&&type2==INT)
				{
					int re=(int)val1%num2;
					char reStr[STR_SIZE];
					sprintf(reStr,"%i",re);
					//printf("int:%s\n",reStr);
					err=setTree(root,reStr,INT,ELEM);
					IF_NOT_OK_RET(err);
					return OK;
				}

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
	//not a basic func
	func *f=findFunc(fhead,root->left->elem);
	if(f==NULL)
		return FUNCNAMEERR;
	printf("argNum:%i\n",f->argNum);

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
			fgets(line,STR_SIZE,fp);
			if(gramCheck(line)!=OK)
				{
					continue;
				}
			
			tree *root=treeInit();			


			if(plantTree(root,line,ehead)!=OK)
				{
					continue;
				}
			int err=eval(root,fhead,ehead);
			//printf("%s\n",statToStr(err));
			//printf("\n");
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
	//	loadFile("./init.ml",fhead,ehead);
	while(TRUE)
		{
			char *line=readline("mkq> ");
			printf("\n");
			if(gramCheck(line)!=OK)
				{
					ERROR("gramCheck not ok!\n");
					free(line);
					continue;
				}
			
			tree *root=treeInit();			


			if(plantTree(root,line,ehead)!=OK)
				{
					ERROR("plant not OK\n");
					free(line);
					continue;
				}

			int err=eval(root,fhead,ehead);
			printf("%s\n",statToStr(err));
			printf("\n");
			if(err==EXIT)
				return 0;
			freeTree(root);
			free(line);
		}
}
