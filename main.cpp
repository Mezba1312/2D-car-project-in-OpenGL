#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

unsigned int VAO, VBO, EBO;
unsigned int shaderProgram;

// Global Initial variable
float carX = 0.0f;
float carScale = 1.0f;
float carRotation = 0.0f;// Road+car
float wheelRotation = 0.0f;
float speed = 0.0006f;
const float PI = 3.1415926535f;


string loadShader(const char* fileName) {
    ifstream file(fileName, ios::binary);
    stringstream buffer;
    if (file.is_open()) {
        buffer << file.rdbuf();
        string content = buffer.str();
        if (content.size() >= 3 && (unsigned char)content[0] == 0xEF && (unsigned char)content[1] == 0xBB && (unsigned char)content[2] == 0xBF)
            content.erase(0, 3);
        return content;
    }
    return "";
}

// 4*4 transform matrix
void setTransform(float x, float y, float rot, float scl) {
    float s = sin(rot), c = cos(rot);
    float matrix[16] = {
        c * scl,  s * scl, 0, 0,
       -s * scl,  c * scl, 0, 0,
        0,        0,       1, 0,
        x,        y,       0, 1
    };
    int loc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
}

// Shape Function
void drawRectangle(float x1, float y1, float x2, float y2) {
    float vertices[] = { x1, y1, x2, y1, x2, y2, x1, y2 };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawTrapezium(float blx, float bly, float brx, float bry, float trx, float tryy, float tlx, float tly) {
    float vertices[] = { blx, bly, brx, bry, trx, tryy, tlx, tly };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    float vertices[] = { x1, y1, x2, y2, x3, y3 };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawCircle(float cx, float cy, float r) {
    const int segments = 80;
    vector<float> vertices;
    vertices.push_back(cx); vertices.push_back(cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        vertices.push_back(cx + r * cos(angle));
        vertices.push_back(cy + r * sin(angle));
    }
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
}

// Keyboard Input
void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { carX += speed; wheelRotation -= 0.1f; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { carX -= speed; wheelRotation += 0.1f; }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { wheelRotation -= 0.05f; }


    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { carRotation -= 0.005f; }

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) carScale += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) carScale -= 0.001f;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1000, 750, "Car Assingment", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    string vS = loadShader("vertex_shader.vs");
    string fS = loadShader("fragment_shader.fs");
    const char* vC = vS.c_str(), * fC = fS.c_str();
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vC, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fC, NULL); glCompileShader(fs);
    shaderProgram = glCreateProgram(); glAttachShader(shaderProgram, vs); glAttachShader(shaderProgram, fs); glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);

    int colorLoc = glGetUniformLocation(shaderProgram, "ourColor");

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.0f, 0.54f, 0.61f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);

        // Road
        setTransform(0, 0, carRotation, carScale);
        glUniform3f(colorLoc, 0.12f, 0.12f, 0.12f);
        drawRectangle(-1.2f, -0.62f, 1.2f, -0.65f);


        float s = sin(carRotation), c = cos(carRotation);
        float carGlobalX = carX * c * carScale;
        float carGlobalY = carX * s * carScale;
        setTransform(carGlobalX, carGlobalY, carRotation, carScale);

        glUniform3f(colorLoc, 0.82f, 0.1f, 0.15f);
        drawRectangle(-0.65f, -0.2f, 0.65f, -0.45f);
        drawTrapezium(-0.75f, -0.2f, -0.65f, -0.45f, -0.65f, -0.2f, -0.65f, -0.2f);
        glUniform3f(colorLoc, 0.85f, 0.12f, 0.18f);
        drawTrapezium(-0.45f, -0.2f, 0.4f, -0.2f, 0.18f, 0.15f, -0.25f, 0.15f);

        // Glass
        glUniform3f(colorLoc, 0.7f, 0.9f, 1.0f);
        drawTrapezium(-0.41f, -0.18f, -0.01f, -0.18f, -0.01f, 0.12f, -0.25f, 0.12f);
        drawTrapezium(0.02f, -0.18f, 0.35f, -0.18f, 0.16f, 0.12f, 0.02f, 0.12f);

        //  Handles, Lights
        glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);
        drawRectangle(-0.01f, -0.2f, 0.01f, -0.45f);
        drawRectangle(0.12f, -0.25f, 0.18f, -0.27f); drawRectangle(-0.18f, -0.25f, -0.12f, -0.27f);
        glUniform3f(colorLoc, 1.0f, 0.85f, 0.0f); drawRectangle(0.62f, -0.25f, 0.68f, -0.32f);
        glUniform3f(colorLoc, 0.5f, 0.0f, 0.0f); drawRectangle(-0.72f, -0.25f, -0.68f, -0.32f);

        // ৩. wheel
        float wheelX[] = { -0.38f, 0.38f };
        for (int i = 0; i < 2; i++) {
            float wX = wheelX[i], wY = -0.48f;
            float finalX = (carX + wX) * c * carScale - wY * s * carScale;
            float finalY = (carX + wX) * s * carScale + wY * c * carScale;


            setTransform(finalX, finalY, wheelRotation + carRotation, carScale);

            glUniform3f(colorLoc, 0.1f, 0.1f, 0.1f); drawCircle(0, 0, 0.13f); // Tyre
            glUniform3f(colorLoc, 0.8f, 0.8f, 0.8f); drawCircle(0, 0, 0.07f);
            glUniform3f(colorLoc, 0.25f, 0.25f, 0.25f);
            for (int j = 0; j < 8; j++) {
                float angle = j * (2.0f * PI / 8.0f);
                drawTriangle(0, 0, 0.08f * cos(angle), 0.08f * sin(angle), 0.08f * cos(angle + 0.35f), 0.08f * sin(angle + 0.35f));
            }
        }

        glfwSwapBuffers(window); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}