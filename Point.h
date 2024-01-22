#pragma once
// Координата для ориентирования на шахматной доске.
struct Point {

	short row;
	short col;

	Point(const short row, const short col) : row(row), col(col) {};
	Point(const Point& other) : row(other.row), col(other.col) {};

	void moveTo(const short rowShift, const short colShift)  {
		row += rowShift;
		col += colShift;
	}

	Point getMovedTo(const short rowShift, const short colShift) {
		return Point(row + rowShift, col + colShift);
	}

	Point operator+(const Point& other) const {
		return Point(other.row + row, other.col + col);
	}

	bool operator==(const Point& other) const {
		return row == other.row && col == other.col;
	}
};
