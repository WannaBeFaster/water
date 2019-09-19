#pragma once

#include <string>
#include "../scene/Settings.h"

class SettingsReader
{
public:
    SettingsReader(const std::string& path);
    ~SettingsReader();

    bool checkAndReload(Settings* out);

private:
    std::string path_;
    timespec mtim_;

    bool check();
    glm::vec3 readColor(const YAML::Node& config, const std::string& name);
};

namespace YAML
{

template<>
struct convert<glm::vec2>
{
    static Node encode(const glm::vec2& rhs)
    {
        Node node;
        node["x"] = rhs.x;
        node["y"] = rhs.y;
        return node;
    }

    static bool decode(const Node& node, glm::vec2& rhs)
    {
        if(!node.IsMap() || node.size() != 2)
        {
            return false;
        }

        rhs.x = node["x"].as<float>();
        rhs.y = node["y"].as<float>();
        return true;
    }
};

template<>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node["x"] = rhs.x;
        node["y"] = rhs.y;
        node["z"] = rhs.z;
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if(!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        rhs.x = node["x"].as<float>();
        rhs.y = node["y"].as<float>();
        rhs.z = node["z"].as<float>();
        return true;
    }
};

}
