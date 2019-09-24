#include<stdio.h>
#include<stdlib.h>

#define NAMESIZE 32
struct var
{
	int exp;
	int constant;
	int rand;
	struct var *next;
};
	
struct varList
{
	char *name;
	struct var *thisVar;
	struct varList *next;
};

//creat just one node of a list
struct var *creatVarNode(int exp,int constant,int rand)
{
	struct var *node=(struct var *)malloc(sizeof(struct var));
	node->exp=exp;
	node->constant=constant;
	node->rand=rand;
	node->next=NULL;
	return node;
}
struct varList *creatVarListNode(char *name)
{
	struct varList *node=(struct varList *)malloc(sizeof(struct varList));
	node->name=name;
	node->thisVar=NULL;
	node->next=NULL;
	return node;
}

struct varList *initVarList()
{
	struct varList *head=creatVarListNode("head");
	return head;
}

struct var *varTop(struct var *head)
{
	struct var *top;
	for(top=head;top->next!=NULL;top=top->next);
	return top;
}
struct varList *varListTop(struct varList *head)
{
	struct varList *top;
	for(top=head;top->next!=NULL;top=top->next);
	return top;
}

int isNum(char c)
{
	if(c>='0'&&c<='9')
		return 1;
	else
		return 0;
}
int strToInt(char *str)
{
	char *p=str;
	int minusFlag=0;
	if(*p=='-')
		{
			minusFlag=1;
			p++;
		}
	if(!isNum(*p))
		return -1;
	int num=0;
	printf("char : %c\n",*p);
	for(;isNum(*p);p++)
		{
			num+=*p;
			num*=10;
		}
	num/=10;
	if(minusFlag)
		{
			num*=(-1);
		}
	return num;
}
struct varList *strToVar(struct varList *head,char *str)
{
	struct varList *top=varListTop(head);
	top->next=creatVarListNode(NULL);
	top=top->next;
	top->next=NULL;
	struct var *thisVar=creatVarNode(0,0,0);
	top->thisVar=thisVar;
	struct var *varTop=thisVar;
	for(char *p=str+1;*p!='"';)
		{
			//递增到"-"或数字
			for(;!(*p=='-'||isNum(*p)||*p=='C');p++);
			int rand=0;
			int constant=0;
			if(*p=='C')
				{
					rand=1;
				}
			else
				{
					constant=strToInt(p);
				}
			//递增到x
			for(;*p!='x';p++);
			//x的下一个该是数字
			p++;
			int exp=strToInt(p);
			//递增到下一个非数字字符
			for(;*p=='-'||isNum(*p);p++);
			//递增到下一个非空白字符
			for(;*p==' '||*p=='\t';p++);
			printf("MKQ\n");
			varTop->next=creatVarNode(exp,constant,rand);
			varTop=varTop->next;
			varTop->next=NULL;
		}
	return top;
}
int printfVar(struct var *thisVar)
{
	for(;thisVar->next!=NULL;thisVar=thisVar->next)
		{
			printf("%ix%i(%i) ",thisVar->next->constant,thisVar->next->exp,thisVar->next->rand);
		}
	printf("\n");
}
int forAllVar(struct varList *head,int (* func)(struct var *thisVar))
{
	for(;head->next!=NULL;head=head->next)
		{
			func(head->next->thisVar);
		}
	return 0;
}
			
int main()
{
	struct varList *head=initVarList();
	strToVar(head,"\"3x3 Cx4 5x5 -6x2 \"");
	forAllVar(head,printfVar);
}
			
	       






	
	
