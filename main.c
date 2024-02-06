#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "camera.h"

typedef struct Cube Cube;
struct Cube
{
    Vector3 *rotations;
};


const char* vertex_shader_source =   "//Vertex shader code                                                                      \n"
                                     "                                                                                          \n"
                                     "#version 330 core                                                                         \n"
                                     "                                                                                          \n"
                                     "layout (location = 0) in vec3 position;                                                   \n"
                                     "layout (location = 1) in vec3 normal;                                                     \n"
                                     "layout (location = 2) in vec3 instance_position;                                          \n"
                                     "layout (location = 3) in vec3 instance_color;                                             \n"
                                     "layout (location = 4) in mat4 instance_transform;                                         \n"
                                     "                                                                                          \n"
                                     "out vec3 v_instance_color;                                                                \n"
                                     "out vec3 v_normal;                                                                        \n"
                                     "                                                                                          \n"
                                     "uniform mat4 projection;                                                                  \n"
                                     "uniform mat4 view;                                                                        \n"
                                     "                                                                                          \n"
                                     "void main()                                                                               \n"
                                     "{                                                                                         \n"
                                     "   vec4 world_position = instance_transform * vec4(position + instance_position, 1.0);    \n"
                                     "   gl_Position = projection * view * world_position;                                      \n"
                                     "   v_instance_color = instance_color;                                                     \n"
                                     "   v_normal = mat3(transpose(inverse(instance_transform))) * normal;                      \n"
                                     "}                                                                                         \0";




const char* fragment_shader_source = "//Fragment shader code                                                                    \n"
                                     "                                                                                          \n"
                                     "#version 330 core                                                                         \n"
                                     "                                                                                          \n"
                                     "in vec3 v_instance_color;                                                                 \n"
                                     "in vec3 v_normal;                                                                         \n"
                                     "out vec4 fragment_color;                                                                  \n"
                                     "                                                                                          \n"
                                     "uniform vec3 lights_direction[4] = {vec3(1,0,0),vec3(-1,0,0),vec3(0,1,0),vec3(0,0,0)};    \n"
                                     "uniform vec3 lights_color[4]     = {vec3(1,0,0),vec3(1,0,0),vec3(1,0,0),vec3(1,0,0)};     \n"
                                     "uniform vec3 object_color        = vec3(1,1,1);                                           \n"
                                     "uniform vec3 ambient_color       = vec3(1,1,1);                                           \n"
                                     "                                                                                          \n"
                                     "void main()                                                                               \n"
                                     "{                                                                                         \n"
                                     "   vec3 norm = normalize(v_normal);                                                       \n"
                                     "   vec3 diffuse = vec3(0.0);                                                              \n"
                                     "                                                                                          \n"
                                     "   for (int i = 0; i < 4; ++i)                                                            \n"
                                     "   {                                                                                      \n"
                                     "       vec3 lightDirNormalized = normalize(lights_direction[i]);                          \n"
                                     "       float diff = max(dot(norm, lightDirNormalized), 0.0);                              \n"
                                     "       diffuse += diff * lights_color[i];                                                 \n"
                                     "   }                                                                                      \n"
                                     "                                                                                          \n"
                                     "   diffuse *= object_color;                                                               \n"
                                     "   vec3 ambient = ambient_color * object_color;                                           \n"
                                     "   fragment_color = vec4(v_instance_color * (diffuse + ambient), 1.0);                    \n"
                                     "}                                                                                         \0";

void update_view_matrix(Camera *camera, GLfloat *matrix) 
{
    float viewMatrix[16];
    get_view_matrix(camera, viewMatrix);

    matrix[0]  = viewMatrix[0];
    matrix[1]  = viewMatrix[1];
    matrix[2]  = viewMatrix[2];
    matrix[3]  = viewMatrix[3];

    matrix[4]  = viewMatrix[4];
    matrix[5]  = viewMatrix[5];
    matrix[6]  = viewMatrix[6];
    matrix[7]  = viewMatrix[7];

    matrix[8]  = viewMatrix[8];
    matrix[9]  = viewMatrix[9];
    matrix[10] = viewMatrix[10];
    matrix[11] = viewMatrix[11];

    matrix[12] = viewMatrix[12];
    matrix[13] = viewMatrix[13];
    matrix[14] = viewMatrix[14];
    matrix[15] = viewMatrix[15];
}
void setIdentityMatrix(GLfloat *matrix) 
{
    matrix[0]  = 1.0f;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;
    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;
    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}
void set_rotation_matrix(GLfloat *matrix, Vector3 position, Vector3 rotation) 
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(position.x, position.y, position.z);

    glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

    glTranslatef(-position.x, -position.y, -position.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}
int main() 
{
    /*-------------------Setup GLFW--------------------*/
    if (!glfwInit())return -1;

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "GLFW Window", NULL, NULL);
    if (!window) 
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)return -1;
    glEnable(GL_DEPTH_TEST);
    /*-------------------Setup GLFW--------------------*/


    /*----------------------Data-----------------------*/
    GLint instance_count = 50*50*50;

    srand(time(NULL));

    Cube cube;
    cube.rotations = malloc(sizeof(Vector3)*instance_count);

    GLfloat *instance_positions  = malloc(sizeof(GLfloat)*instance_count * 3);
    GLfloat *instance_colors     = malloc(sizeof(GLfloat)*instance_count * 3);
    GLfloat *instance_transforms = malloc(sizeof(GLfloat)*instance_count * 16);

    for (int i = 0; i < instance_count; ++i) 
    {
        instance_positions[i * 3 + 0] = (rand() % 50);
        instance_positions[i * 3 + 1] = (rand() % 50); 
        instance_positions[i * 3 + 2] = (rand() % 50);

        instance_positions[i * 3 + 0] = i % 50;
        instance_positions[i * 3 + 1] = (i / 50)%50;
        instance_positions[i * 3 + 2] = i / (50*50);

        instance_colors[i * 3 + 0] = (rand() % 256) / 255.0f;
        instance_colors[i * 3 + 1] = (rand() % 256) / 255.0f;
        instance_colors[i * 3 + 2] = (rand() % 256) / 255.0f;

        cube.rotations[i].x += (rand() % 100) / 500.0f;
        cube.rotations[i].y += (rand() % 100) / 500.0f;
        cube.rotations[i].z += (rand() % 100) / 500.0f;

        GLfloat *current_matrix = instance_transforms + i * 16;

        Vector3 position = 
            {
                instance_positions[i * 3 + 0],
                instance_positions[i * 3 + 1],
                instance_positions[i * 3 + 2]
            };
            Vector3 rotation = 
            {
                cube.rotations[i].x,
                cube.rotations[i].y,
                cube.rotations[i].z
            };
            set_rotation_matrix(current_matrix,position,rotation);
    }
    /*----------------------Data-----------------------*/

    
    /*------------Compile and attach shader------------*/
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource (fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader  );
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram (shader_program);
    glUseProgram  (shader_program);
    /*------------Compile and attach shader------------*/

    /*-------------------Define cube-------------------*/
    int vertex_count_cube = 24;
    float vertices[] = 
    {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,  1.0f,    // 0
         0.5f, -0.5f, 0.5f, 0.0f,  0.0f,  1.0f,    // 1
         0.5f,  0.5f, 0.5f, 0.0f,  0.0f,  1.0f,    // 2
        -0.5f,  0.5f, 0.5f, 0.0f,  0.0f,  1.0f,    // 3

        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,  // 4
         0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,  // 5
         0.5f,  0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,  // 6
        -0.5f,  0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,  // 7

        // Left face
        -0.5f, -0.5f, -0.5f,-1.0f,  0.0f,  0.0f,   // 8
        -0.5f,  0.5f, -0.5f,-1.0f,  0.0f,  0.0f,   // 9
        -0.5f,  0.5f,  0.5f,-1.0f,  0.0f,  0.0f,   // 10
        -0.5f, -0.5f,  0.5f,-1.0f,  0.0f,  0.0f,   // 11

        // Right face
         0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   // 12
         0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   // 13
         0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   // 14
         0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   // 15

        // Top face
        -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,   // 16
         0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,   // 17
         0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,   // 18
        -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,   // 19

        // Bottom face
        -0.5f, -0.5f,  0.5f, 0.0f,  -1.0f,  0.0f,  // 20
         0.5f, -0.5f,  0.5f, 0.0f,  -1.0f,  0.0f , // 21
         0.5f, -0.5f, -0.5f, 0.0f,  -1.0f,  0.0f,  // 22
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f,  0.0f   // 23
    };
    /*-------------------Define cube-------------------*/

    /*--------------Create vertex buffers--------------*/
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                   //Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //Normals
    glEnableVertexAttribArray(1);

    GLuint vbo_instance_positions;
    glGenBuffers(1, &vbo_instance_positions);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 3, instance_positions, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    GLuint vbo_instance_colors;
    glGenBuffers(1, &vbo_instance_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 3, instance_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    GLuint vbo_instance_transforms;
    glGenBuffers(1, &vbo_instance_transforms);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance_transforms);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 16, instance_transforms, GL_STATIC_DRAW);

    for (int i = 0; i < 4; ++i) 
    {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(4 + i, 1);
    }
    /*--------------Create vertex buffers--------------*/


    /*--------------Camera and projection--------------*/
    GLuint uniform_location_view       = glGetUniformLocation(shader_program, "view"      );
    GLuint uniform_location_projection = glGetUniformLocation(shader_program, "projection");

    GLfloat fov          = 45.0f;
    GLfloat aspect_ratio = 1920.0f / 1080.0f;
    GLfloat near_plane   = 0.1f;
    GLfloat far_plane    = 1000.0f;

    GLfloat projection_matrix[16];
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect_ratio, near_plane, far_plane);
    glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
    glMatrixMode(GL_MODELVIEW);
    glUniformMatrix4fv(uniform_location_projection, 1, GL_FALSE, projection_matrix);

    Camera camera;
    camera_init(&camera);
    update_front(&camera);
    /*--------------Camera and projection--------------*/

    
    /*--------------------User input-------------------*/
    glfwSetWindowUserPointer(window, &camera);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    double t1 = glfwGetTime();
    /*--------------------User input-------------------*/
        

    while (!glfwWindowShouldClose(window)) 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*----------------Update Camear/View---------------*/
        double t0 = glfwGetTime();
        float dt = (float)(t0 - t1);
        t1 = t0;

        process_input(&camera,window, dt);
        GLfloat view_matrix[16];
        update_view_matrix(&camera, view_matrix);
        glUniformMatrix4fv(uniform_location_view, 1, GL_FALSE, view_matrix);
        /*----------------Update Camear/View---------------*/


        /*----------------Update transforms----------------*/
        for (int i = 0; i < instance_count; ++i) 
        {
            cube.rotations[i].x += (rand() % 100) / 500.0f;
            cube.rotations[i].y += (rand() % 100) / 500.0f;
            cube.rotations[i].z += (rand() % 100) / 500.0f;

            GLfloat *current_matrix = instance_transforms + i * 16;
            
            Vector3 position = 
            {
                instance_positions[i * 3 + 0],
                instance_positions[i * 3 + 1],
                instance_positions[i * 3 + 2]
            };
            Vector3 rotation = 
            {
                cube.rotations[i].x,
                cube.rotations[i].y,
                cube.rotations[i].z
            };
            set_rotation_matrix(current_matrix,position,rotation);
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instance_count * 16, instance_transforms, GL_STATIC_DRAW);
        /*----------------Update transforms----------------*/


        /*------------------Draw instances-----------------*/
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_QUADS, 0, vertex_count_cube, instance_count);
        glBindVertexArray(0);
        /*------------------Draw instances-----------------*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(cube.rotations);

    free(instance_positions);
    free(instance_colors   );
    free(instance_transforms   );

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vbo_instance_positions);
    glDeleteProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glfwTerminate();

    return 0;
}
