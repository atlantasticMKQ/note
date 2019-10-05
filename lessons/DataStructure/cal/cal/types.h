#include<stdio.h>
#define FUNC	0
#define INT	1
#define BOOL	2
#define STR	3
#define POLY	4
#define FLOAT	5
char *typeToStr(int type)
{
	switch(type)
		{
		case FUNC:
			return "FUNC";
		case INT:
			return "INT";
		case BOOL:
			return "BOOL";
		case STR:
			return "STR";
		case POLY:
			return "POLY";
		case FLOAT:
			return "FLOAT";
		default:
			return "UNKNOWN";
		}
}
				
