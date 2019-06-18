//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_POINT_H
#define ANTWARZ_POINT_H

#pragma once

#include <ostream>
/**
 * This class is used as a container for coordinates
 * It also has operator== and operator<< for better
 * debugging.
 */
class Point {
public:
    /// Y coord
    int Y;
    /// X coord
    int X;

    /**
     * Method returning point which is relative left to caller
     * @return Point
     */
    Point left();
    /**
     * Method returning point which is relative right to caller
     * @return Point
     */
    Point right();
    /**
     * Method returning point which is relative up to caller
     * @return
     */
    Point up();
    /**
     * Method returning point which is relative down to caller
     * @return
     */
    Point down();
    /**
     * Constructor
     * @param y coord
     * @param x coord
     */
    Point(int y, int x) : Y(y), X(x) {}
    /**
     * Copy constructor
     * @param p - Point
     * @return Point reference
     */
    Point &operator=(const Point &p);
    /**
     * Point equal function
     * @param a
     * @param b
     * @return true - are same\ false - are not
     */
    friend bool operator==(Point &a, Point &b);
    /**
     * Sends object to stream
     * @param os - ostream reference
     * @param p - point
     * @return std::ostream &
     */
    friend std::ostream &operator<<(std::ostream &os, const Point &p);
};


#endif //ANTWARZ_POINT_H
