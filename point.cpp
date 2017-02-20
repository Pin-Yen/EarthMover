#include "point.hpp"

Point::Point(int row, int col, STATUS color){
	this->row = row;
	this->col = col;
	this->color = color;
}
/* do NOT call this constructor directely, 
 * this default constructor is for std::array<Point>*/
Point::Point(){}
