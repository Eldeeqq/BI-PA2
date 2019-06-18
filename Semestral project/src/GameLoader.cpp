//
// Created by eldee on 26.5.19.
//

#include <fstream>
#include <sstream>
#include <sstream>
#include <iostream>
#include "GameLoader.h"
#include "Game.h"
#include "Anthill.h"
#include "Wall.h"

bool GameLoader::loadData(const std::string &path, Game &pGame) {
    std::ifstream file(path);
    std::string line;

    if (!file.good())
        return false;

    while (std::getline(file, line)) {
        switch (line[0]) {
            case '#':
                addWall(line, pGame);
                break;
            case 'A':
                addAnthill(line, pGame);
                break;
            case 'T':
                addTitle(line, pGame);
                break;
            default:
                break;
        }
    }
    file.close();
    return true;
}

void GameLoader::addWall(std::string &line, Game &pGame) {
    int y, x;
    std::istringstream iss(line.substr(line.find(' ') + 1));
    iss >> y;
    iss >> x;
    pGame.objects.push_back(new Wall(y, x));
}

void GameLoader::addAnthill(std::string &line, Game &pGame) {
    int y, x, team, ants;
    std::istringstream iss(line.substr(line.find(' ') + 1));
    iss >> y;
    iss >> x;
    iss >> team;
    iss >> ants;
    //std::cerr<<"Y "<<y<<" X "<<x<<std::endl;
    Anthill *a = new Anthill(y, x, team, ants);
    // if(0<y && y+1<pGame.height && 0<x && x+2<pGame.width)
    pGame.objects.push_back(a);
    pGame.hills.push_back(a);
}

void GameLoader::addTitle(std::string &line, Game &pGame) {
    pGame.title = line.substr(line.find(' ') + 1);
}

bool GameLoader::saveGame(const std::string &name, Game &pGame) {
    std::ofstream file("src/maps/" + name + ".save");
    for (auto x: pGame.objects)
        x->save(file);
    file << name;
    file.close();
    return true;
}
