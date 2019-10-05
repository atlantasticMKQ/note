#include<stdio.h>
#define TRUE	1
#define FALSE	0
int and(int a,int b)
{
	return a&&b;
}
int or(int a,int b)
{
	return a||b;
}
int xor(int a,int b)
{
	return a!=b;
}
char *bootToStr(int a)
{
	if(a==TRUE)
		{
			return "TRUE";
		}
	return "FALSE";
}
	
