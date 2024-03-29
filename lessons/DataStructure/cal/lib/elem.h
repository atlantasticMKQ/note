struct elemNode
{
	int type;
	int num;
	int bool;
	char *str;
	double val;
	char *name;
	struct elemNode *next;
};
typedef struct elemNode elem;
elem *elemInit()
{
	elem *head=MALLOC(elem);
	IF_NULL_RET_NULL(head);
	head->type=UNSET;
	head->num=0;
	head->bool=FALSE;
	head->str=NULL;
	head->val=0;
	head->name=NULL;
	head->next=NULL;
	return head;
}
int addElem(elem *head,int type,int num,int bool,char *str,double val,char *name)
{
	elem *tail;
	TOTAIL(head,tail);
	IF_NULL_RET_OF(tail);
	tail->next=MALLOC(elem);
	elem *tailH=tail;
	IF_NULL_RET_OF(tail->next);
	tail=tail->next;
	tail->type=type;
	tail->num=num;
	tail->bool=bool;
	if(type==STR)
		{
			tail->str=MALLOC_NUM(char,STR_SIZE);
			if(tail->str==NULL)
				{
					free(tail);
					tailH->next=NULL;
					return OF;
				}
			strcpy(tail->str,str);
		}
	else
		tail->str=NULL;
	tail->val=val;
	tail->name=MALLOC_NUM(char,FUNC_NAME_SIZE);
	if(tail->name==NULL)
		{
			free(tail->str);
			free(tail);
			tailH->next=NULL;
			return OF;
		}
	strcpy(tail->name,name);
	tail->next=NULL;
	return OK;
}
elem *findElem(elem *head,char *name)
{
	for(elem *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			if(strcmp(name,tmp->next->name)==0)
				{
					return tmp->next;
				}
		}
	return NULL;
}
int delElem(elem *ehead,elem *toDel)
{
	elem *tmp;
	for(tmp=ehead;tmp->next!=toDel&&tmp->next!=NULL;tmp=tmp->next);
	if(tmp->next==NULL)
		return ELEMNOTFOUND;
	tmp->next=toDel->next;
	free(toDel->name);
	free(toDel->str);
	free(toDel);
	return OK;
}
		 
int freeElem(elem *elemHead)
{
	if(elemHead==NULL)
		return OK;
	freeElem(elemHead->next);
	free(elemHead->name);
	free(elemHead->str);
	free(elemHead);
	return OK;
}
elem *copyElem(elem *head)
{
	elem *clone=elemInit();
	IF_NULL_RET_NULL(clone);
	int err;
	for(elem *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			err=addElem(clone,tmp->next->type,tmp->next->num,tmp->next->bool,tmp->next->str,tmp->next->val,tmp->next->name);
			if(err!=OK)
				{
					freeElem(clone);
					return NULL;
				}
		}
	return clone;
}
int printElem(elem *thisElem)
{
	if(thisElem==NULL)
		return OK;
	switch(thisElem->type)
		{
		case UNSET:
			printf("%s:UNSET\n",thisElem->name);
			break;
		case INT:
			printf("%s:%i:INT\n",thisElem->name,thisElem->num);
			break;
		case FLOAT:
			printf("%s:%lf:FLOAT\n",thisElem->name,thisElem->val);
			break;
		case BOOL:
			printf("%s:%s:BOOL\n",thisElem->name,boolToStr(thisElem->bool));
			break;
		case STR:
			printf("%s:%s:STR\n",thisElem->name,thisElem->str);
			break;
		default:
			printf("unknown type val\n");
			
		}
	return OK;
}
int foreachElem(elem *head,int (* method)(elem *thisElem))
{
	for(elem *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		method(tmp->next);
	return OK;
}

	

				
