#include <iostream>
#include "Application.h"


#include <cstdlib>
#include <functional>
#include <iostream>

#include "MiniWorld.h"
#include "Planet.h"
#include "galaxy/Galaxy.h"
#include "galaxy/GalaxyGenerator.h"
#include "noise/CaveGenerator.h"
#include "render/Atmosphere.h"
#include "render/Cursor.h"
#include "render/Sun.h"
#include "render/TileTexture.h"
#include "utils/Input.h"
#include "utils/dbg.h"
#include "utils/gldbg.h"
#include "utils/glm.h"
#include "world/BlockType.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h> 
#include <cmath>
#endif


const std::string WIN_TITLE = "SPACECRAFT";

float PlanetFaceDetailsPower = 40.0;

uint8_t selectBlockType(blockTypes::dirt);

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

#ifdef __EMSCRIPTEN__

std::map<int, std::string> keys = {
    // Rotation
    {GLFW_KEY_Q, "KEY_Q"},
    {GLFW_KEY_W, "KEY_W"},
    {GLFW_KEY_E, "KEY_E"},
    {GLFW_KEY_A, "KEY_A"},
    {GLFW_KEY_S, "KEY_S"},
    {GLFW_KEY_D, "KEY_D"},
    // Translation
    {GLFW_KEY_U, "KEY_U"},
    {GLFW_KEY_I, "KEY_I"},
    {GLFW_KEY_O, "KEY_O"},
    {GLFW_KEY_J, "KEY_J"},
    {GLFW_KEY_K, "KEY_K"},
    {GLFW_KEY_L, "KEY_L"},
    // Speed
    {GLFW_KEY_Y, "KEY_Y"},
    {GLFW_KEY_H, "KEY_H"},
    // Mode
    {GLFW_KEY_R, "KEY_R"},
};

void UpdateKeyboardInterface() {
  static std::string previous_command = "";
  std::string command;
  for (const auto& key : keys) {
    if (Input::isKeyHold(key.first)) {
      command += "document.getElementById('" + key.second +
                 "').style.backgroundColor = 'gray';";
    } else {
      command += "document.getElementById('" + key.second +
                 "').style.backgroundColor = '#AAD';";
    }
  }
  if (command != previous_command) {
    emscripten_run_script(command.c_str());
    previous_command.swap(command);
  }
}

void UpdateTaskLoad(size_t tasks) {
  static std::string previous_command = "";
  static int i = 0;
  if (i++ % 10 == 0) {
    std::string command =
        "var task_counter = document.getElementById('task_counter');"
        "task_counter.innerHTML = " +
        std::to_string(tasks) +
        ";"
        "task_counter.style.width='" +
        std::to_string(std::powf(tasks / 20000.f, 0.8) * 100.f) + "%';";
    if (command != previous_command) {
      emscripten_run_script(command.c_str());
      previous_command.swap(command);
    }
  }
}

#else

void UpdateKeyboardInterface() {
  // Nothing
}

#endif

void mouseWheelCallback(GLFWwindow *, double x, double y)
{
	selectBlockType = (selectBlockType + (int)y);
}

void reloadAllShaders()
{
  for (auto it(shaderMap.begin()); it != shaderMap.end(); ++it)
    it->second->load();
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
	if (!glfwInit())
	{
		log_err("Cannot initialize glfw3...");
		std::exit(1);
	}

	glfwWindowHints();

  #ifdef __EMSCRIPTEN__
    CreateWebWindow();
  #else
    createWindowInFullscreen(fullscreen);
  #endif

	glfwMakeContextCurrent(window);
  printf("OpenGL version = %s\n", glGetString(GL_VERSION));

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
		TwWindowSize(width, height);
		TwDefine(" GLOBAL help='SPACECRAFT > Minecraft' ");
		TwAddVarRW(bar, "Planet LOD Details", TW_TYPE_FLOAT, &PlanetFaceDetailsPower, " label='Planet LOD' min=5.0 max=60.0 step=1");
		TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");
		TwAddVarRW(bar, "Wireframe", TW_TYPE_BOOL8, &wireframe, " label='Wireframe mode' help='Toggle wireframe display mode.' ");
		TwAddButton(bar, "Reload shader", &reloadAllShaders, NULL, " label='reload shaders and compile them' ");
		TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS' ");
		TwAddVarRW(bar, "blockType", TW_TYPE_UINT8, &selectBlockType, "label='type of the selected block' min=0 max=255 step=1");

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

  #ifndef __EMSCRIPTEN__
  	glewExperimental = GL_TRUE;
  	if (glewInit() != GLEW_NO_ERROR)
  	{
  		log_err("Cannot initialize GLEW...");
  		glfwTerminate();
  		std::exit(1);
  	}
  	glCheckError("GLEWInit Errors");
  #endif

	blockTypeLoadValues();
}

void Application::glfwWindowHints()
{
#ifdef __EMSCRIPTEN__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif
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
		window = glfwCreateWindow(
        width,
        height,
        WIN_TITLE.c_str(),
        fs?glfwGetPrimaryMonitor():NULL, NULL); // Windowed
		if (!window) {
			log_err("Cannot create window...");
			glfwTerminate();
			std::exit(2);
		}
		glfwSetCursorPos(window, width/2, height/2);
	}
}

void Application::CreateWebWindow() {
	if (window) {
		log_err("CreateWebWindow called but windows is already created.");
    return;
	}
	
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  width = 1024;
  height = 640;
  viewWidth = width;
  viewHeight = height;

  window = glfwCreateWindow(
             width,
             height, 
             WIN_TITLE.c_str(), 
             nullptr,
             nullptr); // Windowed

  (void)glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* window, int width, int height) {
        std::cerr << "width = " << width << " height = " << height << std::endl;
      });

  if (!window) {
    log_err("Cannot create window...");
    glfwTerminate();
    std::exit(2);
  }
  //glfwSetCursorPos(window, width/2, height/2);
}

CaveGenerator caves;
Cursor* testCursor;
Galaxy* globalGalaxy;
bool testBool1=false, testBool2=false;


//Application* application_tha

std::function<void()> registered_loop;
void loop_iteration() {
	registered_loop();
}

void Application::run()
{
	//BlockType::getInstance(); // TODO can be deleted when used
	state = appInLoop;
	camera = new Camera(0.0000001f, 1e5);
	camera->view = glm::lookAt(
			glm::vec3(1.5, 1.5f, 1.5f),
			glm::vec3(0.f),
			glm::vec3(0, 1.f, 0.f)
			);
	camera->setCameraManager(new CameraKeyboardMouse);

  testCursor=new Cursor;

	caves.generate();

	globalGalaxy = new Galaxy;
	GalaxyGenerate(globalGalaxy);

	TileTexture::getInstance().init();

	ui = new PlayerUI;
	ui->generateVBO();
	ui->setWhRatio((float)width/height);
	ui->update();

	frame_time_start = (float)glfwGetTime();

	//int limit = 900;
	registered_loop =  [&]() {

		loop();
		fpsCounter++;

    float frame_time_end = (float)glfwGetTime();
		deltaTime = frame_time_end - frame_time_start;
		time += deltaTime;

		if (time > 1.f) {
			fps = (float)fpsCounter/time;
			fpsCounter = 0;
			time = 0.f;
			glfwSetWindowTitle(
          window,
          (" fps = " + std::to_string(fps) + WIN_TITLE).c_str());
		}
		// test des ereurs openGL non reportées:
		{
			static int i=0;
			if (i++%300==0)
				glCheckError("Unreported Error");
		}
    frame_time_start = frame_time_end;
	};

#ifdef __EMSCRIPTEN__
  //emscripten_set_main_loop(loop_iteration, 60, 0);
  emscripten_set_main_loop(loop_iteration, 0, 1);
  glfwTerminate();

#else
	while (state != appExiting) {
		while(!glfwWindowShouldClose(window)) {
			loop_iteration();
		}
	}
#endif

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

	globalGalaxy->step(*camera,contentHandler,globalTime,deltaTime);
	Input::update(window);

  UpdateKeyboardInterface();

	TileTexture::getInstance().update();
    camera->update();

	//---------------------
	//    drawing
	//--------------------
	glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  #ifndef __EMSCRIPTEN__
    glPolygonMode(GL_FRONT_AND_BACK, wireframe?GL_LINE:GL_FILL);
  #endif

	globalGalaxy->draw(*camera);
	testCursor->draw(*camera);

	glDisable(GL_DEPTH_TEST);
	ui->draw();
	glEnable(GL_DEPTH_TEST);

	if(Input::isKeyPressed(GLFW_KEY_N))reloadAllShaders();

	if(Input::isKeyHold(GLFW_KEY_LEFT_SHIFT))
	{
		if(Input::isKeyHold(GLFW_KEY_P))globalTime+=1.0f*deltaTime;
		if(Input::isKeyHold(GLFW_KEY_M))globalTime-=1.0f*deltaTime;
	}else{
		if(Input::isKeyHold(GLFW_KEY_P))globalTime+=0.1f*deltaTime;
		if(Input::isKeyHold(GLFW_KEY_M))globalTime-=0.1f*deltaTime;
	}

	if(Input::isKeyPressed(GLFW_KEY_V))testBool1^=1;
	if(Input::isKeyPressed(GLFW_KEY_B))testBool2^=1;
	if(Input::isKeyPressed(GLFW_KEY_0))globalTime=0.0f;

	// printf("test %d\n",testVal);
	testVal=0;

	contentHandler.handleNewContent();

	#ifndef NTWBAR
		// Draw tweak bars (or don't)
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifndef __EMSCRIPTEN__
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
#endif
		TwDraw();
	#endif

	glfwSwapBuffers(window);
	glfwPollEvents();

#ifdef __EMSCRIPTEN__
  while(true) {
    bool executed = contentHandler.ExecuteOneTask();
    if (!executed)
      break;

    float frame_time_end = (float)glfwGetTime();
    deltaTime = frame_time_end - frame_time_start;
    if (deltaTime > 1.0 / 30.0) {
      break;
    }
  }
  UpdateTaskLoad(contentHandler.TaskListSize());
#endif
}

Application::~Application()
{
	#ifndef NTWBAR
		TwTerminate();
	#endif
	glfwTerminate();
}
