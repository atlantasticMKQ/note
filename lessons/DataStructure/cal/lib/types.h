#define UNMATCH	-2
#define UNSET	-1
#define FUNC	0
#define INT	1
#define BOOL	2
#define STR	3
#define FLOAT	4
char *typeToStr(int type)
{
	switch(type)
		{
		case UNMATCH:
			return "UNMATCH";
		case UNSET:
			return "UNSET";
		case FUNC:
			return "FUNC";
		case INT:
			return "INT";
		case BOOL:
			return "BOOL";
		case STR:
			return "STR";
		case FLOAT:
			return "FLOAT";
		default:
			return "UNKNOWN";
		}
}

int strToType(char *str)
{
	IF_EQ(str,UNSET)
		return UNSET;
	IF_EQ(str,FUNC)
		return FUNC;
	IF_EQ(str,INT)
		return INT;
	IF_EQ(str,BOOL)
		return BOOL;
	IF_EQ(str,STR)
		return STR;
	IF_EQ(str,FLOAT)
		return FLOAT;
	return UNMATCH;
}
