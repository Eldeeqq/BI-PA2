//
// Created by eldee on 26.5.19.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <set>
#include <map>
#include <cmath>
#include "Game.h"
#include "GameLoader.h"
#include "assets.h"

Game::Game(const std::string &path, int w, int h) : path(path), width(w), height(h) {
    std::ifstream file(path);
    std::string name, mapPath;

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> name;
        iss >> mapPath;
        maps.emplace_back(name, mapPath);
        maplines++;
    }

    file.close();
//    std::cerr << this->title << std::endl;
//    std::cerr << this->objects.size() << std::endl;
}

GameObject *Game::at(int y, int x) const {
    for (auto ptr : this->objects) {
        if (ptr != nullptr)
            if (ptr->at(y, x))
                return ptr;
    }

    return nullptr;
}

void Game::updateSlot(int y, int x, char c, int color, bool highlight) {
    char cc[1];
    cc[0] = c;

    if (highlight)
        wattron(mainWin, A_REVERSE);
    wattron(mainWin, COLOR_PAIR(color));
    mvwprintw(mainWin, y, x, cc);
    wattroff(mainWin, COLOR_PAIR(color));
    wattroff(mainWin, A_REVERSE);
    //mapa[y][x] = c;
}

void Game::Run() {
    initScreen();
    while (MainMenu()) {};
}

bool Game::MainMenu() {
    int choice = 0;
    int tt_ctr = 0;
    size_t highlight = 0;
    std::vector <std::string> choices = {"Play", "Exit"};
    while (true) {
        wclear(mainWin);
        wclear(sideWin);
        box(mainWin, 0, 0);

        wattron(mainWin, COLOR_PAIR(3));
        int ctr = 0;
        for (auto x: titleASCII) {
            mvwprintw(mainWin, 2 + ++ctr, 40, x.c_str());
        }
        wattroff(mainWin, COLOR_PAIR(3));

        for (size_t i = 1; i < 5; i++) {
            wattron(mainWin, COLOR_PAIR(i + tt_ctr));
            for (size_t j = 0; j < antASCII.size(); j++) {
                mvwprintw(mainWin, 15 + j, 35 + i * 15, antASCII[j].c_str());
            }
            wattroff(mainWin, COLOR_PAIR(i + tt_ctr));

        }


        for (size_t i = 0; i < choices.size(); i++) {
            if (highlight == i)
                wattron(mainWin, A_REVERSE);
            wattron(mainWin, COLOR_PAIR(3));
            mvwprintw(mainWin, 40 + i, 10, " %s ", choices[i].c_str());
            wattroff(mainWin, A_REVERSE);
            wattroff(mainWin, COLOR_PAIR(3));

        }
        choice = wgetch(mainWin);

        if (choice == 66)
            highlight++;

        if (choice == 65)
            highlight += choices.size() - 1;

        highlight %= choices.size();

        if (choice == 10)
            break;
        tt_ctr++;
        tt_ctr %= 7;
        wrefresh(mainWin);
    }
    int level;
    return (highlight == 1 ? false : selectLevel(level));

}

void Game::printMap() {
    wclear(mainWin);
    for (auto x: objects)
        x->render(*this);


    wrefresh(sideWin);
    wrefresh(mainWin);
    refresh();

}

void Game::garbage() {

    for (size_t idx = 0; idx < objects.size(); idx++)
        if (objects[idx]->HP == INT8_MIN) {
            std::iter_swap(objects.begin() + idx, objects.rbegin());
            objects.back()->del(*this);
            delete objects.back();
            objects.pop_back();

        }


}

Game::~Game() {
    for (auto x: objects)
        delete x;
    for (auto x : toAdd)
        delete x;
    delwin(mainWin);
    delwin(sideWin);
    endwin();
}

void Game::Step() {
    while (antCount != 0) {
        for (auto x: objects)
            x->update(*this);
        garbage();
        //for (auto x: objects)
        for (auto x: objects)
            x->render(*this);
        printMap();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }

}

void Game::loadMap(int idx) {
    if (!GameLoader::loadData(maps[idx].second, *this)) {
        std::cerr << "Couldn't load map " << maps[idx].first << std::endl;
    }
}

bool Game::selectLevel(int &level) {
    wclear(mainWin);
    size_t choice = 0;
    size_t highlight = 0;
    while (true) {
        box(mainWin, 0, 0);
        wattron(mainWin, COLOR_PAIR(3));
        for (size_t i = 0; i < levelASCII.size(); i++) {
            mvwprintw(mainWin, 3 + i, 50, levelASCII[i].c_str());
        }
        wattroff(mainWin, COLOR_PAIR(3));

        mvwprintw(mainWin, 25, 25, " %s ", "List of maps:");
        mvwprintw(mainWin, 26, 25, " %s ", "=============");

        for (size_t i = 0; i < maps.size(); i++) {
            if (highlight == i)
                wattron(mainWin, A_REVERSE);
            mvwprintw(mainWin, 27 + i, 27, " %s ", maps[i].first.c_str());
            wattroff(mainWin, A_REVERSE);
        }
        choice = wgetch(mainWin);
//        std::cerr << "CH " << choice << std::endl;
        if (choice == 66)
            highlight++;

        if (choice == 65)
            highlight += maps.size() - 1;

        highlight %= maps.size();

        if (choice == 10)
            break;

        if (choice == 113)
            return true;

        wrefresh(mainWin);

    }


    loadMap(highlight);
    wclear(mainWin);
    wrefresh(mainWin);
//    std::cerr << "Selected: " << maps[highlight].first << std::endl;
    return PlayGame() ? true : saveGame();
}

bool Game::PlayGame() {
    printMap();
    printScore();
    //std::sort(hills.begin(), hills.end(), [](const Anthill *a, const Anthill *b) -> bool { return a->TEAM > b->TEAM; });
    int rounds = 64;
    while (!winner() && rounds > 0) {
        for (auto x:  hills) {
            if (x->HP != 0 && x->TEAM != 0) {
                x->selected = true;
                printMap();
                if (!getChoice(x)) {
                    return false;
                }
            }

        }
        for (auto x: hills)
            x->update(*this);
        for (auto x: toAdd) {
            x->getPath(*this);
            objects.push_back(x);
        }

        toAdd.clear();
        Step();
        rounds--;
    }

    garbage();
    printMap();


    mvwprintw(mainWin, 20, 40, " %s ", "KONEC HRY");
    if (rounds == 64 && !winner()) {
        mvwprintw(mainWin, 21, 40, " %s ", "You ran out of rounds.");
    } else {
        mvwprintw(mainWin, 21, 40, " %s ", "Winner: ");
        wattron(mainWin, COLOR_PAIR(hills[0]->TEAM));
        mvwprintw(mainWin, 21, 49, "%s ", colors[hills[0]->TEAM].c_str());
        wattroff(mainWin, COLOR_PAIR(hills[0]->TEAM));

    }
    wrefresh(mainWin);
    wgetch(mainWin);

    clearMem();
    wclear(stdscr);
    wclear(mainWin);
    wclear(sideWin);
    refresh();

    return true;
}

bool Game::targetHill(Anthill *src) {
    size_t highlight = 0;
    int choice;
    printMap();
    printScore();
    mvwprintw(sideWin, 2, 2, "%s ", "Map:");
    mvwprintw(sideWin, 3, 2, "> %s ", title.c_str());
    mvwprintw(sideWin, 16, 2, " Stage:");
    mvwprintw(sideWin, 17, 2, "  Targeting");
    wrefresh(sideWin);
    while (true) {
        hills[highlight]->selected = false;
        choice = wgetch(mainWin);

        if (choice == 66)
            highlight++;

        if (choice == 65)
            highlight += hills.size() - 1;

        highlight %= hills.size();

        if (choice == 10)
            break;


        hills[highlight]->selected = true;
        printMap();
        printScore();
        mvwprintw(sideWin, 2, 2, "%s ", "Map:");
        mvwprintw(sideWin, 3, 2, "> %s ", title.c_str());
        mvwprintw(sideWin, 16, 2, " Stage:");
        mvwprintw(sideWin, 17, 2, "  Targeting");
        wrefresh(mainWin);
        wrefresh(sideWin);
    }
    src->focus(hills[highlight]);
    return true;

}

void Game::initScreen() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();

    curs_set(0);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    refresh();

    mainWin = newwin(height, width + 50, 1, 1);
    box(mainWin, 0, 0);
    wrefresh(mainWin);
    sideWin = newwin(height, 50, 1, width + 1);
    wrefresh(sideWin);


//    setlocale(LC_ALL, "");
//    initscr();
//    noecho();
//    curs_set(0);


}

bool Game::getChoice(Anthill *hill) {
    size_t highlight = 0;
    int choice;
    std::vector <std::string> choices = {"Send Wave", "Auto", "Pass"};
    while (true) {
        wclear(sideWin);


        printScore();
        mvwprintw(sideWin, 2, 2, "%s ", "Map:");
        mvwprintw(sideWin, 3, 2, "> %s ", title.c_str());
        mvwprintw(sideWin, 5, 2, " Turn: ");
        wattron(sideWin, COLOR_PAIR(hill->TEAM));
        mvwprintw(sideWin, 6, 3, " %s ", colors[hill->TEAM].c_str());
        wattroff(sideWin, COLOR_PAIR(hill->TEAM));

        mvwprintw(sideWin, 9, 2, " %s ", "Possible actions:");
        mvwprintw(sideWin, 10, 2, " ================= ");

        for (size_t i = 0; i < choices.size(); i++) {
            if (highlight == i)
                wattron(sideWin, A_REVERSE);
            mvwprintw(sideWin, 11 + i, 4, " %s ", choices[i].c_str());
            wattroff(sideWin, A_REVERSE);

        }
        mvwprintw(sideWin, 14, 2, " ================= ");
        mvwprintw(sideWin, 16, 2, " Stage:");
        mvwprintw(sideWin, 17, 2, "  Action select");

        wrefresh(mainWin);
        wrefresh(sideWin);
        choice = wgetch(sideWin);

        if (choice == 66)
            highlight++;

        if (choice == 65)
            highlight += choices.size() - 1;

        highlight %= choices.size();

        if (choice == 10)
            break;

        if (choice == 113)
            return false;

        wrefresh(mainWin);
        wrefresh(sideWin);
    }
    hill->selected = false;
    if (highlight == 2)
        return true;

    if (highlight == 1)
        return autoMove(hill);

    else return targetHill(hill);
}

bool Game::autoMove(Anthill *hill) {
    // checks if there is unhabitated anthill
    for (auto x: hills)
        if (x->TEAM == 0 || x->HP == 0) {
            hill->focus(x);
            return true;
        }
    for (auto x: hills) {
        if (x->HP < std::ceil(hill->HP / 2) && x->TEAM != hill->TEAM) {
            hill->focus(x);
            return true;
        }
    }
    for (auto x: hills) {
        if (x->TEAM != hill->TEAM)
            hill->focus(x);
    }
    return true;
}

bool Game::winner() {
    std::set<int> teams;

    for (auto x: hills) {

        teams.emplace(x->TEAM);

    }

    return teams.size() == 1;
}

void Game::printScore() {
    std::map<int, int> score;
    for (auto x: hills) {
        if (x->HP != 0 && x->TEAM != 0) {
            if (score.find(x->TEAM) == score.end())
                score.insert(std::make_pair(x->TEAM, 0));
            score[x->TEAM] += x->HP;
        }
    }

    mvwprintw(sideWin, 24, 4, "# of ants ");
    int ctr = 0;
    wattron(sideWin, A_REVERSE);
    for (auto x: score) {
        wattron(sideWin, COLOR_PAIR(x.first));
        mvwprintw(sideWin, 25 + ctr++, 4, " %6d ", x.second);
        wattroff(sideWin, COLOR_PAIR(x.first));
    }
    wattroff(sideWin, A_REVERSE);

    wrefresh(sideWin);
}

void Game::clearMem() {

    for (auto x: objects)
        delete x;
    for (auto x: toAdd)
        delete x;

    toAdd.clear();
    objects.clear();
    hills.clear();
}

bool Game::saveGame() {
    wclear(mainWin);
    wclear(sideWin);
    wclear(stdscr);
    mvwprintw(stdscr, 20, 10, "Do you want to save the game?");
    size_t highlight = 0;
    int choice = 0;
    std::vector <std::string> choices = {"Yes", "No"};
    while (true) {
        for (size_t i = 0; i < choices.size(); i++) {
            if (i == highlight)
                wattron(stdscr, A_REVERSE);
            mvwprintw(stdscr, 21 + i, 15, choices[i].c_str());
            wattroff(stdscr, A_REVERSE);
        }
        refresh();

        choice = wgetch(sideWin);

        if (choice == 66)
            highlight++;

        if (choice == 65)
            highlight += choices.size() - 1;

        highlight %= choices.size();

        if (choice == 10)
            break;
        refresh();
    }

    if (highlight == 1) {
        clearMem();

        return true;
    }


    std::string save = "Save" + std::to_string(maplines);
    GameLoader::saveGame(save, *this);

    std::ofstream file(path, std::ios::out | std::ios::app);
    std::string line = save + " src/maps/" + save + ".save\n";
    file << line;
    file.close();
    maps.emplace_back(save, "src/maps/" + save + ".save");
//    std::cout<<maps.back().first<<"X"<<maps.back().second<<std::endl;
    clearMem();
    file.close();
    return true;
}
