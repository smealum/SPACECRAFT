#include "Application.h"
#include "Producer.h"

int main()
{
    //TEST
        ContentInputQueue testInputQueue;
        ContentOutputQueue testOutputQueue;

        Producer p1(testInputQueue, testOutputQueue);
    //END TEST

    Application::getInstance().run();
    return 0;
}
