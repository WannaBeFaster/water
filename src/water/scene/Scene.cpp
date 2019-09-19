#include "../../headers.h"

#include "../render/CubeMap.h"
#include "Settings.h"

#include "Scene.h"

//-------------------------------------------------------------------------------------------------
void Scene::init(const Settings& settings)
{
    cubemap_.load("data/tx/" + settings.SkyboxName);

    sky_.init(settings, &cubemap_);

    ocean_.init(settings);
    ocean_.setCubemap(&cubemap_);

    settings_ = settings;
}

//-------------------------------------------------------------------------------------------------
void Scene::update(double time)
{
    ocean_.update(time);
}

//-------------------------------------------------------------------------------------------------
void Scene::setTransforms(const Transforms& transforms)
{
    sky_.setTransforms(transforms);
    ocean_.setTransforms(transforms);
}

//-------------------------------------------------------------------------------------------------
void Scene::draw()
{
    if (settings_.RenderSky)
        sky_.draw();

    ocean_.draw();
}
