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
	: GLApp(), terrain(), camera()
{
}

EngineApp::~EngineApp()
{
}

/**
* @ brief		update terrain renderer.
* @ details		this will be located in main loop. update whole terrain renderer like position, direction, status and etc...
*/
void EngineApp::updateScene(float dt)
{
	Profile();

	camera.processKeyInput();
	camera.sendVP(vpUBO, GLApp::getAspectRatio());

	const glm::vec3 cameraPos = camera.getViewPos();

	terrain.updateScene(dt);
	terrain.buildNonUniformPatch(cameraPos, glm::vec3(0.0f));

	assetManager->refreshDirtyAssets();
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ...
*/
void EngineApp::drawScene(void) const
{
	Profile();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(Color::DarkBlue[0], Color::DarkBlue[1], Color::DarkBlue[2], Color::DarkBlue[3]);

	const float totalTime = timer.getTotalTime();

	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	terrain.drawTerrain(GL_PATCHES);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	simpleShader->useProgram();

	glm::mat4 model;
	model = glm::translate(glm::mat4(1.f), glm::vec3(0.f));
	model = glm::rotate(model, totalTime, glm::normalize(glm::vec3(0.f, 1.f, 1.f)));

	simpleShader->sendUniform("model", model);

	glBindVertexArray(VAO);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 36u, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0u);
	glBindTexture(GL_TEXTURE_2D, 0u);
	glBindBuffer(GL_UNIFORM_BUFFER, 0u);
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

	if (!initUniformBufferObject())
		return false;

	if (!terrain.initWithLocalFile(GLApp::getAspectRatio(), {}))
		return false;

	return true;
}

bool EngineApp::initUniformBufferObject(void)
{
	glGenBuffers(1, &vpUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, vpUBO, 0, sizeof(glm::mat4) * 2);

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

	if (!initTextures())
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
		simpleShader = assetManager->addAsset<GLShader>({ 
			"../resources/shader/simple_vs.glsl",
			"../resources/shader/simple_fs.glsl" 
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("An error occurred while initializing shader file.\n{}", e.what());
		return false;
	}

	return true;
}

/**

*/
bool EngineApp::initTextures(void)
{
	return true;
}

/**
* @ brief		initialize geometry buffer in current opengl context.
* @ return		return boolean whether if initialize geometry buffer is successful or not.
*/
bool EngineApp::initGeometryBuffer(void)
{
	/// below hard coded stuffs are for testing.
	/// will be replaced to terrain geometry setup.

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
		0u, 1u, 2u,
		3u, 4u, 5u,
		6u, 7u, 8u,
		9u, 10u, 11u,
		12u, 13u, 14u,
		15u, 16u, 17u,
		18u, 19u, 20u,
		21u, 22u, 23u,
		24u, 25u, 26u,
		27u, 28u, 29u,
		30u, 31u, 32u,
		33u, 34u, 35u
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

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
	camera.processMousePos(xpos, ypos);
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
	camera.processMousePos(btn, action, mods);
}

void EngineApp::scrollCallback(double xoffset, double yoffset)
{
	camera.processScroll(yoffset);
}