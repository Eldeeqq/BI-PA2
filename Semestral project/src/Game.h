//
// Created by eldee on 26.5.19.
//

#ifndef ANTWARZ_GAME_H
#define ANTWARZ_GAME_H

#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <ncurses.h>

#include "Anthill.h"
#include "Ant.h"


class GameObject;

class GameLoader;
/**
 * This class represents the game as itself.
 * It is a container for other objects and is
 * responsible for rendering and interaction.
 */
class Game {
    /// Fs path to file with maps
    std::string path;
    /// Width of map
    int width{};
    /// Height of map
    int height{};
    /// amount of maps
    int maplines = 1;
    /// Ncurses window used to render map
    WINDOW *mainWin{};
    /// Ncurses window used to render controlls
    WINDOW *sideWin{};
    /// Map of colors : teams
    std::map<int, std::string> colors = {{1, "GREEN"},
                                         {2, "RED"},
                                         {3, "YELLOW"},
                                         {4, "BLUE"},
                                         {5, "CYAN"},
                                         {6, "MAGENTA"}};
    /// Alive ant counter
    int antCount = 0;
    /// Map title
    std::string title;
    /// Vector of GameObject *
    std::vector<GameObject *> objects;
    /// New GameObjects to be added next step
    std::vector<Ant *> toAdd;
    /// Vector of Anthills
    std::vector<Anthill *> hills;
    /// Map of Maps [bit lame naming, i know]
    std::vector <std::pair<std::string, std::string>> maps;
    /// used to laod/save maps
    friend class GameLoader;
    /// uses Anthills
    friend class Anthill;
    /// uses Ant
    friend class Ant;
    /// needs access to Path
    friend class Path;
    /// Selects a Map which is to be played
    bool selectLevel(int &level);
    /// Renders main menu and handles controll
    bool MainMenu();
    /// Renders the game using Ncurses and handles controlll
    bool PlayGame();
    /// Inits Ncurses
    void initScreen();
    /// Method with which player targets other Anthills
    bool targetHill(Anthill *src);
    /// Gets action from user
    bool getChoice(Anthill *hill);
    /// checks whether one player captured all Anthills
    bool winner();


public:
    /// Loads map using GameLoader
    void loadMap(int idx);
    /// Prints map into terminal
    void printMap();
    /// Deletes dead GameObjects
    void garbage();
    /// Starts the Game
    void Run();
    /// Updates all GameObjects
    void Step();
    /**
     * Game constructor
     * @param path - path to list of maps
     * @param w - width
     * @param h - height
     */
    explicit Game(const std::string &path, int w, int h);
    /// Implicit destructor
    ~Game();
    /**
     * Method which GameObjects use to retrieve GameObject o
     * specific coordinates
     * \return GameObject * which is on coords or nullptr if there is no such object
     */
    GameObject *at(int y, int x) const;
    /// Updates specific slot in game (Used for rendering)
    void updateSlot(int y, int x, char c, int color, bool highlight);

    /**
     * Function whih decides the move instead of the player
     * It prefers Empty Anthills > Low HP enemies > First enemy encountered
     * @param hill - Anthill that is to move
     * @return true
     */
    bool autoMove(Anthill *hill);
    /// Method which prints score
    void printScore();
    /// Clears Memory
    void clearMem();
    // Saves map
    bool saveGame();
};


#endif //ANTWARZ_GAME_H
