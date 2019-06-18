//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_GAMELOADER_H
#define ANTWARZ_GAMELOADER_H

#pragma once

#include "Game.h"
#include "GameObject.h"

class Game;
/**
 * This class is used to load and save Game map.
 */
class GameLoader {
private:
    /// Adds a Wall into the game.
    static void addWall(std::string &line, Game &pGame);
    /// Adds an Anthill into the game.
    static void addAnthill(std::string &line, Game &pGame);
    /// adds a map title name to a Game
    static void addTitle(std::string &line, Game &pGame);

public:
    /// loads data
    static bool loadData(const std::string &path, Game &pGame);
    // saves data
    static bool saveGame(const std::string &name, Game &pGame);
};


#endif //ANTWARZ_GAMELOADER_H
