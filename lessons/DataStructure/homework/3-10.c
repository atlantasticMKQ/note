#include<stdio.h>
#include<stdlib.h>
struct node
{
	int num;
	struct node *next;
};
int delnode(struct node *head,struct node *todel)
{
	struct node *tmp;
	for(tmp=head;tmp->next!=todel;tmp=tmp->next);
	tmp->next=todel->next;
	free(todel);
	return 0;
}
struct node *init(int num)
{
	struct node *head=(struct node *)malloc(sizeof(struct node));
	struct node *tmp=head;
	srand(time(NULL));
	for(int i=0;i<num;i++)
		{
			tmp->next=(struct node *)malloc(sizeof(struct node));
			tmp=tmp->next;
			tmp->num=rand()%100;
			tmp->next=NULL;
		}
	return head;
}
int max(struct node *head)
{
	int max=0;
	for(struct node *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			if(tmp->next->num>max)
				{
					max=tmp->next->num;
				}
		}
	return max;
}
int min(struct node *head)
{
	int min=head->next->num;
	for(struct node *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			if(tmp->next->num<min)
				min=tmp->next->num;
		}
	return min;
}
int removecopy(struct node *head)
{
	max=max(head);
	min=min(head);
	int array[max-min+1];
	for(int i=0;i<max-min+1;i++)
		{
			array[i]=0;
		}
	for(struct node *tmp=head;tmp->next!=NULL;tmp=tmp->next)
		{
			if(array[tmp->next->num-min]!=0)
				{
					delnode(head,tmp->next);
					if(tmp->next==NULL)
						break;
				}
			array[tmp->next->num-min]=1;
		}
}
int main()
{
	struct node *head=init(15);
	
	
      
