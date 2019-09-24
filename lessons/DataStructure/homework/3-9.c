#include<stdio.h>
#include<stdlib.h>
struct node
{
	int index;
	struct node *next;
	struct node *front;
};
int printNode(struct node *toprint)
{
	printf("%i<->\n",toprint->index);
	//printf("%i<-%i\n",toprint->front->index,toprint->index);
	return 0;
}
int foreach(struct node *link,int (* func)(struct node *todo))
{
	struct node *tmp;
	for(tmp=link;tmp->next!=NULL;tmp=tmp->next)
		{
			func(tmp->next);
		}
	printf("\n");
	return 0;
}
int destory(struct node *head)
{
	if(head->next==NULL)
		{
			free(head);
			return 0;
		}
	else
		{
			destory(head->next);
			free(head);
		}
	
}
struct node *initLink(int num)
{
	struct node *head=(struct node *)malloc(sizeof(struct node));
	if(head==NULL)
		{
			return NULL;
		}
	head->front=NULL;
	head->next=NULL;
	head->index=0;
	struct node *tail=head;
	for(int i=0;i<num;i++)
		{
			tail->next=(struct node *)malloc(sizeof(struct node));
			if(tail->next==NULL)
				{
					destory(head);
					return NULL;
				}
			tail->next->front=tail;
			tail=tail->next;
			tail->next=NULL;
			tail->index=i+1;
		}
	return head;
}
int isEven(int num)
{
	if(num%2==0)
		return 1;
	else
		return 0;
}
int reverse(struct node *link)
{
	struct node *tmp;
	for(tmp=link;tmp->next!=NULL;tmp=tmp->next)
		{
			tmp->next->front=tmp->next->next;
		}
	tmp->front=NULL;
	link->front=link->next;
	for(tmp=link;tmp->front!=NULL;tmp=tmp->front)
		{
			tmp->front->next=tmp;
		}
	link->next->next=NULL;
	tmp->front=link;
	link->next=tmp;
	link->front=NULL;
	return 0;
}
struct node *add(struct node *link1,struct node *link2)
{
	struct node *tmp;
	for(tmp=link1;tmp->next!=NULL;tmp=tmp->next);
	tmp->next=link2->next;
	link2->next->front=tmp;
	free(link2);
	return link1;
}
	       
int division(struct node *link)
{
	struct node *odd=(struct node *)malloc(sizeof(struct node));
	struct node *even=(struct node *)malloc(sizeof(struct node));
	struct node *oddp=odd;
	struct node *evenp=even;
	evenp->next=NULL;
	evenp->front=NULL;
	oddp->next=NULL;
	oddp->front=NULL;
	int i=0;
	for(struct node *tmp=link;tmp->next!=NULL;tmp=tmp->next)
		{
			i++;
			if(!isEven(i))
				{
					oddp->next=tmp->next;
					oddp->next->front=oddp;
					oddp=oddp->next;

				}
			else
				{
					evenp->next=tmp->next;
					evenp->next->front=evenp;
					evenp=evenp->next;
				}
			
		}
	
	oddp->next=NULL;
	evenp->next=NULL;
	reverse(even);
	add(odd,even);
	//foreach(even,printNode);
	foreach(odd,printNode);
	
}
	
					
					
					
					
			
			
			
int main()
{
	struct node *link=initLink(15);
	foreach(link,printNode);
	division(link);
	destory(link);
}
			
