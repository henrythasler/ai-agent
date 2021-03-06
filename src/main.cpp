#include <string>
#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "util/util.h"
#include "util/shader.h"

const std::string programName = "AI-Agent Simulation";
const float frameCounterInterval_s = 1.0;
const float highDPIscaleFactor = 1.0;
const float backgroundR = 0.1f, backgroundG = 0.3f, backgroundB = 0.2f;
const bool useFullscreen = false;

int windowWidth = 1200,
    windowHeight = 800;

float frameTime = .1f;
float prevTimestamp = 0.0f;
int frameCounter = 0;
int iFrame = 0;

glm::vec2 viewportCenter = {0,0};
float viewportZoom = 1.0;

std::filesystem::path currentPath = ".";
std::filesystem::path basePath = ".";

std::string fontName = "JetBrainsMono-ExtraLight.ttf";
std::string vertexShaderFileName = "/home/henry/dev/ai-agent/assets/shader/world.vert";
std::string fragmentShaderFileName = "/home/henry/dev/ai-agent/assets/shader/world.frag";

GLFWwindow *glfWindow = nullptr;
GLFWmonitor *monitor = nullptr;
const GLFWvidmode *mode = nullptr;

GLuint shaderProgram, VBO, VAO, texture, population;
float pixels[] = {
    0.9f, 0.9f, 0.9f,   1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,   0.9f, 0.9f, 0.9f};

float populationData[256 * 4];

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
    windowWidth = width;
    windowHeight = height;
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

    if (glfWindow)
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

    if (useFullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
        mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    }

    glfWindow = glfwCreateWindow(
        mode ? mode->width : windowWidth,
        mode ? mode->height : windowHeight,
        programName.c_str(),
        monitor,
        nullptr);

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

    std::cout << "[INFO] OpenGL Renderer: " << glGetString(GL_RENDERER) << " (" << glGetString(GL_VENDOR) << ")" << std::endl;
    std::cout << "[INFO] OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
    std::cout << "[INFO] Shader Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Check GL_MAX_UNIFORM_BLOCK_SIZE to make sure we can allocate enough UBO memory
    int32_t maxUniformBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    std::cout << "[INFO] GL_MAX_UNIFORM_BLOCK_SIZE: " << maxUniformBlockSize << std::endl;
    if(maxUniformBlockSize < 65536) 
    {
        std::cerr << "[ERROR] GL_MAX_UNIFORM_BLOCK_SIZE must be >=65536" << std::endl;
        return false;
    }

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
        nullptr,
        nullptr);
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
bool buildShaderProgram()
{
    if (!shader::loadShader(vertexShaderFileName.c_str(), fragmentShaderFileName.c_str(), nullptr /*geometryShader*/, &shaderProgram))
    {
        return false;
    }

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

    // add texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // unbind for now
    glBindTexture(GL_TEXTURE_2D, 0);

    // add uniform buffer object for population
    unsigned int uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, "Population");
    glUniformBlockBinding(shaderProgram, uniformBlockIndex, 0);

    glGenBuffers(1, &population);
    glBindBuffer(GL_UNIFORM_BUFFER, population);
    glBufferData(GL_UNIFORM_BUFFER, 32772, nullptr, GL_STATIC_DRAW); // allocate 12 KiB of memory
    glBindBuffer(GL_UNIFORM_BUFFER, 0);    
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, population, 0, 32772);

    // uncomment this call to draw in wireframe polygons
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return true;
}

void composeDearImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

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
    if (ImGui::Begin("Example: Simple overlay", nullptr, window_flags))
    {
        ImGui::Text("Framerate: %.0fHz (%.2fms)", 1 / frameTime, frameTime * 1000.);
        ImGui::Text("Frame: %i", iFrame);
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse: (%.0f,%.0f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse: <invalid>");
        ImGui::Text("Center: (%.0f,%.0f)", viewportCenter.x, viewportCenter.y);
        ImGui::Text("Zoom: %.0f", viewportZoom);
    }
    ImGui::End();
}

void processUserInteraction()
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 dragdelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    if(ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        viewportCenter = glm::vec2(-dragdelta.x, dragdelta.y);
    }
    viewportZoom += io.MouseWheel;
}

int main(int argc, char *argv[])
{
    std::cout << std::endl
              << "[INFO] Start " << util::currentTime(std::chrono::system_clock::now()) << std::endl;

    // setting paths to resources
    currentPath = std::filesystem::current_path();
    std::cout << "[DEBUG] Current working directory: " << currentPath << std::endl;

    // basePath = std::filesystem::path(argv[0]).parent_path();
    basePath = std::filesystem::path(argv[0]).remove_filename();
    basePath = currentPath / basePath;

    std::cout << "[DEBUG] Executable name/path: " << argv[0] << " "
              << "parent path: " << basePath << std::endl;

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
    if (!buildShaderProgram())
    {
        std::cerr << "[ERROR] Shader initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    // rendering loop
    while (!glfwWindowShouldClose(glfWindow))
    {
        float currTimestamp = glfwGetTime();
        frameCounter++;
        iFrame++;

        // update only every second (http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/)
        if ((currTimestamp - prevTimestamp) > frameCounterInterval_s)
        {
            frameTime = (currTimestamp - prevTimestamp) / float(frameCounter);
            prevTimestamp = currTimestamp;
            frameCounter = 0;
        }

        // the frame starts with a clean scene
        glClearColor(backgroundR, backgroundG, backgroundB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // draw our triangle
        glUseProgram(shaderProgram);

        shader::setFloat(shaderProgram, "iFrame", iFrame);
        shader::setFloat(shaderProgram, "iTime", currTimestamp);
        shader::setVec2(shaderProgram, "iResolution", glm::vec2(windowWidth, windowHeight));
        shader::setVec2(shaderProgram, "iViewportCenter", viewportCenter);
        shader::setFloat(shaderProgram, "iZoom", exp(-viewportZoom/10.));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_UNIFORM_BUFFER, population);
        int popCount = (iFrame) % 1000;
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &popCount); 
        // glBindBuffer(GL_UNIFORM_BUFFER, 0);        

        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // Dear ImGui frame
        composeDearImGuiFrame();

        // User Interaction
        processUserInteraction();

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

    std::cout << "[INFO] Exit " << util::currentTime(std::chrono::system_clock::now()) << std::endl;
    return 0;
}