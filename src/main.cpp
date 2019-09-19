#include "headers.h"

#include "water/render/Window.h"
#include "water/scene/Scene.h"
#include "water/scene/Settings.h"
#include "water/utils/SettingsReader.h"
#include "water/utils/Log.h"

void printHelp();
void readSettings(const char* path, Settings* setts);
void initLogging();

//-------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    initLogging();

    PLOG_INFO << "Ocean waves simulation app started" << std::endl;

    try
    {
        if (argc != 2)
        {
            printHelp();
            return 1;
        }

        const char* fname = argv[1];
        SettingsReader sr(fname);
        Settings settings;
        sr.checkAndReload(&settings);

        Window window;
        window.init(1024, 768, "Water");

        //
        Scene scene;
        scene.init(settings);

        // main loop
        double lastTime = glfwGetTime();
        int nbFrames = 0;

        int checkFrames = 0;

        while(!window.isExiting())
        {
            // check settings file every 10 frames
            if (++checkFrames == 10)
            {
                checkFrames = 0;
                if (sr.checkAndReload(&settings))
                {
                    scene.init(settings);
                }
            }

            double currentTime = glfwGetTime();
            nbFrames++;
            if ( currentTime - lastTime >= 1.0 )
            {
                glfwSetWindowTitle(window.getID(), std::string("Water - " + std::to_string(nbFrames) + " FPS").c_str());

                nbFrames = 0;
                lastTime += 1.0;
            }

            window.processEvents();

            // update
            scene.update(currentTime);

            // render everything
            window.clear();
            scene.setTransforms(window.getTransforms());
            scene.draw();
            window.swapBuffers();
        }
    }
    catch (std::exception& e)
    {
        PLOG_ERROR << "Exception:";
        PLOG_ERROR << e.what();
        PLOG_ERROR << "Quitting" << std::endl;

        return 2;
    }

    PLOG_INFO << "Quitting" << std::endl;
    return 0;
}

//-------------------------------------------------------------------------------------------------
void printHelp()
{
    PLOG_INFO << "Usage: ./water-app <settings.yaml>";
}

//-------------------------------------------------------------------------------------------------
void initLogging()
{
    static plog::RollingFileAppender<plog::CustomFormatter<true>> fileAppender("log.txt");
    static plog::ColorConsoleAppender<plog::CustomFormatter<false>> consoleAppender;
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);
}
