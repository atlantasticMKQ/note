struct elemNode
{
	int type;
	int num;
	int bool;
	char *str;
	//poly* poly;
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
	//head->poly=NULL;
	head->val=0;
	head->name=NULL;
	head->next=NULL;
	return head;
}
int addElem(elem *head,int type,int num,int bool,char *str,double val,char *name)
{
	elem *tail;
	TOTAIL(head,tail);
	tail->next=MALLOC(elem);
	IF_NULL_RET_OF(tail->next);
	tail=tail->next;
	tail->type=type;
	tail->num=num;
	tail->bool=bool;
	tail->str=str;
	tail->val=val;
	tail->name=name;
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
						
	

				
