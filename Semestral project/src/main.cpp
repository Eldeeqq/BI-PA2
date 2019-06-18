#include <iostream>
#include "Game.h"

int main() {
    Game g("src/maps/maplist.list", 120, 50);
    g.Run();
    return 0;
}
