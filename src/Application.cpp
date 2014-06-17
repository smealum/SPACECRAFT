#include "Application.h"
#include "utils/dbg.h"
#include "utils/gldbg.h"
#include <cstdlib>
#include "utils/Input.h"
#include "Planet.h"
#include "SolarSystem.h"
#include "render/Sun.h"
#include "MiniWorld.h"
#include "render/Atmosphere.h"
#include "render/Cursor.h"
#include "utils/TextureManager.h"
#include "world/BlockType.h"
#include "utils/glm.h"
#include "noise/CaveGenerator.h"
#include "galaxy/Galaxy.h"
#include "galaxy/GalaxyGenerator.h"

#define WIN_TITLE "SPACECRAFT"

float PlanetFaceDetailsPower = 28.0;

blockTypes::T selectBlockType(blockTypes::dirt);

using namespace std;
using namespace glm;

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
#else
void reloadAllShaders(void * /*clientData*/)
{
    for (auto it(shaderMap.begin()); it != shaderMap.end(); ++it)
    {
        it->second->load();
    }
}
#endif
void mouseWheelCallback(GLFWwindow *, double x, double y)
{
	selectBlockType = (blockTypes::T)(selectBlockType + (int)y);
}

void reloadAllShaders()
{
	for (auto it(shaderMap.begin()); it != shaderMap.end(); ++it)
	{
		it->second->load();
	}
}

Application::Application() : 
	state(appReady),
    fullscreen(false),
	// fullscreen(true),
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
	// glCheckError("Flush Previous Errors");

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

	glCheckError("Context creation Errors");

	// transparency
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCheckError("GL State initialisation");

#ifndef NTWBAR
	bar = TwNewBar("SPACECRAFT");
	//TwDefine((name+" iconified=true").c_str()); // minimizes
	TwWindowSize(width, height);
	TwDefine(" GLOBAL help='SPACECRAFT > Minecraft' ");
	TwAddVarRW(bar, "Planet LOD Details", TW_TYPE_FLOAT, &PlanetFaceDetailsPower, " label='Planet LOD' min=5.0 max=60.0 step=1");
	TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");
	TwAddVarRW(bar, "Wireframe", TW_TYPE_BOOL8, &wireframe, " label='Wireframe mode' help='Toggle wireframe display mode.' ");
	TwAddButton(bar, "Reload shader", &reloadAllShaders, NULL, " label='reload shaders and compile them' ");
	TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS' ");
	TwAddVarRW(bar, "blockType", TW_TYPE_UINT8, (char*)&selectBlockType, "label='type of the selected block' min=0 max=255 step=1");

	// vsync on
	glfwSwapInterval(vsync);

	// Set GLFW event callbacks
	// - Redirect window size changes to the callback function WindowSizeCB
	glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);

	glCheckError("tweak bar");
#endif

	glfwSetScrollCallback(window, &mouseWheelCallback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_NO_ERROR)
	{
		log_err("Cannot initialize GLEW...");
		glfwTerminate();
		std::exit(1);
	}

	glCheckError("GLEWInit Errors");

	blockTypeLoadValues();
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
		window = glfwCreateWindow(width, height, WIN_TITLE, fs?glfwGetPrimaryMonitor():NULL, NULL); // Windowed
		if (!window) {
			log_err("Cannot create window...");
			glfwTerminate();
			std::exit(2);
		}
		glfwSetCursorPos(window, width/2, height/2);
	}
}

CaveGenerator caves;
Cursor* testCursor;
int testTexture;
int testTextureArray;
Galaxy* globalGalaxy;
bool testBool1=false, testBool2=false;

void Application::run()
{
	BlockType::getInstance(); // TODO can be deleted when used
	state = appInLoop;
	camera = new Camera(0.0000001f, 1e5);
	camera->view = glm::lookAt(
			glm::vec3(1.5, 1.5f, 1.5f),
			glm::vec3(0.f),
			glm::vec3(0, 1.f, 0.f)
			);
	camera->setCameraManager(new CameraKeyboardMouse());

	tt=new testShaders;
	testCursor=new Cursor();

	testTexture=TextureManager::getInstance().loadTexture("data/blocksPack.png");
	testTextureArray=TextureManager::getInstance().loadTextureArray("data/blocksPackArray.png",16,16);
	caves.generate();
	
	globalGalaxy = new Galaxy();
	GalaxyGenerate(globalGalaxy);

	float timeA;
	char titleBuff[512];
	while (state != appExiting)
	{
		while(!glfwWindowShouldClose(window))
		{
			timeA = (float)glfwGetTime();
			loop();
			fpsCounter++;
			deltaTime = (float)glfwGetTime() - timeA;
			time += deltaTime;
			if (time > 1.f)
			{
				fps = (float)fpsCounter/time;
				fpsCounter = 0;
				time = 0.f;
				// updating title to have FPS
				sprintf(titleBuff, "%s FPS: %.1f", WIN_TITLE, fps);
				glfwSetWindowTitle(window, titleBuff);
			}
			// test des ereurs openGL non reportées:
			{
				static int i=0;
				if (i++%300==0)
					glCheckError("Unreported Error");
			}
		}
	}

}

int testVal;
float globalTime=0.0f;

void Application::loop()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		state = appExiting;
	}

	Input::update(window);


	globalGalaxy->step(*camera,contentHandler,globalTime);
    
    camera->update();

	//---------------------
	//    drawing
	//--------------------
	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, wireframe?GL_LINE:GL_FILL);

	globalGalaxy->draw(*camera);
	testCursor->draw(*camera);


	if(Input::isKeyPressed(GLFW_KEY_N))reloadAllShaders();

    if(Input::isKeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        if(Input::isKeyHold(GLFW_KEY_P))globalTime+=0.1f;
        if(Input::isKeyHold(GLFW_KEY_M))globalTime-=0.1f;
    }else{
    	if(Input::isKeyHold(GLFW_KEY_P))globalTime+=0.001f;
        if(Input::isKeyHold(GLFW_KEY_M))globalTime-=0.001f;
    }

    if(Input::isKeyPressed(GLFW_KEY_V))testBool1^=1;
	if(Input::isKeyPressed(GLFW_KEY_B))testBool2^=1;

	// printf("test %d\n",testVal);
	testVal=0;

	contentHandler.handleNewContent();

    #ifndef NTWBAR
        // Draw tweak bars (or don't)
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		TwDraw();
    #endif

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
