#pragma once
#include <GLFW/glfw3.h>

typedef struct Vector3 Vector3;
struct Vector3
{
    float x,y,z;
};

typedef struct Camera Camera;
struct Camera
{
     GLFWwindow* window;
    float position[3];
    float front[3];
    float up[3];
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;
    float last_x;
    float last_y;
    int mb1;
};

void camera_init(Camera *camera);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) ;

void get_view_matrix(Camera *camera, float* viewMatrix) ;

void process_input(Camera* camera,GLFWwindow *window, float deltaTime);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void update_front(Camera* camera);