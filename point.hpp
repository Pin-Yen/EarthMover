#ifndef STATUS_ENUM
#define STATUS_ENUM
   enum STATUS{EMPTY, BLACK, WHITE};
#endif

class Point
{
public:

	/* constructor */
	Point(int row, int col, STATUS color);
	Point();
	int row,col;
	STATUS color;
};