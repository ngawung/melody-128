#include <string>
#include <yaml-cpp/yaml.h>

#include "utils/Config.hpp"


namespace {
    YAML::Node config = YAML::LoadFile(RESOURCE_PATH"config.yml");
}

std::string Config::FONT_FILE = config["font"]["file"].as<std::string>();

Vector2 Config::FONT_SIZE = {
    config["font"]["width"].as<float>(),
    config["font"]["height"].as<float>()
};

int Config::FONT_ROW = config["font"]["row"].as<int>();

Vector2 Config::CANVAS_SIZE = {
    config["canvas"]["width"].as<float>(),
    config["canvas"]["height"].as<float>()
};

Vector2 Config::WINDOW_SIZE = {80.0f, 35.0f};