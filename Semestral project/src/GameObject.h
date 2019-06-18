//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_GAMEOBJECT_H
#define ANTWARZ_GAMEOBJECT_H

class Game;

#pragma once

#include <fstream>
/**
 * This class is used as a main component of the game,
 * some object also inherits from this one.
 * */

class GameObject {

public:
    ///  Y coordinate
    int Y{};
    /// X coordinate
    int X{};
    /// TEAM identificator
    int TEAM{};
    /// HEALTH
    int HP{};
    /// DAMAGE
    int DMG{};
    /// value which tells us whether this GameObject is static
    bool isStatic = false;

    /**
    * GameObject constructor
    * @param X, Y , team, hp, dmg
    */
    inline GameObject(int y, int x, int team, int hp, int dmg) :
            Y(y), X(x), TEAM(team), HP(hp), DMG(dmg) {};

    virtual ~GameObject() = default;

    /**
    * GameObject constructor for static objects
    * @param X, Y
    */
    inline GameObject(int y, int x) : Y(y), X(x), isStatic(true) {}

    /**
    * Updates state of itself in the game
    * @param Game
    */
    virtual void update(Game &game) = 0;
    /**
    * Renders itself in the game ui
    * @param Game &
    */
    virtual void render(Game &game) = 0;
    /**
    * Tells us if this object is at specific coordinates
    * @param int y, int x
    * \return true - is \ false - is not
    */
    virtual bool at(int y, int x) const = 0;
    /**
    * Tells us if this object can capture referenced object
    * \param GameObject reference
    * \return true - can \ false - can not
    */
    virtual bool canCapture(GameObject &obj) { return false; }
    /**
    * Does some aditional logic before deleting the object
    * \param Game reference
    * \return true\false depending on inheritance
    */
    virtual bool del(Game &game) const { return false; }
    /**
       * Saves file into the stream
       * \param std::ostream reference
       */
    virtual void save(std::ofstream &os) {}
    /**
       * Tells us if object can step on target
       * \param GameObjec pointer
       * \return true - can \ false - can not
       */
    virtual bool canStepOn(GameObject *target) const { return false; }
};


#endif //ANTWARZ_GAMEOBJECT_H
