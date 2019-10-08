#include<stdio.h>
#include<stdlib.h>
struct ringNode
{
	int elem;
	struct ringNode *next;
};
typedef struct ringNode ring;
#define OK	0
#define OF	1
#define RINGEND	2
#define MALLOC(type,num) (type *)malloc(sizeof(type)*(num))
#define IF_NULL_RET_NULL(ptr) do{if(ptr==NULL)return NULL;}while(0)
#define IF_NULL_RET_OF(ptr) do{if(ptr==NULL) return OF;}while(0)
ring *ringInit()
{
	ring *head=MALLOC(ring,1);
	IF_NULL_RET_NULL(head);
	head->next=head;
	head->elem=0;
	return head;
}
int addRing(ring *head,int elem)
{
	ring *tail=MALLOC(ring,1);
	IF_NULL_RET_OF(tail);
	tail->elem=elem;
	ring *tmp;
	for(tmp=head;tmp->next!=head;tmp=tmp->next);
	tmp->next=tail;
	tail->next=head;
	return OK;
}
int outRing(ring *head,int *elem)
{
	if(head->next==head)
		return RINGEND;
	*elem=head->next->elem;
	ring *tmp=head->next;
	head->next=tmp->next;
	free(tmp);
	return OK;
}
int main()
{
	ring *head=ringInit();
	int err;
	int elem;
	for(int i=0;i<10;i++)
		{
			err=addRing(head,i);
			printf("err=%i\n",err);
		}
	for(ring *tmp=head;tmp->next!=head;tmp=tmp->next)
		printf("%i->",tmp->next->elem);
	printf("\n");
	for(int i=0;i<9;i++)
		{
			err=outRing(head,&elem);
			printf("err=%i\nelem=%i\n",err,elem);
		}
}
	
	
	
