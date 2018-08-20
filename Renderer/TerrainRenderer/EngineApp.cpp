#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "EngineProfiler.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

EngineApp::EngineApp()
	: GLApp()
{
}

EngineApp::~EngineApp()
{
}

/**
* @ brief		deal with whole initialization of Terrain Renderer.
* @ return		return boolean whether if intializing terrain renderer is successful or not.
*/
bool EngineApp::initEngine(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!initAssets())
		return false;

	if (!initGeometryBuffer())
		return false;

	return true;
}

/**
* @ brief		initialize asset resources.
* @ return		return boolean whether if loading and initializing assets are successful or not.
*/
bool EngineApp::initAssets(void)
{
	assetManager = std::make_unique<AssetManager>();

	if (!initShader())
		return false;

	return true;
}

/** 
* @ brief		initialize shaders.
* @ return		return boolean whether if initializing shader is successful or not.
*/
bool EngineApp::initShader(void)
{
	try 
	{
		simpleShader = std::make_shared<GLShader>("../resources/shader/vertex_shader.glsl", nullptr, nullptr, nullptr, "../resources/shader/fragment_shader.glsl");

		assetManager->addAsset(simpleShader, { "../resources/shader/vertex_shader.glsl", "../resources/shader/fragment_shader.glsl" });
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("An error occurred while initializing shader file.\n{}", e.what());
		return false;
	}

	return true;
}

/**
* @ brief		initialize geometry buffer in current opengl context.
* @ return		return boolean whether if initialize geometry buffer is successful or not.
*/
bool EngineApp::initGeometryBuffer(void)
{
	typedef struct _vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	} Vertex ;

	std::vector<Vertex> _vertices = {
		// positions          // normals           // texture coords
		{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f) } ,
	    { glm::vec3(1.0f,  1.0f, -1.0f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(1.0f, -1.0f, -1.0f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f,  1.0f, -1.0f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f) } ,
	    { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  1.0f) } ,
	    
	    { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f) } ,
	    { glm::vec3(1.0f, -1.0f,  1.0f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f) } ,
	    
	    { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
	    { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f, -1.0f, -1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(1.0f,  1.0f, -1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(1.0f, -1.0f, -1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f, -1.0f,  1.0f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
	    
	    { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(1.0f, -1.0f, -1.0f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(1.0f, -1.0f,  1.0f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f, -1.0f,  1.0f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
	    { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    
	    { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(1.0f,  1.0f, -1.0f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
	    { glm::vec3(1.0f,  1.0f,  1.0f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
	    { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
	    { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  0.0f) }
	};

	std::vector<unsigned int> _indices = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35
	};
	
	unsigned int VBO, EBO;
	
	glGenVertexArrays(1u, &VAO);
	glGenBuffers(1u, &VBO);
	glGenBuffers(1u, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0u);
	glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1u);
	glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2u);
	glVertexAttribPointer(2u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	
	glBindVertexArray(0u);
	
	return true;
}

/**
* @ brief		update terrain renderer.
* @ details		this will be located in main loop. update whole terrain renderer like position, direction, status and etc...
*/
void EngineApp::updateScene(float dt)
{
	Profile();

	assetManager->listenToAssetChanges();
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ... 
*/
void EngineApp::drawScene(void) const
{
	Profile();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(Color::LightSteelBlue[0], Color::LightSteelBlue[1], Color::LightSteelBlue[2], Color::LightSteelBlue[3]);

	const float totalTime = timer.getTotalTime();

	simpleShader->useProgram();

	glm::mat4 model;
	model = glm::translate(glm::mat4(1.f), glm::vec3(0.f));
	model = glm::rotate(model, totalTime, glm::normalize(glm::vec3(0.f, 1.f, 1.f)));

	simpleShader->sendUniform("model", model);
	simpleShader->sendUniform("view", glm::lookAt(glm::vec3(0.f, 4.f, 6.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
	simpleShader->sendUniform("project", glm::perspective(glm::radians(45.0f), getAspectRatio(), 0.1f, 100.f));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36u, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0u);
	glBindTexture(GL_TEXTURE_2D, 0u);
}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
}

void EngineApp::scrollCallback(double xoffset, double yoffset)
{
}