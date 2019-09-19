#include<stdio.h>
#include<stdlib.h>

//size of char array of options and elems
#define OP_SIZE 16
#define ELEM_SIZE 16

//Error code
#define OVERFLOAT -1
#define ERROR -2
#define POPEND -3
#define OK 0

//Elem type
#define ELEM 1
#define OP 2
#define HEAD 0

struct stack
{
	char *buffer;//there will be options or elems
	int elemType;
	struct stack *next;
};

struct stack *initStack()
{
	struct stack *stackHead=(struct stack *)malloc(sizeof(struct stack));
	if(stackHead==NULL)
		{
			return NULL;
		}
	stackHead->buffer=(char *)malloc(ELEM_SIZE*sizeof(char));
	if(stackHead->buffer==NULL)
		{
			return NULL;
		}
	sprintf(stackHead->buffer,"HEAD");
	stackHead->elemType=HEAD;
	stackHead->next=NULL;
	return stackHead;
}
struct stack *stackTop(struct stack *stackHead)
{
	struct stack *top;
	for(top=stackHead;top->next!=NULL;top=top->next);
	return top;
}
int pushStack(struct stack *stackHead,int elemType,char *buffer)
{
	//to the top of stack
	struct stack *top;
	top=stackTop(stackHead);
	
	top->next=(struct stack *)malloc(sizeof(struct stack));
	if(top->next==NULL)
		{
			return OVERFLOAT;
		}
	top=top->next;
	top->elemType=elemType;
	top->buffer=buffer;
	top->next=NULL;
	return OK;
}
int popStack(struct stack *stackHead,int *elemType,char **buffer)
{
	if(stackHead->next==NULL)
		{
			return POPEND;
		}
	struct stack *stackPointer;
	
	//find one in front of the one to poped
	for(stackPointer=stackHead;stackPointer->next->next!=NULL;stackPointer=stackPointer->next);
	
	*elemType=stackPointer->next->elemType;
	*buffer=stackPointer->next->buffer;
	free(stackPointer->next);
	stackPointer->next=NULL;
	return OK;
}
/*
#ifdef DEBUG
int forAllStack(struct stack *stackHead,int (* method)(struct stack *stackPointer))
{
	for(struct stack *pointer=stackHead;pointer->next!=NULL;pointer=pointer->next)
		{
			method(pointer);
		}
	if(stackHead->next==NULL)
		{
			method(stackHead);
		}
	return OK;
}
int printStack(struct stack *stackPointer)
{
	printf("\e[1;43m\e[1;39m%s\t%i\e[0m\e[0m\n",stackPointer->buffer,stackPointer->elemType);
	return OK;
}
//fake main for test
int main()
{
	struct stack *Q_stack=initStack();
	printf("%s\n",Q_stack->buffer);
	char *buffer=(char *)malloc(ELEM_SIZE*sizeof(char));
	sprintf(buffer,"MKQ");
	for(int i=0;i<5;i++)
		{
			pushStack(Q_stack,1,buffer);
		}
	forAllStack(Q_stack,printStack);
	int i;
	char *a;
	for(int j=0;j<3;j++)
		{
			popStack(Q_stack,&i,&a);
		}
	forAllStack(Q_stack,printStack);
}
     
#endif
*/
