#include "expr.h"

typedef double (*FuncPtr) (double);
const char* Expr::OPCODE[OPLEN] = {"ln", "log", "sqrt", "asin", "acos", "atan", "sinh", "cosh", "tanh",
									"sin", "cos", "tan", "abs", "ceil", "floor"};
const FuncPtr Expr::OPFUNC[OPLEN] = {log, log10, sqrt, asin, acos, atan, sinh, cosh, tanh,
									sin, cos, tan, fabs, ceil, floor};

Expr::Expr()
{
	type = TYPE_UNKNOWN;
	error = 0;
	left = NULL;
	right = NULL;
}

Expr::Expr(char* expr)
{
	type = TYPE_UNKNOWN;
	error = 0;
	left = NULL;
	right = NULL;
	
	expr = funcRead(expr);
	init(expr);
}

Expr::Expr(const Expr& other)
{
	clear();
	copy(other);
}

const Expr& Expr::operator=(const Expr& other)
{
	if (this != &other)
	{
		clear();
		copy(other);
	}
	return *this;
}

Expr::~Expr()
{
	clear();
}

void Expr::init(char* expr)
{
	int length = strlen(expr);
	int cumRank = 0;
	int minRank = INT_MAX, minCumRank = 0, minIndex = -1;
	for (int i=0;i<length;i++)              //Find operator with minimum order
	{
		int curRank = getRank(expr[i]);
		if (curRank==10 || curRank==-10)
		{
			cumRank += curRank;
			curRank = cumRank;
		}
		else if (curRank != 0)
		{
			curRank = cumRank + curRank;
			if (curRank < minRank)
			{
				minRank = curRank;
				minCumRank = cumRank;
				minIndex = i;
			}
		}
	}
	
	if (minIndex == -1)                   	//No operator
	{
		while (expr[0]=='(' && expr[length-1]==')')
		{
			expr[length-1] = '\0';
			expr++;
			length = strlen(expr);
		}
		if (isNumber(expr))   				//Number
		{
			type = TYPE_NUM;
			value = atof(expr);
		}
		else if (isVarName(expr))			//Variable
		{
			type = TYPE_VAR;
		}
		else
		{
			type = TYPE_UNKNOWN;
			error = ERROR_PARSE;
		}
	}
	else                                    //Found operator
	{
		while (minCumRank>0 && expr[0]=='(' && expr[length-1]==')')
		{
			expr[length-1] = '\0';
			expr++;
			length = strlen(expr);
			minIndex--;
			minCumRank -= 10;
		}
		if (isOperator(expr[minIndex]))
		{
			type = TYPE_OP;
			op = expr[minIndex];
			char leftStr[length], rightStr[length];
			memset(leftStr, 0, length);
			memset(rightStr, 0, length);
			strncpy(leftStr, expr, minIndex);
			strcpy(rightStr, expr+minIndex+1);
			left = new Expr(leftStr);
			right = new Expr(rightStr);
		}
		else
		{
			type = TYPE_UNKNOWN;
			error = ERROR_PARSE;
		}
	}
}

void Expr::copy(const Expr& other)
{
	type = other.type;
	error = other.error;
	value = other.value;
	op = other.op;
	
	if (other.left)
	{
		left->clear();
		left = new Expr();
		left->copy(*(other.left));
	}
	if (other.right)
	{
		right->clear();
		right = new Expr();
		right->copy(*(other.right));
	}
}

void Expr::clear()
{
	type = TYPE_UNKNOWN;
	
	if (left != NULL)
	{
		left->clear();
		delete left;
		left = NULL;
	}
	if (right != NULL)
	{
		right->clear();
		delete right;
		right = NULL;
	}
}

Result Expr::eval()
{
	if (error)
		return Result::Error(error);

	if (type == TYPE_UNKNOWN)
		return Result::Error(ERROR_TYPE);
	if (type == TYPE_NUM)
		return Result(value);
	if (type == TYPE_OP)
	{
		if (op == '=')
			return Result::Error(ERROR_NOTPERM);

		Result lres = left->eval();
		Result rres = right->eval();
		if (lres.error | rres.error)
			return Result::Error(lres.error | rres.error);

		double lvalue = lres.value;
		double rvalue = rres.value;
		if (op == '+')
			return lvalue+rvalue;
		if (op == '-')
			return lvalue-rvalue;
		if (op == '*')
			return lvalue*rvalue;
		if (op == '/')
			return lvalue/rvalue;
		if (op == '^')
			return pow(lvalue, rvalue);
		if (op == '#')
			return funcEval(lvalue, rvalue);
	}
	if (type == TYPE_VAR)
		return Result::Error(ERROR_NOTPERM);
	return Result::Error(ERROR_UNKNOWN);
}

bool Expr::isNumber(char* str)
{
    char *endptr;
    strtof(str, &endptr);
    if (*endptr != '\0')
        return false;
    return true;
}

bool Expr::isOperator(char op)
{
	return op=='+' || op=='-' || op=='*' || op=='/' || op=='^' || op=='#' || op=='=';
}

bool Expr::isVarName(char* str)
{
	if (str==NULL || strlen(str)==0)
		return false;
	if (!isalpha(str[0]))
		return false;
	for (size_t i=1;i<strlen(str);i++)
	{
		if (!isalnum(str[i]))
			return false;
	}
	return true;
}

int Expr::getRank(char op)
{
	if (op == '+' || op == '-')
		return 1;
	if (op == '*' || op == '/')
		return 2;
	if (op == '^')
		return 3;
	if (op == '#')
		return 5;
	if (op == '=')
		return 8;
	if (op == '(')
		return 10;
	if (op == ')')
		return -10;
	else
		return 0;
}

char* Expr::funcRead(char* var)
{
	char buf[strlen(var)];
	strcpy(buf, var);
	for (int i=0;i<OPLEN;i++)
	{
		char* ptr = strstr(buf, OPCODE[i]);
		while (ptr != NULL)
		{
			sprintf(ptr, "%d", i);
			ptr[strlen(ptr)] = '#';
			char* src = ptr+strlen(OPCODE[i]);
			char* dst = strrchr(ptr, '#')+1;
			while (*src)
			{
				*dst = *src;
				src++;
				dst++;
			}
			*dst = '\0';
			ptr = strstr(buf, OPCODE[i]);
		}
	}
	strcpy(var, buf);
	return var;
}

Result Expr::funcEval(int x, double y)
{
	if (x>=0 && x<OPLEN)
		return Result((*OPFUNC[x])(y));
	else
		return Result::Error(ERROR_FUNC);
}
