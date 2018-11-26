#include "GLShader.hpp"
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <iostream>
#include "obfuscator.hpp"

enum class GLShader::CHECK_TARGET : int
{
	SHADER = 0,
	PROGRAM = 1
};

GLShader::GLShader()
	: programID(0)
{
}

GLShader::GLShader(const char* vs_path, const char* fs_path)
	: programID(0)
{
	loadAsset(vs_path, fs_path);
}

GLShader::~GLShader()
{
	glDeleteProgram(programID);
}

/**
* @ brief		as method name says, just setup shader. 
* @ details		if setup is successfully finished, member variable "programID" will have complete shader context.
				params order obey opengl rendering pipeline.
* @ param		vertex shader file path. must not be nullptr.
* @ param		tessellation control shader file path. can be nullptr.
* @ param		tessellation evaluation shader file path. can be nullptr.
* @ param		geometry shader file path. can be nullptr.
* @ param		fragment shader file path. must not be nullptr.
*/
void GLShader::loadAsset(const char* vs_path, const char* fs_path)
{
	std::string vs_string, fs_string;

	getShaderString(vs_path, vs_string);
	getShaderString(fs_path, fs_string);

	GLuint vs, fs;

	const char* vs_source = vs_string.c_str(), *fs_source = fs_string.c_str();

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, nullptr);
	glCompileShader(vs);

	if (checkStatus(vs, CHECK_TARGET::SHADER))
		std::clog << OBFUSCATE("Vertex Shader [ ") << vs_path << OBFUSCATE(" ] Compile finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Vertex Shader [ ") << vs_path << OBFUSCATE(" ] Compile failed.") << std::endl;
		throw std::exception();
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, nullptr);
	glCompileShader(fs);

	if (checkStatus(fs, CHECK_TARGET::SHADER))
		std::clog << OBFUSCATE("Fragment Shader [ ") << fs_path << OBFUSCATE(" ] Compile finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Fragment Shader [ ") << fs_path << OBFUSCATE(" ] Compile failed.") << std::endl;
		throw std::exception();
	}

	programID = glCreateProgram();
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);

	if (checkStatus(programID, CHECK_TARGET::PROGRAM))
		std::clog << OBFUSCATE("Program Linking Finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Program Linking Failed") << std::endl;
		throw std::exception();
	}

	glDetachShader(programID, vs);
	glDeleteShader(vs);
	glDetachShader(programID, fs);
	glDeleteShader(fs);

	std::clog << OBFUSCATE("Create Shader Program Success") << std::endl;
}

void GLShader::loadRawAsset(const char* vs_source, const char* fs_source)
{
	GLuint vs, fs;

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, nullptr);
	glCompileShader(vs);

	if (checkStatus(vs, CHECK_TARGET::SHADER))
		std::clog << OBFUSCATE("Vertex Shader Compile finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Vertex Shader Compile failed.") << std::endl;
		throw std::exception();
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, nullptr);
	glCompileShader(fs);

	if (checkStatus(fs, CHECK_TARGET::SHADER))
		std::clog << OBFUSCATE("Fragment Shader Compile finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Fragment Shader Compile failed.") << std::endl;
		throw std::exception();
	}

	programID = glCreateProgram();
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);

	if (checkStatus(programID, CHECK_TARGET::PROGRAM))
		std::clog << OBFUSCATE("Program Linking Finished") << std::endl;
	else
	{
		std::cerr << OBFUSCATE("Program Linking Failed") << std::endl;
		throw std::exception();
	}

	glDetachShader(programID, vs);
	glDeleteShader(vs);
	glDetachShader(programID, fs);
	glDeleteShader(fs);

	std::clog << OBFUSCATE("Create Shader Program Success") << std::endl;
}

/**
* @ brief		get string from glsl shader file.
* @ param		shader file path which you want to get whole string.
* @ param		string reference, string from shader file will return to this reference.
*/
void GLShader::getShaderString(const std::string& path, std::string& ret_string)
{
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		shaderFile.open(path);
		std::ostringstream osstr;
		osstr << shaderFile.rdbuf();
		ret_string = osstr.str();
		shaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << OBFUSCATE("Ifstream failure : ") << e.what() << std::endl;
	}
}

/**
* @ brief		check status of target.
* @ details		if target is shader, check compile status, else if target is program then check link status.
				if status is "fail", get infolog from opengl and print it to log. and finally return false. else, return true;
* @ param		target which you want to check status.
* @ param		type of target. there are two types, SHADER and PROGRAM.
* @ return		if status is not "fail", return true. if not, return false;
*/
bool GLShader::checkStatus(unsigned int  target, CHECK_TARGET targetType)
{
	switch (targetType)
	{
		case CHECK_TARGET::SHADER :
		{
			int success;
			glGetShaderiv(target, GL_COMPILE_STATUS, &success);
			if (!success) {
				int infoLogLength;
				glGetShaderiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<char> infoLog(infoLogLength);
				glGetShaderInfoLog(target, infoLog.size(), nullptr, &infoLog[0]);
				std::cerr << OBFUSCATE("Shader Compile Failed. info log :") << std::endl;
				std::cerr << &infoLog[0] << std::endl;
				
				return false;
			}
			break;
		}
		case CHECK_TARGET::PROGRAM :
		{
			int success;
			glGetProgramiv(target, GL_LINK_STATUS, &success);
			if (!success) {
				int infoLogLength;
				glGetProgramiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<char> infoLog(infoLogLength);
				glGetProgramInfoLog(target, infoLog.size(), nullptr, &infoLog[0]);
				std::cerr << OBFUSCATE("Program Link Failed. info log : ") << std::endl;
				std::cerr << &infoLog[0] << std::endl;
				
				return false;
			}
			break;
		}
		default :
		{
			std::cerr << OBFUSCATE("Unknown target type is given") << std::endl;
			return false;
		}
	}

	return true;
}

/**
* @ brief		when you want to use shader context, this method must be called at first.
*/
void GLShader::useProgram(void) const
{
	glUseProgram(programID);
}

/**
* @ brief		location of uniform variable in shader context.
* @ return		return location of uniform variable in shader context.
				if program don't have uniform variable with given name, return -1.
*/
int GLShader::getUniformLocation(const std::string & varName) const
{
	int loc = glGetUniformLocation(programID, (const GLchar *)varName.c_str());

	return loc;
}

/**
* @ brief		send int variable to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log. 
*/
void GLShader::sendUniform(const std::string & varName, int i) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform1i(loc, i);
	}
}

/**
* @ brief		send float variable to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log.
*/
void GLShader::sendUniform(const std::string& varName, float f) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform1f(loc, f);
	}
}

/**
* @ brief		send bool variable to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log.
*/
void GLShader::sendUniform(const std::string & varName, bool b) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform1i(loc, b);
	}
}

/**
* @ brief		send vector variable with 2 elements to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
print critical log.
*/
void GLShader::sendUniform(const std::string& varName, const glm::vec2& vec) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform2fv(loc, 1, &vec[0]);
	}
}

/**
* @ brief		send vector variable with 3 elements to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log.
*/
void GLShader::sendUniform(const std::string & varName, const glm::vec3 & vec) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform3fv(loc, 1, &vec[0]);
	}
}

/**
* @ brief		send vector variable with 4 elements to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log.
*/
void GLShader::sendUniform(const std::string & varName, const glm::vec4 & vec) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniform4fv(loc, 1, &vec[0]);
	}
}

/**
* @ brief		send matrix variable with 4 dimension to uniform location with name "varName(param)".
* @ details		get uniform location with name "varName(param)" from getUniformLocation method. if given location(varName) is wrong,
				print critical log. note that how opengl graphics pipeline works. it take matrix and apply transpose to matrix. 
				if you want to use your own matrix from general linear algebra theory, keep this in mind.
*/
void GLShader::sendUniform(const std::string & varName, const glm::mat4 & mat) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		std::cerr << OBFUSCATE("Undefined Uniform Variable Name : ") << varName << std::endl;
	}
	else {
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}
