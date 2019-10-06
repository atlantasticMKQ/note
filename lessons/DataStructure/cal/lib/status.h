#include<stdio.h>
/*
 *status.h
 */
#define OK      	0     	//exit with no problem
#define ERR     	1	//exit with unknown err
#define OF      	2	//over float
#define UST     	3	//unset
#define BDAG    	4	//bad args type
#define POPEND  	5	//error of char stack popend
#define MATCHERR        	6	//error of brac match
#define NUMERR  	7	//error of str to int
#define NAMEERR 	8	//error of func/elem name 


char *statToStr(int stat)
{
	switch(stat)
		{
		case OK:
			return "OK";
		case ERR:
			return "ERR";
		case OF:
			return "OF";
		case UST:
			return "UST";
		case BDAG:
			return "BDAG";
		case POPEND:
			return "POPEND";
		case MATCHERR:
			return "MATCHERR";
		case NUMERR:
			return "NUMERR";
		case NAMEERR:
			return "NAMEERR";
		default:
			return "UNKNOWN";
		}
}

