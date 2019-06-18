//
// Created by eldee on 26.5.19.
//

#include <array>
#include <queue>
#include <list>
#include <iostream>
#include "Path.h"
#include "Game.h"
#include "GameObject.h"
#include <ncurses.h>


Point Path::getNext() {
    return path[++actualIDX];
}

bool Path::inMap(Point p) const {
    return inMap(p.Y, p.X);
}

bool Path::inMap(int y, int x) const {
    return 0 <= y && y < 50 && 0 <= x && x < 120;
}

void Path::calculatePath(GameObject &src, GameObject &trg, Game &game) {

    path.clear();


    while (!q.empty()) q.pop();
    std::vector <std::vector<state >> visited;
    visited.resize(game.height);

    for (auto &x : visited) {
        x.resize(game.width);
        x.assign(x.size(), state::NEW);
    }

    std::list <std::pair<Point, Point>> routes;

    Point fin(trg.Y, trg.X);
//    std::cout<<"Target>"<<fin<<std::endl;

    visited[src.Y][src.X] = state::OPEN;
    q.emplace(src.Y, src.X);

    while (!q.empty()) {
        Point ac = q.front();
        q.pop();
//        std::cerr<<"Start"<<ac<<std::endl;
//        std::cerr<<"Point "<<ac<<std::endl;
        game.updateSlot(ac.Y, ac.X, '.', 1, true);
        GameObject *actual = game.at(ac.Y, ac.X);
        if (actual == &trg) {
            fin = Point(ac.Y, ac.X);
            break;
        }
        // check left point
        Point p = ac.left();
        if (check(p, &src, &trg, game) && visited[p.Y][p.X] == state::NEW) {
            visited[p.Y][p.X] = state::OPEN;
            q.push(p);
            routes.emplace_back(ac, p);
//            std::cerr<<p<<std::endl;
        }

        // check up point
        p = ac.up();
        if (check(p, &src, &trg, game) && visited[p.Y][p.X] == state::NEW) {
            visited[p.Y][p.X] = state::OPEN;
            q.push(p);
            routes.emplace_back(ac, p);

//            std::cerr<<p<<std::endl;
        }

        // check right point
        p = ac.right();
        if (check(p, &src, &trg, game) && visited[p.Y][p.X] == state::NEW) {
            visited[p.Y][p.X] = state::OPEN;
            q.push(p);
            routes.emplace_back(ac, p);
//            std::cerr<<p<<std::endl;

        }

        // check down point
        p = ac.down();
        if (check(p, &src, &trg, game) && visited[p.Y][p.X] == state::NEW) {
            visited[p.Y][p.X] = state::OPEN;
            q.push(p);
            routes.emplace_back(ac, p);

            //   std::cerr<<p<<std::endl;

        }
        visited[ac.Y][ac.X] = state::CLOSED;
//        std::cerr<<std::endl;

//        for(size_t i =0; i<visited.size(); i++){
//            for(size_t j = 0; j<visited[0].size(); j++){
//                if(visited[i][j] == state::OPEN)
//                    std::cout<<"O";
//                if(visited[i][j] == state::CLOSED)
//                    std::cout<<"C";
//                if(visited[i][j] == state::NEW)
//                    std::cout<<" ";
//
//            }
//            std::cout<<std::endl;
        //  }


//        std::this_thread::sleep_for(timespan);

        // std::cout<<std::endl;
    }

    while (!routes.empty()) {
        if (routes.back().second == fin) {
            path.insert(path.begin(), fin);
            fin = routes.back().first;
        }
        routes.pop_back();
    }
}

bool Path::check(Point &p, GameObject *src, GameObject *obj, Game &game) {
    if (!inMap(p))
        return false;

    GameObject *ptr = game.at(p.Y, p.X);

    if (src->canStepOn(ptr))
        return false;

    if (ptr == src)
        return false;

    if (ptr == nullptr)
        return true;
//    return (!ptr->isStatic || ptr == obj)&&() ptr->TEAM !=src->TEAM &;
    if (ptr->isStatic)
        return false;
    if (ptr == obj)
        return true;
    if (ptr->TEAM == src->TEAM)
        return src != ptr;
    // return ( ptr == obj)&&() ptr->TEAM !=src->TEAM &;
    return false;
}

bool Path::hasMoves() {
    return actualIDX < path.size();
}



