//
// Created by eldee on 29.5.19.
//

#ifndef ANTWARZ_ASSETS_H
#define ANTWARZ_ASSETS_H

#pragma  once

#include <string>
#include <vector>
/**
 * \brief This class is used as an Asset storage.
 */
/// Map selection logo
const std::vector <std::string> levelASCII =
        {" __         ______     __   __   ______     __         ______    ",
         R"(/\ \       /\  ___\   /\ \ / /  /\  ___\   /\ \       /\  ___\   )",
         R"(\ \ \____  \ \  __\   \ \ \'/   \ \  __\   \ \ \____  \ \___  \  )",
         R"( \ \_____\  \ \_____\  \ \__|    \ \_____\  \ \_____\  \/\_____\ )",
         R"(  \/_____/   \/_____/   \/_/      \/_____/   \/_____/   \/_____/ )"};
/// Main menu logo
const std::vector <std::string> titleASCII =
        {" ______     __   __     ______      __     __     ______     ______     ______",
         R"(/\  __ \   /\ "-.\ \   /\__  _\    /\ \  _ \ \   /\  __ \   /\  == \   /\___  \)",
         R"(\ \  __ \  \ \ \-.  \  \/_/\ \/    \ \ \/ ".\ \  \ \  __ \  \ \  __<   \/_/  /__)",
         R"( \ \_\ \_\  \ \_\\"\_\    \ \_\     \ \__/".~\_\  \ \_\ \_\  \ \_\ \_\   /\_____\)",
         R"(  \/_/\/_/   \/_/ \/_/     \/_/      \/_/   \/_/   \/_/\/_/   \/_/ /_/   \/_____/)"};

/// Ant in ascii art

const std::vector <std::string> antASCII =
        {R"(/\/\)",
         R"(  \_\  _..._)",
         R"(  (" )(_..._))",
         R"(   ^^  // \\)"};


#endif //ANTWARZ_ASSETS_H
