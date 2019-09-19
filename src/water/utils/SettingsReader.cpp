#include "../../headers.h"

#include "Log.h"

#include "SettingsReader.h"

//-------------------------------------------------------------------------------------------------
SettingsReader::SettingsReader(const std::string& path)
{ 
    path_ = path;

    mtim_.tv_nsec = 0;
    mtim_.tv_sec = 0;
}

//-------------------------------------------------------------------------------------------------
SettingsReader::~SettingsReader()
{
}

//-------------------------------------------------------------------------------------------------
bool SettingsReader::checkAndReload(Settings* out)
{
    if (!check())
        return false;

    PLOG_INFO << "Reading settings: " << StreamColors(termcolor::magenta, true) << path_;

    // file has been changed (or never read), so read it.
    YAML::Node config = YAML::LoadFile(path_);

    out->GridResolution = config["GridResolution"].as<int>();
    out->GridWorldSize = config["GridWorldSize"].as<float>();
    out->GridOrigin = config["GridOrigin"].as<glm::vec2>();
    out->Size = config["Size"].as<float>();

    out->UsePhillips = config["UsePhillips"].as<bool>();

    out->A = config["A"].as<float>();
    out->WindSpeed = config["WindSpeed"].as<float>();
    out->WindAlignment = config["WindAlignment"].as<float>();
    out->MinWaveLength = config["MinWaveLength"].as<float>();
    out->WindDirection = glm::normalize(config["WindDirection"].as<glm::vec2>());
    out->Choppiness = config["Choppiness"].as<float>();
    out->DisplacementFactor = config["DisplacementFactor"].as<float>();

    out->RenderSky = config["RenderSky"].as<bool>();
    out->SkyboxName = config["SkyboxName"].as<std::string>();
    out->SkyboxHalfSize = config["SkyboxHalfSize"].as<float>();
    out->SmoothNormals = config["SmoothNormals"].as<float>();

    out->OceanColor = readColor(config, "OceanColor");
    out->LightColor = readColor(config, "LightColor");
    out->SkyColor = readColor(config, "SkyColor");

    out->LightDir = glm::normalize(config["LightDir"].as<glm::vec3>());

    out->Kr = config["Kr"].as<float>();
    out->Kt = config["Kt"].as<float>();
    out->Ks = config["Ks"].as<float>();

    out->PhongFactor = config["PhongFactor"].as<float>();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool SettingsReader::check()
{
    int fd = open(path_.c_str(), O_RDONLY);
    if (fd < -1)
        return false;

    struct stat fileStat;
    if (fstat(fd, &fileStat) < 0)
        return false;

    close(fd);

    if (fileStat.st_mtim.tv_nsec != mtim_.tv_nsec)
    {
        mtim_.tv_nsec = fileStat.st_mtim.tv_nsec;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
glm::vec3 SettingsReader::readColor(const YAML::Node& config, const std::string& name)
{
    glm::vec3 color = config[name].as<glm::vec3>();
    return color / 255.0f;
}
