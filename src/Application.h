#ifndef __APPLICATION_H__
#define __APPLICATION_H__
/*******************************************************************************
 *  Application                                                                *
 *  Mon May 26 CEST 2014                                                       *
 *  Copyright Eduardo San Martin Morote                                        *
 *  eduardo.san-martin-morote@ensimag.fr                                       *
 *  http://posva.net                                                           *
 ******************************************************************************/

#include "utils/Singleton.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef NTWBAR
#include <AntTweakBar.h>
#endif

enum appState {
    appReady,
    appInLoop,
    appPaused,
    appExiting
};

class Application : public Singleton<Application> {
    private:
        Application();
        appState state;
        bool fullscreen, vsync, active;
#ifndef NTWBAR
        TwBar *bar;
#endif
        unsigned int width, height;
        float viewWidth, viewHeight;
        GLFWwindow* window;

        friend class Singleton<Application>;

        void glfwWindowHints();

    public:
        ~Application();
        void run();
        void loop();
        void createWindowInFullscreen(bool fs);
        //void resetVSync();
        inline void exit() { state = appExiting; }

};

#endif
