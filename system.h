#ifndef SYSTEM_H
#define SYSTEM_H

#include "input.h"
#include "output.h"
#include "expr.h"

using namespace std;

class System
{
	private:
		char eventBuf[8];
		int eventLen;
		
		bool disk;
		
		void clearBuf();
		void prompt();
		void exec(const char* command);

	public:
		System();
		void init();
		void event(char code);
};

#endif
