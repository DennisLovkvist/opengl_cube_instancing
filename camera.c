#include "camera.h"
#include <math.h>

void normalize(float* vector) 
{
    float length = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
}
void cross_product(const float* v1, const float* v2, float* result) 
{
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
void subtract(const float* v1, const float* v2, float* result) 
{
    result[0] = v1[0] - v2[0];
    result[1] = v1[1] - v2[1];
    result[2] = v1[2] - v2[2];
}
float dotProduct(const float* v1, const float* v2) 
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
float radians(float degrees) 
{
    return degrees * (float)(3.14) / 180.0f;
}
void look_at(const float* eye, const float* target, const float* up, float* view_matrix)
 {
    float f[3];
    float r[3];
    float u[3];

    subtract(target, eye, f);
    normalize(f);

    cross_product(f, up, r);
    normalize(r);

    cross_product(r, f, u);

    view_matrix[0] =  r[0];
    view_matrix[1] =  u[0];
    view_matrix[2] = -f[0];
    view_matrix[3] =  0.0f;

    view_matrix[4] =  r[1];
    view_matrix[5] =  u[1];
    view_matrix[6] = -f[1];
    view_matrix[7] =  0.0f;

    view_matrix[8]  =  r[2];
    view_matrix[9]  =  u[2];
    view_matrix[10] = -f[2];
    view_matrix[11] =  0.0f;

    view_matrix[12] = -dotProduct(r, eye);
    view_matrix[13] = -dotProduct(u, eye);
    view_matrix[14] =  dotProduct(f, eye);
    view_matrix[15] =  1.0f;
}
void move_front(Camera *camera, float distance) 
{
    camera->position[0] += camera->front[0] * distance;
    camera->position[1] += camera->front[1] * distance;
    camera->position[2] += camera->front[2] * distance;
}

void move_right(Camera *camera, float distance) 
{
    float right[3];
    cross_product(camera->front, camera->up, right);
    normalize(right);
    camera->position[0] += right[0] * distance;
    camera->position[1] += right[1] * distance;
    camera->position[2] += right[2] * distance;
}
void move_up(Camera *camera, float distance) 
{
    camera->position[1] += distance;
}
void move_down(Camera *camera, float distance) 
{
    camera->position[1] -= distance;
}
void get_view_matrix(Camera *camera, float* viewMatrix) 
{
    float target[3];
    target[0] = camera->position[0] + camera->front[0];
    target[1] = camera->position[1] + camera->front[1];
    target[2] = camera->position[2] + camera->front[2];

    look_at(camera->position, target, camera->up, viewMatrix);
}
void update_front(Camera* camera) 
{
    float radYaw   = radians(camera->yaw);
    float radPitch = radians(camera->pitch);

    camera->front[0] = cos(radYaw) * cos(radPitch);
    camera->front[1] = sin(radPitch);
    camera->front[2] = sin(radYaw) * cos(radPitch);

    normalize(camera->front);
}
void camera_init(Camera *camera)
{
    camera->front[0] = 0.0f;
    camera->front[1] = 0.0f;
    camera->front[2] = -1.0f;

    camera->up[0] = 0.0f;
    camera->up[1] = 1.0f;
    camera->up[2] = 0.0f;

    camera->yaw         = -133.0f;
    camera->pitch       = -30.0f;
    camera->speed       = 20.5f;
    camera->sensitivity = 0.1f;
    camera->zoom        = 45.0f;

    camera->position[0] = 106;
    camera->position[1] = 84;
    camera->position[2] = 102;

    camera->mb1 = 0;
}
void process_input(Camera* camera,GLFWwindow *window, float dt) 
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move_front(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move_front(camera, -camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        move_right(camera, -camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        move_right(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        move_up(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        move_down(camera, camera->speed * dt);
    }
}

void mouseCallback(GLFWwindow* window, double position_x, double position_y) 
{
    Camera* camera = (Camera*)(glfwGetWindowUserPointer(window));

    float offset_x = position_x - camera->last_x;
    float offset_y = camera->last_y - position_y;

    camera->last_x = position_x;
    camera->last_y = position_y;

    offset_x *= camera->sensitivity;
    offset_y *= camera->sensitivity;

    if(camera->mb1)
    {
        camera->yaw   += offset_x;
        camera->pitch += offset_y;

        if (camera->pitch > 89.0f)
        {
            camera->pitch = 89.0f;
        }
        if (camera->pitch < -89.0f)
        {
            camera->pitch = -89.0f;
        }

        update_front(camera);
    }
    
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        Camera* camera = (Camera*)(glfwGetWindowUserPointer(window));
        camera->mb1 = 1;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
    {
        Camera* camera = (Camera*)(glfwGetWindowUserPointer(window));
        camera->mb1 = 0;
    }
}
void scrollCallback(GLFWwindow* window, double offset_x, double offset_y) 
{
    Camera* camera = (Camera*)(glfwGetWindowUserPointer(window));
    camera->zoom -= (float)(offset_y);

    if (camera->zoom < 1.0f)
    {
        camera->zoom = 1.0f;
    }
    if (camera->zoom > 45.0f)
    {
        camera->zoom = 45.0f;
    }
}
