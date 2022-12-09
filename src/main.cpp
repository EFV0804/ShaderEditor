#define GLFW_INCLUDE_NONE

#include "Application.h"

int main() {

    Application& app = Application::Get();
    app.init("ShaderEditor");
    app.run();
    app.close();

    return 0;
}