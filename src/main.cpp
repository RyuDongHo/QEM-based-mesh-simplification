
#define GLM_ENABLE_EXPERIMENTAL
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "common.h"

// window size & position
const GLuint WIN_W = 1600;
const GLuint WIN_H = 900;
const GLuint WIN_X = 800;
const GLuint WIN_Y = 450;
float aspectRatio = (float)WIN_W / (float)WIN_H;
GLFWwindow* window;

// OBJ mesh data
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<glm::vec4> colors; // Generated colors for each vertex
int numVertices = 0;

// Sphere data (backup)
int levelSphere = 3;
int numVertSphere = 0;
glm::vec4 vertSphere[10240];
glm::vec4 normSphere[10240];
glm::vec4 colorSphere[10240];

GLuint vao;
GLuint vbo;
GLuint textureID;

float theta = 0.f;
float fov = 45.f;

// track ball matrix
glm::mat4 matDrag = glm::mat4(1.f);
glm::mat4 matUpdated = glm::mat4(1.f);

// mvp matrix
glm::mat4 matModel = glm::mat4(1.f);
glm::mat4 matView = glm::mat4(1.f);
glm::mat4 matProj = glm::mat4(1.f);

// object info variable
GLfloat color[4] = { 0.933f, 0.769f, 0.898f, 1.0f };

GLuint programID;

int dragMode = GL_FALSE;
glm::vec2 dragStart = glm::vec2(1.f);

void DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const GLvoid* userParam) {
	printf("Type: %#x; Source: %#x; ID: %d; Severity: %#x\n", type, source, id, severity);
	printf("Message: %s\n", message);
	fflush(stdout);
}

void initFunc() {
	// program settings
	programID = LoadShader("../../shader/vertex.glsl", "../../shader/fragment.glsl");

	if (programID == 0) {
		printf("Shader loading failed!\n");
		return;
	}

	// vao settings
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vbo settings
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	// Convert vec3 vertices to vec4 and prepare color data
	std::vector<glm::vec4> verticesVec4;
	for (const auto& v : vertices) {
		verticesVec4.push_back(glm::vec4(v, 1.0f));
	}
	
	size_t vertexSize = verticesVec4.size() * sizeof(glm::vec4);
	size_t colorSize = colors.size() * sizeof(glm::vec4);
	size_t uvSize = uvs.size() * sizeof(glm::vec2);
	
	glBufferData(GL_ARRAY_BUFFER, vertexSize + colorSize + uvSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, verticesVec4.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertexSize, colorSize, colors.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertexSize + colorSize, uvSize, uvs.data());

	// Setup vertex attributes (using layout locations from shader)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)vertexSize);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexSize + colorSize));
	glEnableVertexAttribArray(2);

	// clear
	glClearColor(0.5f, 0.8f, 0.8f, 1.0f);
	glClearDepthf(1.0f);
	glUseProgram(programID);

	// cullision mode settings
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
}
void updateFunc() {
	float elapsedTime = (float)glfwGetTime();
	theta = elapsedTime * (3.141592f / 2.f);
	float radius = 3.f;
	matView = glm::lookAt(
		//glm::vec3(radius * cosf(theta), 0.f, radius * sinf(theta)),
		glm::vec3(0.f, 0.f, radius),
		glm::vec3(0.f),
		glm::vec3(0.f, 1.f, 0.f));
	matProj = glm::perspectiveRH(
		glm::radians(fov),  // FOV (시야각)
		aspectRatio,         // aspect ratio (width/height)
		0.1f,                 // zNear
		50.0f                // zFar
	);
}
void drawFunc() {
	glClearColor(0.5f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.f, 1.f);
	GLint win_w, win_h;
	glfwGetWindowSize(window, &win_w, &win_h);
	aspectRatio = (float)win_w / (float)win_h;
	GLint map_x = (GLint)(win_w * 0.7f);
	GLint map_y = (GLint)(win_h * 0.05f);
	GLsizei map_w = (GLsizei)(win_w * 0.25f);
	GLsizei map_h = (GLsizei)(win_h * 0.25f);

	glBindVertexArray(vao);
	glUseProgram(programID);

	GLuint loc;
	loc = glGetUniformLocation(programID, "modelMat");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matModel));

	loc = glGetUniformLocation(programID, "viewMat");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matView));

	loc = glGetUniformLocation(programID, "projMat");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matProj));

	// Bind texture and set useTexture to true
	if (textureID != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		loc = glGetUniformLocation(programID, "textureSampler");
		glUniform1i(loc, 0);
		loc = glGetUniformLocation(programID, "useTexture");
		glUniform1i(loc, 1);
	} else {
		loc = glGetUniformLocation(programID, "useTexture");
		glUniform1i(loc, 0);
	}

	glViewport(0, 0, win_w, win_h);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glEnable(GL_SCISSOR_TEST);
	glScissor(map_x, map_y, map_w, map_h);
	glViewport(map_x, map_y, map_w, map_h);
	glClearColor(0.5f, 0.5f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glScissor(0, 0, win_w, win_h);
	glDisable(GL_SCISSOR_TEST);
	
	glFinish();
	
}
void cursorPosFunc(GLFWwindow* win, double xscr, double yscr)
{
	if (dragMode) {
		glm::vec2 dragCur = glm::vec2((GLfloat)xscr, (GLfloat)yscr);
		matDrag = calcTrackball(dragStart, dragCur, (float)WIN_W, (float)WIN_H);
		matModel = matDrag * matUpdated;
	}
}
void mouseButtonFunc(GLFWwindow* win, int button, int action, int mods)
{
	GLdouble x, y;
	switch (action) {
	case GLFW_PRESS:
		dragMode = GL_TRUE;
		glfwGetCursorPos(win, &x, &y);
		dragStart = glm::vec2((GLfloat)x, (GLfloat)y);
		break;
	case GLFW_RELEASE:
		dragMode = GL_FALSE;
		glfwGetCursorPos(win, &x, &y);
		glm::vec2 dragCur = glm::vec2((GLfloat)x, (GLfloat)y);
		matDrag = calcTrackball(dragStart, dragCur, (float)WIN_W, (float)WIN_H);
		matModel = matDrag * matUpdated;
		matDrag = glm::mat4(1.0F); // reset
		matUpdated = matModel; // update to the object matrix
		break;
	}
	fflush(stdout);
}
void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			printf("escape pressed\n");
			fflush(stdout);
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_J:
		if (action == GLFW_PRESS) {
			fov = std::clamp(fov + 5.f, 0.f, 120.f);
		}
		break;
	case GLFW_KEY_K:
		if (action == GLFW_PRESS) {
			fov = std::clamp(fov - 5.f, 0.f, 120.f);
		}
		break;
	default:
		break;
	}
}
void refreshFunc(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}



int main(int argc, char* arvg[]) {

	// glfw settings
	glfwInit();
	window = glfwCreateWindow(WIN_W, WIN_H, "title", NULL, NULL);
	glfwSetWindowPos(window, WIN_X, WIN_Y);
	glfwMakeContextCurrent(window);

	// glew settings
	glewInit();

	//glfw callback settings
	glfwSetKeyCallback(window, keyFunc);
	glfwSetCursorPosCallback(window, cursorPosFunc);
	glfwSetMouseButtonCallback(window, mouseButtonFunc);

	// Load OBJ file
	bool res = loadOBJ("../../resource/mesh.obj", vertices, uvs, normals);
	if (!res) {
		printf("Failed to load OBJ file!\n");
		return -1;
	}
	numVertices = vertices.size();
	printf("Loaded %d vertices\n", numVertices);
	
	// Generate colors for each vertex
	for (size_t i = 0; i < vertices.size(); i++) {
		glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f); // White for texture
		colors.push_back(color);
	}
	
	// Load texture
	textureID = loadTexture("../../resource/texture.jpg");
	if (textureID == 0) {
		printf("Warning: Texture not loaded, using vertex colors\n");
	}
	
	// generateSphere(levelSphere); // Backup sphere generation
	// register debug callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallback(DebugLog, NULL);

	initFunc();
	// main loop
	while (!glfwWindowShouldClose(window)) {
		//dpdate
		updateFunc();
		// draw
		drawFunc();
		// error check
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("OpneGL error%x\n", err);
			fflush(stdout);
		}
		// GLFW actions
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//done
	glfwTerminate();
	return 0;
}