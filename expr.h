#ifndef EXPR_H
#define EXPR_H

#include "Arduino.h"
#include <climits>

#define TYPE_UNKNOWN 0
#define TYPE_NUM 1
#define TYPE_OP 2
#define TYPE_VAR 3

#define ERROR_UNKNOWN 1         //Unknown Error
#define ERROR_PARSE 2           //Parse Error
#define ERROR_NOTPERM 4         //Operation not permitted
#define ERROR_TYPE 8            //Type mismatch
#define ERROR_FUNC 16           //Error in # function

using namespace std;

struct Result
{
	double value;
	int error;
	
	Result()
	{
		error = 0;
	};
	
	Result(double val)
	{
		value = val;
		error = 0;
	};
	
	static Result Error(int err)
	{
		Result res;
		res.error = err;
		return res;
	};
};

class Expr
{
	typedef double (*FuncPtr) (double);
	
	private:
		static const int OPLEN = 15;
		static const char* OPCODE[OPLEN];
		static const FuncPtr OPFUNC[OPLEN];

		int type;
		int error;
		double value;
		char op;
		Expr* left;
		Expr* right;

		void init(char* expr);
		void copy(const Expr& other);
		void clear();
		
		static bool isNumber(char* str);
		static bool isOperator(char op);
		static bool isVarName(char* str);
		static int getRank(char op);
		static char* funcRead(char* var);
		static Result funcEval(int x, double y);

	public:
		Expr();
		Expr(char* expr);
		Expr(const Expr& other);
		const Expr& operator=(const Expr& other);
		~Expr();

		Result eval();
};

#endif
