//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_ANT_H
#define ANTWARZ_ANT_H

#pragma once

#include "GameObject.h"
#include "Path.h"
/**
 * The main object of the game,
 * interacts with other and is the only movable object.
 */
class Ant : public GameObject {
    /// parent hill
    GameObject *parent;
    /// target hill
    GameObject *target;
    /// delay of ant
    int delay{};
    /// member which calculates the path and tells Ant where to go
    Path pathFinder;
public:
    /// Constructor
    Ant(GameObject *parent, GameObject *target, int delay, int y, int x, int team, int hp, int dmg);
    /// Destructor
    ~Ant() override = default;

    void getPath(Game &game);

    void update(Game &game) override;

    void render(Game &game) override;

    bool at(int y, int x) const override;

    bool del(Game &game) const override;

    bool canStepOn(GameObject *target) const override;

};


#endif //ANTWARZ_ANT_H
