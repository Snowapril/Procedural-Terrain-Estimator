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

class GLShader : public EngineAsset
{
private:
	unsigned int programID;

	enum class CHECK_TARGET : int;

	static void getShaderString(const char* path, std::string& ret_string);
	static bool checkStatus(unsigned int target, CHECK_TARGET targetType);
public:
	GLShader();
	GLShader(const char* vs_path, const char* tcs_path, const char* tes_path, const char* gs_path, const char* fs_path);
	virtual ~GLShader();

	void setupShader(const char* vs_path, const char* tcs_path, const char* tes_path, const char* gs_path, const char* fs_path);

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