#include <string>
#include <mini/ini.h>

#define GLOBAL_CONFIG
#include "utils/Config.hpp"

void Config::LoadConfig(std::string file) {
    mINI::INIFile in(file);
    mINI::INIStructure ini;
    in.read(ini);

    Config::FONT_FILE = ini.get("font").get("file");

    Config::FONT_SIZE = {
        stof(ini.get("font").get("width")),
        stof(ini.get("font").get("height"))
    };

    Config::FONT_ROW = stof(ini.get("font").get("row"));

    Config::CANVAS_SIZE = {
        stof(ini.get("canvas").get("width")),
        stof(ini.get("canvas").get("height"))
    };

    Config::WINDOW_SIZE = {80.0f, 35.0f};
}