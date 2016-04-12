#ifndef SYSTEM_H
#define SYSTEM_H

#include "input.h"
#include "output.h"
#include "expr.h"

class System
{
	private:
		Input& cin;
		Output& cout;
		
		char eventBuf[8];
		int eventLen;
		
		void clearBuf();
	public:
		System(Input& in, Output& out);
		void event(char code);
		void prompt();
		void exec(const char* cmd);
		
};

#endif
