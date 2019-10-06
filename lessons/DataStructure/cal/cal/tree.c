#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"lib/types.h"
#include"lib/status.h"

//Status
#define OK	0	//ok
#define ERR	1	//unknown errors
#define OF	2	//overfloat of alloc
#define UST	3	//unset
#define BDAG	4	//bad arg
#define POPEND	5
#define MATCHERR	6
#define NUMERR	7
#define NAMEERR	8

//Basic operations
#define DEF	0
#define ADD	1
#define SUB	2
#define IF	3
#define WHILE	4
#define LEQ	5
#define LE	6
#define BEQ	7
#define BE	8
#define EQ	9
#define LET	10

//Elem types
#define FUNC	0
#define INT	1
#define BOOL	2
#define STR	3
#define POLY	4

#define TRUE	1
#define FALSE	0

#define FUNC	0
#define EVAL	1
#define ELEM	2
#define LINK	3
struct treeNode
{
	int type;
	int state;

	int func;
	int num;
	int bool;
	char *str;
	//poly *poly;

	struct treeNode *left;
	struct treeNode *right;
};
typedef struct treeNode tn;

struct elemNode
{
	int type;
	char *name;

	int num;
	int bool;
	char *str;
	//poly *poly;

	struct elemNode *next;
};
typedef struct elemNode elem;

struct argNode
{
	int argIndex;
	int type;

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


struct charStack
{
	char c;
	struct charStack *next;
};

typedef struct charStack cs;
#define MALLOC(type) (type *)malloc(sizeof(type))
#define TOTAIL(head,tail) do{for(tail=head;tail->next!=NULL;tail=tail->next);}while(0)
#define IF_NULL_RET_NULL(ptr) do{if(ptr==NULL)return NULL;}while(0)
#define IF_NULL_RET_OF(ptr) do{if(ptr==NULL)return OF;}while(0)
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
int ifNum(char c)
{
	if(c<='9'&&c>='0')
		return TRUE;
	return FALSE;
}
int ifLetter(char c)
{
	if((c<='z'&&c>='a')||(c<='Z'&&c>='A'))
		return TRUE;
	return FALSE;
}
int ifBlank(char c)
{
	if(c=='\n'||c==' ')
		return TRUE;
	return FALSE;
}
int ifLeft(char c)
{
	if(c=='('||c=='{'||c=='[')
		return TRUE;
	return FALSE;
}
int ifRight(char c)
{
	if(c==')'||c=='}'||c==']')
		return TRUE;
	return FALSE;
}
int ifMatch(char c1,char c2)
{
	if(!(ifLeft(c1)&&ifRight(c2)))
		return FALSE;
	if(c1=='('&&c2!=')')
		return FALSE;
	if(c1=='['&&c2!=']')
		return FALSE;
	if(c1=='{'&&c2!='}')
		return FALSE;
	return TRUE;
}
#define IF_STACK_OF(stack) do{if(err==OF){printf("stack OF\n");csFree(stack);return OF;}}while(0)
int bracCheck(char *str)
{
	cs *stack=csInit();
	IF_NULL_RET_OF(stack);
	int err;
	for(char *c=str;*c!='\0';c++)
		{
			if(*c=='"')
				{
					printf("here is a \"\n");
					char tmp;
					err=csPop(stack,&tmp);
					printf("pop:%c \n",tmp);
					if(err==POPEND)
						return NAMEERR;
					if(tmp!=*c)
						{
										
							
							err=csPush(stack,tmp);
							printf("push:%c\n",tmp);
							IF_STACK_OF(stack);
							err=csPush(stack,*c);
							printf("push:%c\n",*c);
							IF_STACK_OF(stack);
						}
				
				
				}
							
			if(ifLeft(*c))
				{
					printf("left brac '%c'\n",*c);
					err=csPush(stack,*c);
					printf("push:%c\n",*c);
					if(err==OF)
						{
							printf("stack OF\n");
							csFree(stack);
							return OF;
						}
				}
			if(ifRight(*c))
				{
					printf("right brac '%c'\n",*c);
					char poped;
					err=csPop(stack,&poped);
					printf("pop:%c \n",poped);
					if(!ifMatch(poped,*c))
						{
							csFree(stack);
							return MATCHERR;
						}
				}
		}
	if(ifEmptyCs(stack))
		{
			csFree(stack);
			return OK;
		}
	return MATCHERR;
}
int ifCharInLaw(char c)
{
	if(c=='{'||c=='}'||c=='('||c==')'||c=='['||c==']')
		return TRUE;
	if(c<='z'&&c>='a')
		return TRUE;
	if(c<='Z'&&c>='A')
		return TRUE;
	if(c==' ')
		return TRUE;
	if(c=='"')
		return TRUE;
	return FALSE;
}
int gramCheck(char *str)
{
	for(char *c=str;*c!='\0'&&*c!='\n';c++)
		{
			if(ifCharInLaw(*c)==FALSE)
				return NAMEERR;
		}
			
	if(bracCheck(str)==MATCHERR)
		{
			return MATCHERR;
		}
	return OK;
}
	
	
					
							
					
					
int main()
{
	char line[64];
	scanf("%s",line);
	int i=gramCheck(line);
	printf("%i\n",i);
}
