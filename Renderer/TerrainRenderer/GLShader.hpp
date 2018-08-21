/**
* @file		GLShader.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	OpenGL GLSL Shader.
* @details	Initialize shader with given GLSL files' path. user can send uniform from CPU to GPU with this class.
* @see
*/

#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include "EngineAsset.hpp"
#include <vector>
#include <array>

class GLShader : public EngineAsset
{
private:
	unsigned int programID;
	
	enum class CHECK_TARGET : int;
	enum SHADER_TYPE {
		VS  = 0,
		TCS = 1,
		TES = 2,
		GS  = 3,
		FS  = 4,
	};

	std::array<std::string, 5> shaderFileName;
	std::string directoryPath;
	std::string programName;

	void parseShaderPath(const std::vector<std::string>& assetPath);
	static void getShaderString(const std::string& path, std::string& ret_string);
	static bool checkStatus(unsigned int target, CHECK_TARGET targetType);
public:
	GLShader();
	GLShader(const std::vector<std::string>& assetPath);
	virtual ~GLShader();

	void loadAsset(const std::vector<std::string>& assetPath);
	void reloadAsset(void);

	void useProgram(void) const;
	int getUniformLocation(const std::string& varName) const;

	void sendUniform(const std::string& varName, int i) const;
	void sendUniform(const std::string& varName, float f) const;
	void sendUniform(const std::string& varName, bool b) const;
	void sendUniform(const std::string& varName, const glm::vec3& vec) const;
	void sendUniform(const std::string& varName, const glm::vec4& vec) const;
	void sendUniform(const std::string& varName, const glm::mat4& mat) const;
};


#endif