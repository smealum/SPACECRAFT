#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
  #define GLFW_INCLUDE_ES3
  #include <GLFW/glfw3.h>
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
#endif

#include "render/Camera.h"
#include "render/camera/CameraKeyboardMouse.h"
#include "data/ContentHandler.h"
#include "ui/PlayerUI.h"
#include "utils/Singleton.h"

#ifndef NTWBAR
    #include <AntTweakBar.h>
#endif


#ifndef __EMSCRIPTEN__
#define NUMPRODUCERS (3)
#else
#define NUMPRODUCERS (1)
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
        bool fullscreen, vsync, active, wireframe;
        #ifndef NTWBAR
            TwBar *bar;
        #endif
        unsigned int width, height;
        float viewWidth, viewHeight;
        GLFWwindow* window;
        Camera *camera;
        float bgColor[3];
        ContentHandler contentHandler;
        float frame_time_start;
        float deltaTime, time, fps;
        int fpsCounter;

        PlayerUI *ui;

        friend class Singleton<Application>;

        void glfwWindowHints();

    public:
        ~Application();
        void run();
        void loop();
        void createWindowInFullscreen(bool fs);
        void CreateWebWindow();
        //void resetVSync();
        inline void exit() { state = appExiting; }
        inline float getWindowRatio() const { return static_cast<float>(width)/static_cast<float>(height); }
        inline GLFWwindow* getWindow() { return window; }

        inline Camera& getCamera() { return *camera; }

        // elapsed time in seconds
        inline float getFrameDeltaTime() const { return deltaTime; }
    };

#endif
