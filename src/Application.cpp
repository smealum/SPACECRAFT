#include "Application.h"
#include "utils/dbg.h"
#include <cstdlib>
#include "utils/Input.h"
#include "Planet.h"
#include "MiniWorld.h"

#ifndef NTWBAR
inline void TwEventMouseButtonGLFW3(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{TwEventMouseButtonGLFW(button, action);}
inline void TwEventMousePosGLFW3(GLFWwindow* /*window*/, double xpos, double ypos)
{TwMouseMotion(int(xpos), int(ypos));}
inline void TwEventMouseWheelGLFW3(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{TwEventMouseWheelGLFW(yoffset);}
inline void TwEventKeyGLFW3(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{TwEventKeyGLFW(key, action);}
inline void TwEventCharGLFW3(GLFWwindow* /*window*/, int codepoint)
{TwEventCharGLFW(codepoint, GLFW_PRESS);}
inline void TwWindowSizeGLFW3(GLFWwindow* /*window*/, int width, int height)
{TwWindowSize(width, height);}

void TW_CALL reloadAllShaders(void * /*clientData*/)
{
    for (auto it(shaderMap.begin()); it != shaderMap.end(); ++it)
    {
        it->second->load();
    }
}
#endif

Application::Application() : 
    state(appReady),
    fullscreen(false),
    vsync(false),
    active(true),
    wireframe(false),
#ifndef NTWBAR
    bar(NULL),
#endif
    width(800),
    height(600),
    viewWidth(width),
    viewHeight(height),
    window(NULL),
    camera(NULL),
    bgColor{0.0f, 0.0f, 0.0f},
    contentHandler(NUMPRODUCERS),
    deltaTime(0.f),
    time(0.f),
    fps(0.f),
    fpsCounter(0)
{
    if (!glfwInit())
    {
        log_err("Cannot initialize glfw3...");
        std::exit(1);
    }

    glfwWindowHints();

    createWindowInFullscreen(fullscreen);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // can be GLFW_CURSOR_HIDDEN

    #ifndef NTWBAR
        TwInit(TW_OPENGL_CORE, NULL);
    #endif

    // transparency
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    #ifndef NTWBAR
        bar = TwNewBar("SPACECRAFT");
        //TwDefine((name+" iconified=true").c_str()); // minimizes
        TwWindowSize(width, height);
        TwDefine(" GLOBAL help='SPACECRAFT > Minecraft' ");
        TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");
        TwAddVarRW(bar, "Wireframe", TW_TYPE_BOOL8, &wireframe, " label='Wireframe mode' help='Toggle wireframe display mode.' ");
        TwAddButton(bar, "Reload shader", &reloadAllShaders, NULL, " label='reload shaders and compile them' ");
        TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS' ");

        // vsync on
        glfwSwapInterval(1);

        // Set GLFW event callbacks
        // - Redirect window size changes to the callback function WindowSizeCB
        glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

        glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
        glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
        glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
        glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
        glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
    #endif

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_NO_ERROR)
    {
        log_err("Cannot initialize GLEW...");
        glfwTerminate();
        std::exit(1);
    }
}

void Application::glfwWindowHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

}

void Application::createWindowInFullscreen(bool fs)
{
    if (window)
    {
        log_err("createWindowInFullscreen called but windows is alreayd created.");
    } else {
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = mode->width;
        height = mode->height;
        viewWidth = width;
        viewHeight = height;
        if (!fs)
            width *= 2.f/3.f, height *= 2.f/3.f;
        window = glfwCreateWindow(width, height, "SPACECRAFT", fs?glfwGetPrimaryMonitor():NULL, NULL); // Windowed
        if (!window) {
            log_err("Cannot create window...");
            glfwTerminate();
            std::exit(2);
        }
	glfwSetCursorPos(window, width/2, height/2);
    }
}

Planet* testPlanet;
MiniWorld* testMiniWorld;
PlanetFaceBufferHandler* testBuffer;

void Application::run()
{
    state = appInLoop;
    camera = new Camera(0.000001f, 100.f);
    camera->view = glm::lookAt(
            glm::vec3(1.5, 1.5f, 1.5f),
            glm::vec3(0.f),
            glm::vec3(0, 1.f, 0.f)
            );
    camera->setCameraManager(new CameraKeyboardMouse());

    tt = new testShaders;
    testPlanet=new Planet((planetInfo_s){0}, contentHandler);
    // testChunk=new Chunk(testPlanet);
    // testMiniWorld=new MiniWorld(testPlanet, testPlanet->faces[2]);
    // testBuffer=new PlanetFaceBufferHandler(*testPlanet->faces[0], 1024);
    // testBuffer->addFace(testPlanet->faces[0]);
    // testPlanet->testFullGeneration(4, testBuffer);

    float timeA;
    while (state != appExiting)
    {
        while(!glfwWindowShouldClose(window))
        {
            timeA = (float)glfwGetTime();
            loop();
            fpsCounter++;
            deltaTime = (float)glfwGetTime() - timeA;
            time += deltaTime;
            if (time > 0.f)
            {
                fps = (float)fpsCounter/time;
                fpsCounter = 0;
                time = 0.f;
            }
        }
    }

}

void Application::loop()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
        state = appExiting;
    }

    Input::update(window);
    camera->update();

    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->updateFrustum();
    testPlanet->processLevelOfDetail(*camera);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe?GL_LINE:GL_FILL);
    // tt->draw();
    // testPlanet->drawDirect();
    testPlanet->draw(*camera);
    // testChunk->draw(*camera);
    // testMiniWorld->draw(*camera);
    // testBuffer->draw(*camera);

    contentHandler.handleNewContent();

    // #ifndef NTWBAR
    //      // Draw tweak bars
    //     glUseProgram(0);
    //     glBindVertexArray(0);
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    //     TwDraw();
    // #endif

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Application::~Application()
{
    #ifndef NTWBAR
        TwTerminate();
    #endif
    glfwTerminate();
}

