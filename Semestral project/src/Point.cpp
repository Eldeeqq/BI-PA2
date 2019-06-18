//
// Created by eldee on 26.5.19.
//
#include "Point.h"

Point Point::left() {
    return Point(Y, X - 1);
}

Point Point::right() {
    return Point(Y, X + 1);
}

Point Point::up() {
    return Point(Y - 1, X);
}

Point Point::down() {
    return Point(Y + 1, X);
}

bool operator==(Point &a, Point &b) {
    return a.X == b.X && a.Y == b.Y;
}

Point &Point::operator=(const Point &a) {
    this->X = a.X;
    this->Y = a.Y;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "[" << p.Y << ", " << p.X << "]";
    return os;
}
