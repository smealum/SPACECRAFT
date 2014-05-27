#ifndef INPUT_QHZPGW1N
#define INPUT_QHZPGW1N

#include <map>

struct GLFWwindow;

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
	static void setMousePos(double x, double y); // relative
};

#endif /* end of include guard: INPUT_QHZPGW1N */
