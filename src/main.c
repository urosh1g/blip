#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define igGetIO igGetIO_Nil
#include <cimgui/cimgui.h>
#include <cimgui/cimgui_impl.h>
#include <cglm/cglm.h>
#include <shader.h>
#include <camera.h>
#include <texture.h>
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

camera_t* global_camera = NULL;
float last_x = 0, last_y = 0;
bool first_pass = true;

float last_frame = 0;
float delta_time = 0;

struct ImGuiContext* imgui_ctx;
struct ImGuiIO* imgui_io;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    (void)window;
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
    // add necessary key checks here
    float amount = 5.0f * delta_time;
    switch (key) {
    case GLFW_KEY_W:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera_move(global_camera, DIRECTION_FRONT, amount);
        }
        break;
    case GLFW_KEY_S:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera_move(global_camera, DIRECTION_BACK, amount);
        }
        break;
    case GLFW_KEY_A:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera_move(global_camera, DIRECTION_LEFT, amount);
        }
        break;
    case GLFW_KEY_D:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera_move(global_camera, DIRECTION_RIGHT, amount);
        }
        break;
    case GLFW_KEY_APOSTROPHE:
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        break;
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)button;
    (void)action;
    (void)mods;
    // add necessary mouse checks here
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    (void)xpos;
    (void)ypos;
    if (first_pass) {
        first_pass = false;
        last_x = xpos;
        last_y = ypos;
        return;
    }
    float xoffset = (xpos - last_x) * 0.1f;
    float yoffset = (last_y - ypos) * 0.1f;

    last_x = xpos;
    last_y = ypos;

    camera_rotate(global_camera, xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    (void)yoffset;
    // add necessary onscroll actions here
}

void error_callback(int error_code, const char* description) {
    fprintf(stderr, "glfw error code: %d\n", error_code);
    fprintf(stderr, "%s\n", description);
    exit(EXIT_FAILURE);
}

GLFWwindow* init_glad_glfw() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init glfw\n");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "New window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create window\n");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    // enable vsync
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetErrorCallback(error_callback);

    imgui_ctx = igCreateContext(NULL);
    imgui_io = igGetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, 1);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    igStyleColorsDark(NULL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        fprintf(stderr, "Failed to init glad\n");
        exit(EXIT_FAILURE);
    }
    return window;
}

int main() {
    GLFWwindow* window = init_glad_glfw();

    GLuint vert_shader = shader_load("vs.glsl", GL_VERTEX_SHADER);
    GLuint frag_shader = shader_load("fs.glsl", GL_FRAGMENT_SHADER);

    GLuint shaders[2];
    shaders[0] = vert_shader;
    shaders[1] = frag_shader;
    GLuint program = program_link(shaders, 2);

    float vertices[] = {// coords	  //tex_coords
                        -0.5, 0.5,  0, 0, 1, -0.5, -0.5, 0, 0, 0,
                        0.5,  -0.5, 0, 1, 0, 0.5,  0.5,  0, 1, 1};
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int img_id = tex_load("./assets/img.png", true);
    (void) img_id2;
    program_use(program);
    glActiveTexture(GL_TEXTURE0);
    tex_bind(img_id);
    GLuint tex_uniform = glGetUniformLocation(program, "tex");
    glUniform1i(tex_uniform, 0);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    vec3 camera_pos = {0, 0, 2};
    vec3 world_up = {0, 1, 0};
    camera_t* camera = camera_create(camera_pos, world_up, 16 / 9.0f, 0.1f,
                                     100.0f, 45.0f, CAMERA_PERSPECTIVE);

    global_camera = camera;

    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float)glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program_use(program);

        GLuint model_id = glGetUniformLocation(program, "model");
        GLuint view_id = glGetUniformLocation(program, "view");
        GLuint projection_id = glGetUniformLocation(program, "projection");
        (void)camera;

        glUniformMatrix4fv(model_id, 1, GL_FALSE, (const float*)model);
        glUniformMatrix4fv(view_id, 1, GL_FALSE, (const float*)camera->view);
        glUniformMatrix4fv(projection_id, 1, GL_FALSE,
                           (const float*)camera->projection);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        igShowDemoWindow(NULL);

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(imgui_ctx);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
