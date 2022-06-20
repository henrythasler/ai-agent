#include <string>
#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "util/util.h"

std::string programName = "AI-Agent Simulation";
int windowWidth = 1200,
    windowHeight = 800;
float highDPIscaleFactor = 1.0;
float backgroundR = 0.1f,
      backgroundG = 0.3f,
      backgroundB = 0.2f;

float frameTime = 0.0f;
float prevTimestamp = 0.0f;

std::filesystem::path currentPath = ".";
std::filesystem::path basePath = ".";

std::string fontName = "JetBrainsMono-ExtraLight.ttf";
std::string vertexShaderFileName = "/home/henry/dev/ai-agent/assets/shader/demo.vert";
std::string fragmentShaderFileName = "/home/henry/dev/ai-agent/assets/shader/demo.frag";

GLFWwindow *glfWindow = NULL;

GLint timeLocation;

unsigned int shaderProgram, VBO, VAO;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void glfw_error_callback(int error, const char *description)
{
    std::cerr << "[ERROR] GLFW error: " << error << ", " << description << std::endl;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void teardown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    if (glfWindow != NULL)
    {
        glfwDestroyWindow(glfWindow);
    }
    glfwTerminate();
}

bool initializeGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        std::cerr << "[ERROR] Couldn't initialize GLFW" << std::endl;
        return false;
    }
    else
    {
        std::cout << "[INFO] GLFW initialized" << std::endl;
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfWindow = glfwCreateWindow(
        windowWidth,  // mode->width,
        windowHeight, // mode->height,
        programName.c_str(),
        NULL, // monitor
        NULL);

    if (!glfWindow)
    {
        std::cerr << "[ERROR] Couldn't create a GLFW window" << std::endl;
        return false;
    }

    glfwSetKeyCallback(glfWindow, key_callback);

    glfwSetWindowPos(glfWindow, 100, 100);
    glfwSetWindowSizeLimits(
        glfWindow,
        static_cast<int>(900 * highDPIscaleFactor),
        static_cast<int>(500 * highDPIscaleFactor),
        GLFW_DONT_CARE,
        GLFW_DONT_CARE);

    // watch window resizing
    glfwSetFramebufferSizeCallback(glfWindow, framebuffer_size_callback);

    glfwMakeContextCurrent(glfWindow);
    // VSync
    glfwSwapInterval(1);

    std::cout << "[INFO] OpenGL from GLFW "
              << glfwGetWindowAttrib(glfWindow, GLFW_CONTEXT_VERSION_MAJOR)
              << "."
              << glfwGetWindowAttrib(glfWindow, GLFW_CONTEXT_VERSION_MINOR)
              << std::endl;

    return true;
}

bool initializeGLAD()
{
    // load all OpenGL function pointers with glad
    // without it not all the OpenGL functions will be available,
    // such as glGetString(GL_RENDERER), and application might just segfault
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize GLAD" << std::endl;
        return false;
    }
    else
    {
        std::cout << "[INFO] GLAD initialized" << std::endl;
    }

    std::cout << "[INFO] OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "[INFO] OpenGL from glad "
              << GLVersion.major << "." << GLVersion.minor
              << std::endl;

    return true;
}

bool initializeDearImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.Fonts->AddFontFromFileTTF(
        fontName.c_str(),
        24.0f * highDPIscaleFactor,
        NULL,
        NULL);
    // setImGuiStyle(highDPIscaleFactor);

    // setup platform/renderer bindings
    if (!ImGui_ImplGlfw_InitForOpenGL(glfWindow, true))
    {
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init())
    {
        return false;
    }

    return true;
}

// build and compile our shader program
void buildShaderProgram()
{
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::string vertexShaderSource = readFile(vertexShaderFileName);
    // std::cout << "[DEBUG] vertexShaderSource: " << vertexShaderSource << std::endl;

    const char *vertexShaderData = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderData, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string fragmentShaderSource = readFile(fragmentShaderFileName);
    // std::cout << "[DEBUG] fragmentShaderSource: " << fragmentShaderSource << std::endl;

    const char *fragmentShaderData = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderData, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    timeLocation = glGetUniformLocation(shaderProgram, "time");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] =
        {
            -1.0f,
            -1.0f,
            0.0f, // left
            1.0f,
            -1.0f,
            0.0f, // right
            1.0f,
            1.0f,
            0.0f, // top
            -1.0f,
            -1.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            -1.0f,
            1.0f,
            0.0f,
        };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // you can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other VAOs requires a call
    // to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs)
    // when it's not directly necessary
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void composeDearImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    // standard demo window
    // ImGui::ShowDemoWindow();

    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + work_size.x - PAD;
    window_pos.y = work_pos.y + PAD;
    window_pos_pivot.x = 1.0f;
    window_pos_pivot.y = 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Once, window_pos_pivot);

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (ImGui::Begin("Example: Simple overlay", NULL, window_flags))
    {
        ImGui::Text("Framerate: %.0fHz (%.2fms)", 1/frameTime, frameTime*1000.);
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.0f,%.0f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
    }
    ImGui::End();
}

int main(int argc, char *argv[])
{
    std::cout << "["
              << currentTime(std::chrono::system_clock::now())
              << "] "
              << "Start\n- - -\n\n";

    // setting paths to resources
    currentPath = std::filesystem::current_path();
    std::cout << "[DEBUG] Current working directory: " << currentPath << std::endl;

    // basePath = std::filesystem::path(argv[0]).parent_path();
    basePath = std::filesystem::path(argv[0]).remove_filename();
    basePath = currentPath / basePath;

    std::cout << "[DEBUG] Executable name/path: " << argv[0] << std::endl
              << "parent path: " << basePath << std::endl
              << std::endl;
    fontName = "/home/henry/dev/ai-agent/assets/fonts/JetBrainsMono/JetBrainsMono-ExtraLight.ttf"; //(basePath / "assets/fonts/JetBrainsMono" / fontName).string();

    std::cout << "[DEBUG] Font filename: " << fontName << std::endl;

    if (!initializeGLFW())
    {
        std::cerr << "[ERROR] GLFW initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    if (!initializeGLAD())
    {
        std::cerr << "[ERROR] glad initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    if (!initializeDearImGui())
    {
        std::cerr << "[ERROR] Dear ImGui initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    // build and compile our shader program
    buildShaderProgram();

    // rendering loop
    while (!glfwWindowShouldClose(glfWindow))
    {

        float currTimestamp = glfwGetTime();
        frameTime = currTimestamp - prevTimestamp;
        prevTimestamp = currTimestamp;

        // the frame starts with a clean scene
        glClearColor(backgroundR, backgroundG, backgroundB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // draw our triangle
        glUseProgram(shaderProgram);
        glUniform1f(timeLocation, currTimestamp);
        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // Dear ImGui frame
        composeDearImGuiFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfWindow);

        // continuous rendering, even if window is not visible or minimized
        glfwPollEvents();
        // or you can sleep the thread until there are some events
        // in case of running animations (glTF, for example), also call glfwPostEmptyEvent() in render()
        // glfwWaitEvents();
    }

    teardown();

    std::cout << "\n- - -\n"
              << "["
              << currentTime(std::chrono::system_clock::now())
              << "] "
              << "Quit\n";

    return 0;
}