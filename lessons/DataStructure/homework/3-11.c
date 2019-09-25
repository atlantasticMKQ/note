#include<stdio.h>
#include<stdlib.h>
struct node
{
	struct node *next;
	int index;
};
struct node *init(int num)
{
	struct node *head=(struct node *)malloc(sizeof(struct node));
	head->index=0;
	struct node *tmp=head;
	for(int i=1;i<num;i++)
		{
			tmp->next=(struct node *)malloc(sizeof(struct node));
			tmp=tmp->next;
			tmp->index=i;
		}
	tmp->next=head;
	return head;
}
int delfront(struct node *node)
{
	struct node *tmp;
	for(tmp=node;tmp->next->next!=node;tmp=tmp->next);
	free(tmp->next);
	tmp->next=node;
	return 0;
}
int main()
{
	struct node *head=init(15);
	struct node *tmp=head;
	for(int i=0;i<20;i++)
		{
			printf("%i\n",tmp->index);
			tmp=tmp->next;
		}
	delfront(head);
	tmp=head;
	for(int i=0;i<20;i++)
		{
			printf("%i\n",tmp->index);
			tmp=tmp->next;
		}
}
