#ifndef CURSOR_H
#define CURSOR_H

class Cursor
{
	private:
		int width;
		int height;

	public:
		int x;
		int y;

		Cursor();
		Cursor(int width, int height);
		void clear();
		Cursor& operator+=(int offset);         //Move horizontally, change y if warp needed
};

#endif
