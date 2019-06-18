//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_ANTHILL_H
#define ANTWARZ_ANTHILL_H

#pragma once

#include "GameObject.h"
/**
 * Another main component of the game.
 * It generates ants and based of instances of
 */
class Anthill : public GameObject {
    /// Amount of ants to be sent
    int toSend;
    /// Targeted Anthill
    GameObject *target = nullptr;
    /// help counter
    int ctr = 0;
public:
    /// defines whether Anthill should be highlighted
    bool selected = false;
    /// Constructor
    Anthill(int y, int x, int team, int ants) : GameObject(y, x, team, ants, 0) {

    }
    /// Destructor
    ~Anthill() override = default;

    void update(Game &game) override;

    void render(Game &game) override;

    bool at(int y, int x) const override;
    /// targets a specific GameObject
    bool focus(GameObject *newTarget);
    /// targets a GameObject at coords
    bool focus(int y, int x, Game &game);

    bool canCapture(GameObject &obj) override;
    void save(std::ofstream &os) override;

};


#endif //ANTWARZ_ANTHILL_H
