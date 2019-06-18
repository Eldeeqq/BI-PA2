//
// Created by eldee on 26.5.19.
//

#include "Wall.h"
#include "Game.h"

void Wall::render(Game &game) {
    game.updateSlot(Y, X, '#', 0, false);
}

void Wall::update(Game &game) {}

bool Wall::at(int y, int x) const {
    return y == Y && x == X;
}

void Wall::save(std::ofstream &os) {
    os << "# " << Y << " " << X << "\n";
}
