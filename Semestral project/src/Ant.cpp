//
// Created by eldee on 26.5.19.
//

#include <iostream>
#include "Ant.h"
#include "Game.h"

void Ant::update(Game &game) {
    if (delay > 0) {
//        std::cerr << "Delay:" << delay << std::endl;
        delay--;
        return;
    }

    if (HP < 0) {
        HP = INT8_MIN;
        return;
    }


    if (pathFinder.hasMoves()) {
        auto point = pathFinder.getNext();
        GameObject *tmp = game.at(point.Y, point.X);
        if (tmp == nullptr) {
            Y = point.Y;
            X = point.X;
//            std::cerr << "Move" << std::endl;

            return;
        }
        if (tmp->TEAM == this->TEAM) {
            if (tmp->canCapture(*this)) {
                tmp->HP += this->HP;
                HP = INT8_MIN;
//                std::cerr << "Aid ours" << std::endl;
                return;
            }
            Y = point.Y;
            X = point.X;
            return;

        } else {
            // can conquer enemy
            if (tmp->canCapture(*this)) {
                tmp->TEAM = TEAM;
                tmp->HP = this->HP;
                HP = INT8_MIN;
//                std::cerr << "Conquer enemy" << std::endl;
                return;
            }
            // its ant
            tmp->HP -= DMG;
            if (tmp->HP == 0 && tmp->DMG != 0) {
                tmp->HP = INT8_MIN;
                Y = point.Y;
                X = point.X;
//                std::cerr << "killed target" << std::endl;
                return;
            } else {
                HP = INT8_MIN;
//                std::cerr << "killed by enemy" << std::endl;
                return;
            }

        }
    }


}

void Ant::render(Game &game) {

    game.updateSlot(Y, X, DMG > 1 ? 'F' : 'o', TEAM, false);
}

bool Ant::at(int y, int x) const {
    return Y == y && x == X;
}

void Ant::getPath(Game &game) {
    if (target == nullptr)
        return;
    pathFinder.calculatePath(*this, *target, game);
//    for(auto & x: pathFinder.path)
//        std::cerr<<x;
//    std::cerr<<std::endl;
}

Ant::Ant(GameObject *parent, GameObject *target, int delay, int y, int x, int team, int hp, int dmg)
        : GameObject(y, x, team, hp, dmg), parent(parent), target(target), delay(delay) {}

bool Ant::del(Game &game) const {
    game.antCount--;
    return true;
}

bool Ant::canStepOn(GameObject *target) const {
    return target == parent;
}

