#include<stdio.h>
#include<stdlib.h>
#include<editline.h>
#define OK	0
#define POPEND	1
#define OF	2
#define MALLOC(type) (type *)malloc(sizeof(type))
#define MALLOC_NUM(type,num) (type *)malloc(sizeof(type)*(num))
#define TOTAIL(head,tail) do{for(tail=head;tail->next!=NULL;tail=tail->next);}while(0)
#define IF_NULL_RET_NULL(ptr) do{if(ptr==NULL)return NULL;}while(0)
#define IF_NULL_RET_OF(ptr) do{if(ptr==NULL)return OF;}while(0)
#define CK_ERR do{if(err!=OK){csFree(opStack);printf("err:%i\n",err);return err;}}while(0)
#define TRUE 	1
#define FALSE 	0



struct charStack
{
	char c;
	struct charStack *next;
};

typedef struct charStack cs;

cs *csInit()
{
	cs *head;
	head=MALLOC(cs);
	IF_NULL_RET_NULL(head);
	head->c='\0';
	head->next=NULL;
	return head;
}
int csPush(cs *head,char c)
{
	cs *tail;
	TOTAIL(head,tail);

	tail->next=MALLOC(cs);
	tail=tail->next;
	IF_NULL_RET_OF(tail);
	tail->next=NULL;
	tail->c=c;
       	//printf("push:%c\n",c);
	return OK;
}
int csPop(cs *head,char *c)
{
	cs *tail;
	TOTAIL(head,tail);
	if(head==tail)
		return POPEND;
	cs *tailFront;
	for(tailFront=head;tailFront->next->next!=NULL;tailFront=tailFront->next);
	tailFront->next=NULL;
	*c=tail->c;
	free(tail);
	//printf("pop:%c\n",*c);
	return OK;
}
int csFree(cs *head)
{

	if(head==NULL)
		return OK;
	csFree(head->next);
	free(head);
	return OK;
}
int ifEmptyCs(cs *head)
{
	cs *tail;
	TOTAIL(head,tail);
	if(head==tail)
		return TRUE;
	return FALSE;
}
int checkTop(cs *head,char *top)
{
	char c;
	int err=csPop(head,&c);
	if(err!=OK)
		return err;
       
	*top=c;
	err=csPush(head,c);
	if(err!=OK)
		return err;
	return OK;
}
	
int ifLetter(char c)
{
	if((c<='z'&&c>='a')||(c<='Z'&&c>='A'))
		return TRUE;
	return FALSE;
}
#include<string.h>
int ifRe(char *line)
{
	cs *stack=csInit();
	int err;
	for(char *c=line;*c!='#';c++)
		{
			err=csPush(stack,*c);
			if(err!=OK)
				return 0;
		}
	char c;
	char *cpy=MALLOC_NUM(char,128);
	char *ptr=cpy;
	for(err=csPop(stack,&c);err!=POPEND;)
		{
			*ptr=c;
			ptr++;
			err=csPop(stack,&c);
		}
	*ptr='#';
	*(ptr+1)='\0';
	if(strcmp(cpy,line)==0)
		return 1;
	return 0;
}
int main()
{
	char *line=readline("mkq->");
	printf("%i\n",ifRe(line));
}
	
