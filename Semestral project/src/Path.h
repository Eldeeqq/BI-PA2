//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_PATH_H
#define ANTWARZ_PATH_H
#pragma once

#include <vector>
#include <queue>
#include "Point.h"

class GameObject;

class Game;

/**
 * This class is used to calculate path using BFS algorithm.
 * */

class Path {
    /// States of visitation of a point
    enum state {
        NEW, OPEN, CLOSED
    };

    /// queue for BFS

    std::queue <Point> q;

    /// check if coordinates are in range of a map

    bool inMap(int y, int x) const;

    /// check if point are in range of a map

    bool inMap(Point p) const;

    /// tells us on what step of path Ant is
    size_t actualIDX = 0;

    /// check if unit can move on target slot
    bool check(Point &p, GameObject *src, GameObject *obj, Game &game);


public:
    /// actual path represented in vector of points
    std::vector <Point> path;

    /// getter for next step
    Point getNext();

    /// check if there are steps left
    bool hasMoves();

    /// path calculator
    void calculatePath(GameObject &src, GameObject &trg, Game &game);
};


#endif //ANTWARZ_PATH_H
