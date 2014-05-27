#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "utils/Singleton.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef NTWBAR
    #include <AntTweakBar.h>
#endif
#include "render/camera/Camera.h"
#include "render/camera/manager/CameraManager.h"
#include "render/camera/manager/CameraKeyboard.h"
#include "tests/testShaders.h"
#include "data/ContentHandler.h"

#define NUMPRODUCERS (1)

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
        Camera *camera;
        float bgColor[3];
        testShaders *tt;
        ContentHandler contentHandler;

        friend class Singleton<Application>;

        void glfwWindowHints();

    public:
        ~Application();
        void run();
        void loop();
        void createWindowInFullscreen(bool fs);
        //void resetVSync();
        inline void exit() { state = appExiting; }
        inline float getWindowRatio() const { return static_cast<float>(width)/static_cast<float>(height); }

        inline Camera& getCamera() { return *camera; }
};

#endif
