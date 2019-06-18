//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_WALL_H
#define ANTWARZ_WALL_H

#pragma once

#include "GameObject.h"
/**
 * Imovable static GameObject [WALL]
 */
class Wall : public GameObject {
public:
    /**
     * Constructor
     * @param int y
     * @param int x
     */
    Wall(int y, int x) : GameObject(y, x) {}

    /**
     * Destructor
     */
    ~Wall() override = default;

    void update(Game &game) override;

    void render(Game &game) override;

    bool at(int y, int x) const override;

    void save(std::ofstream &os) override;


};


#endif //ANTWARZ_WALL_H
