struct argNode
{
	int index;
	int type;
	char *name;
	struct argNode *next;
};
typedef struct argNode arg;

struct funcNode
{
	int retType;
	char *name;
	int argNum;

	struct funcNode *next;
	struct argNode *args;
};
typedef struct funcNode func;
arg *argInit()
{
	arg *head;
	head=MALLOC(arg);
	IF_NULL_RET_NULL(head);
	head->type=FUNC;
	head->index=0;
	head->name=NULL;
	head->next=NULL;
	return head;
}
int addArg(arg *head,int type,char *name)
{
	arg *tail;
	TOTAIL(head,tail);
	int index=tail->index+1;
	tail->next=MALLOC(arg);
	tail=tail->next;
	IF_NULL_RET_OF(tail);
	tail->next=NULL;
	tail->type=type;
	tail->index=index;
	tail->name=name;
	return OK;
}
int *freeArg(arg *head)
{
	if(head==NULL)
		{
			return OK;
		}
	freeArg(head->next);
	free(head->name);
	free(head);
	return OK;
}
arg *cloneArg(arg *head)
{
	arg *clone=argInit();
	IF_NULL_RET_NULL(clone);
	arg *tmp;
	int err;
	char *name;
	for(tmp=head;tmp->next!=NULL;tmp=tmp->next)
	{
		name=copyStr(tmp->name);
		if(tmp!=head&&name==NULL)
			return NULL;
		err=addArg(clone,tmp->type,name);
		if(err!=OK)
		{
			freeArg(clone);
			return NULL;
		}
	}
	return clone;
}

int printArg(arg thisArg)
{
	printf("%s %s,",typeToStr(thisArg.type),thisArg.name);
	return OK;
}
int foreachArg(arg *head,int (* method)(arg thisArg))
{
	for(arg *tmp=head;tmp->next!=NULL;tmp=tmp->next)
	{
		method(*(tmp->next));
	}
	return OK;
}
func *funcInit()
{
	func *head;
	head=MALLOC(func);
	IF_NULL_RET_NULL(head);
	head->retType=FUNC;
	head->name=NULL;
	head->argNum=0;
	head->next=NULL;
	head->args=NULL;
	return head;
}
int addFunc(func *head,int retType,char *name,int argNum,arg *argHead)
{
	func *tail;
	TOTAIL(head,tail);
	tail->next=MALLOC(func);
       
	IF_NULL_RET_OF(tail->next);
	tail->next->retType=retType;
	tail->next->name=MALLOC_NUM(char,FUNC_NAME_SIZE);
	if(tail->next->name==NULL)
		{
			free(tail->next);
			tail->next=NULL;
			return OF;
		}
	strcpy(tail->next->name,name);
	tail->next->argNum=argNum;
	tail->next->args=argHead;
	tail->next->next=NULL;
	return OK;
}
int printFunc(func thisFunc)
{
	printf("%s %s(",typeToStr(thisFunc.retType),thisFunc.name);
	foreachArg(thisFunc.args,printArg);
	printf(")\n");
	return OK;
}
int foreachFunc(func *head,int (*method)(func func))
{
	for(func *tmp=head;tmp->next!=NULL;tmp=tmp->next)
	{
		method(*(tmp->next));
	}
	return OK;
}
int freeFunc(func *head)
{
	if(head==NULL)
		return OK;
	freeFunc(head->next);
	freeArg(head->args);
	free(head->name);
	free(head);
	return OK;
}
func *cloneFunc(func *head)
{
	func *clone;
	func *tmpF;
	arg *tmpA;
	int err;
	char *name;
	clone=funcInit();
	IF_NULL_RET_NULL(clone);
	for(tmpF=head;tmpF->next!=NULL;tmpF=tmpF->next)
		{
			tmpA=cloneArg(tmpF->next->args);
			if(tmpA==NULL)
				{
					freeFunc(clone);
					return NULL;
				}
			name=copyStr(tmpF->name);
			if(name==NULL)
				{
					freeFunc(clone);
					freeArg(tmpA);
					return NULL;
				}
			err=addFunc(clone,tmpF->retType,name,tmpF->argNum,tmpA);
			if(err!=OK)
				{
					freeFunc(clone);
					return NULL;
				}
				
		}
	return clone;
}
func *findFunc(func *head,char *name)
{
	func *tmp;
	for(tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			if(strcmp(tmp->next->name,name)==0)
				{
					return tmp->next;
				}
		}
	return NULL;
}
