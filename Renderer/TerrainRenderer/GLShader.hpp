/**
* @file		GLShader.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	OpenGL GLSL Shader.
* @details	Initialize shader with given GLSL files' path. user can send uniform from CPU to GPU with this class.
* @see
*/

#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include "EngineAsset.hpp"

class GLShader : public EngineAsset
{
private:
	uint32_t programID;
	
	enum class CHECK_TARGET : int;
	enum SHADER_TYPE {
		VS  = 0,
		TCS = 1,
		TES = 2,
		GS  = 3,
		FS  = 4,
		CS  = 5,
	};

	void parseShaderPath(const std::vector<std::string>& assetPath);
	static void getShaderString(const std::string& path, std::string& ret_string);
	static bool checkStatus(uint32_t target, CHECK_TARGET targetType);
public:
	GLShader();
	GLShader(const std::vector<std::string>& assetPath);
	GLShader(const GLShader& other);
	GLShader& operator=(const GLShader& other);
	virtual ~GLShader();
public:
	void loadAsset(const std::vector<std::string>& assetPath);
	virtual void reloadAsset(void);

	void useProgram(void) const;
	int getUniformLocation(const std::string& varName) const;

	void sendUniform(const std::string& varName, int i) const;
	void sendUniform(const std::string& varName, float f) const;
	void sendUniform(const std::string& varName, bool b) const;
	void sendUniform(const std::string& varName, const glm::vec2& vec) const;
	void sendUniform(const std::string& varName, const glm::vec3& vec) const;
	void sendUniform(const std::string& varName, const glm::vec4& vec) const;
	void sendUniform(const std::string& varName, const glm::mat4& mat) const;
};


#endif