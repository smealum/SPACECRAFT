#ifndef INPUT_QHZPGW1N
#define INPUT_QHZPGW1N

#include <GLFW/glfw3.h>
#include <map>

class Input
{
    public:
        static void update(GLFWwindow* window);
        static bool isKeyPressed(int key);
        static bool isKeyReleased(int key);
        static bool isKeyHold(int key);
        static bool isMousePressed(int key);
        static bool isMouseReleased(int key);
        static bool isMouseHold(int key);
        static double mouseX();
        static double mouseY();
};

#endif /* end of include guard: INPUT_QHZPGW1N */
