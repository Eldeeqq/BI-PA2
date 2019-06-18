//
// Created by eldee on 26.5.19.
//

#include "Anthill.h"
#include "Game.h"
#include "Ant.h"
#include <cmath>
#include <iostream>

void Anthill::update(Game &game) {
    if (target == this) {
        target = nullptr;
        return;
    }
    if (target != nullptr) {
        for (int i = 0; i <= std::ceil((HP / 2)); i++) {
            Ant *a = new Ant(this, target, i, Y + 2, X, TEAM, 1, 1);
            game.toAdd.push_back(a);
            game.antCount++;
            HP--;
        }

        target = nullptr;
    } else {
        if (HP != 0) {
            HP += ctr % HP == 0;
            ++ctr %= HP;
        }
    }


}

void Anthill::render(Game &game) {
    for (int i = -2; i <= 2; i++) {
        game.updateSlot(Y - 1, X - i, 'X', TEAM, selected);
        game.updateSlot(Y + 1, X - i, 'X', TEAM, selected);
    }

    game.updateSlot(Y, X - 2, 'X', TEAM, selected);
    game.updateSlot(Y, X + 2, 'X', TEAM, selected);

    game.updateSlot(Y, X - 1, HP / 100 % 10 + '0', TEAM, selected);
    game.updateSlot(Y, X, HP / 10 % 10 + '0', TEAM, selected);
    game.updateSlot(Y, X + 1, HP % 10 + '0', TEAM, selected);
}

bool Anthill::at(int y, int x) const {
    return abs(y - Y) < 2 && abs(x - X) < 3;
}

bool Anthill::focus(GameObject *newTarget) {
    if (target == this) {
        target = nullptr;
        return false;
    }
    target = newTarget;
    Point p(this->Y, this->X);
    Point q(newTarget->Y, newTarget->X);
//    std::cerr<<p<<" --> "<<q<<std::endl;
    return nullptr != target;
}

bool Anthill::focus(int y, int x, Game &game) {
    target = game.at(y, x);
    return nullptr != target;
}

bool Anthill::canCapture(GameObject &obj) {
    return HP == 0 || obj.TEAM == TEAM;
}

void Anthill::save(std::ofstream &os) {
    os << "A " << Y << " " << X << " " << TEAM << " " << HP << "\n";
}

