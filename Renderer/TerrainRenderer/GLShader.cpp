#include "GLShader.hpp"
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

enum class GLShader::CHECK_TARGET : int
{
	SHADER = 0,
	PROGRAM = 1
};

GLShader::GLShader()
	: programID(0), directoryPath(), programName()
{
}

GLShader::GLShader(const std::vector<std::string>& assetPath)
	: programID(0), directoryPath(), programName()
{
	loadAsset(assetPath);
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
void GLShader::loadAsset(const std::vector<std::string>& assetPath)
{
	parseShaderPath(assetPath);

	std::string vs_string, tcs_string, tes_string, gs_string, fs_string;

	getShaderString(directoryPath + std::string(shaderFileName[VS]) , vs_string);
	
	if (!shaderFileName[TCS].empty())
		getShaderString(directoryPath + std::string(shaderFileName[TCS]), tcs_string);
	if (!shaderFileName[TES].empty())
		getShaderString(directoryPath + std::string(shaderFileName[TES]), tes_string);
	if (!shaderFileName[GS].empty())
		getShaderString(directoryPath + std::string(shaderFileName[GS]), gs_string);
	
	getShaderString(directoryPath + std::string(shaderFileName[FS]), fs_string);

	GLuint vs, tcs, tes, gs, fs;

	const char* vs_source = vs_string.c_str(), *fs_source = fs_string.c_str();
	const char *tcs_source = nullptr, *tes_source = nullptr, *gs_source = nullptr;

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, nullptr);
	glCompileShader(vs);

	if (checkStatus(vs, CHECK_TARGET::SHADER))
		EngineLogger::getConsole()->info("Vertex Shader [{}] Compile finished.", shaderFileName[VS]);
	else
	{
		EngineLogger::getConsole()->error("Vertex Shader [{}] Compile failed.", shaderFileName[VS]);
		throw std::exception();
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, nullptr);
	glCompileShader(fs);

	if (checkStatus(fs, CHECK_TARGET::SHADER))
		EngineLogger::getConsole()->info("Fragment Shader [{}] Compile finished.", shaderFileName[FS]);
	else
	{
		EngineLogger::getConsole()->error("Fragment Shader [{}] Compile failed.", shaderFileName[FS]);
		throw std::exception();
	}

	if (!shaderFileName[TCS].empty()) {
		tcs_source = tcs_string.c_str();
		tcs = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(tcs, 1, &tcs_source, nullptr);
		glCompileShader(tcs);

		if (checkStatus(tcs, CHECK_TARGET::SHADER))
			EngineLogger::getConsole()->info("Tessellation Control Shader [{}] Compile finished.", shaderFileName[TCS]);
		else
		{
			EngineLogger::getConsole()->error("Tessellation Control Shader [{}] Compile failed.", shaderFileName[TCS]);
			throw std::exception();
		}
	}

	if (!shaderFileName[TES].empty()) {
		tes_source = gs_string.c_str();
		tes = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(tes, 1, &tes_source, nullptr);
		glCompileShader(tes);

		if (checkStatus(tes, CHECK_TARGET::SHADER))
			EngineLogger::getConsole()->info("Tessellation Evaluation Shader [{}] Compile finished.", shaderFileName[TES]);
		else
		{
			EngineLogger::getConsole()->error("Tessellation Evaluation Shader [{}] Compile failed.", shaderFileName[TES]);
			throw std::exception();
		}
	}

	if (!shaderFileName[GS].empty()) {
		gs_source = gs_string.c_str();
		gs = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gs, 1, &gs_source, nullptr);
		glCompileShader(gs);

		if (checkStatus(gs, CHECK_TARGET::SHADER))
			EngineLogger::getConsole()->info("Geometry Shader [{}] Compile finished.", shaderFileName[GS]);
		else
		{
			EngineLogger::getConsole()->error("Geometry Shader [{}] Compile failed.", shaderFileName[GS]);
			throw std::exception();
		}
	}

	programID = glCreateProgram();
	glAttachShader(programID, vs);
	if (!shaderFileName[TCS].empty())
		glAttachShader(programID, tcs);
	if (!shaderFileName[TES].empty())
		glAttachShader(programID, tes);
	if (!shaderFileName[GS].empty())
		glAttachShader(programID, gs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);

	if (checkStatus(programID, CHECK_TARGET::PROGRAM))
		EngineLogger::getConsole()->info("Program Linking finished.");
	else
	{
		EngineLogger::getConsole()->error("Program Linking Failed");
		throw std::exception();
	}

	glDetachShader(programID, vs);
	glDeleteShader(vs);
	if (!shaderFileName[TCS].empty())
	{
		glDetachShader(programID, tcs);
		glDeleteShader(tcs);
	}
	if (!shaderFileName[TES].empty())
	{
		glDetachShader(programID, tes);
		glDeleteShader(tes);
	}
	if (!shaderFileName[GS].empty())
	{
		glDetachShader(programID, gs);
		glDeleteShader(gs);
	}
	glDetachShader(programID, fs);
	glDeleteShader(fs);

	EngineLogger::getConsole()->info("Linking Program Success.");
}

/**
* @ brief		parse given asset paths.
* @ details		parse given asset paths to directory path, shader program name, shader type. note that shader file name (without extension) must be composed with 
				shader program name and shader type(vs, tcs, tes, gs, fs) separated by delimiter "_". 
* @ param		initializer list which filled with shader files' path.
* @ todo		this method may be very unsafe and low performance i think. maybe there are better way to parse.
*/
void GLShader::parseShaderPath(const std::vector<std::string>& assetPath)
{
	for (const auto& path : assetPath)
	{
		const auto fileNameIdx = path.find_last_of("/"); //before fileNameIdx is directory path.

		if (directoryPath.empty())
			directoryPath = path.substr(0, fileNameIdx + 1);

		const std::string fileName = path.substr(fileNameIdx + 1);

		const auto shaderTypeIdx = fileName.find_last_of("_");

		if (programName.empty())
			programName = fileName.substr(0, shaderTypeIdx);

		const std::string typeAndExtension = fileName.substr(shaderTypeIdx + 1);

		const auto extensionIdx = typeAndExtension.find_last_of(".");

		const std::string shaderType = typeAndExtension.substr(0, extensionIdx);

		if (shaderType == "vs")
			shaderFileName[VS] = fileName;
		else if (shaderType == "tcs")
			shaderFileName[TCS] = fileName;
		else if (shaderType == "tes")
			shaderFileName[TES] = fileName;
		else if (shaderType == "gs")
			shaderFileName[GS] = fileName;
		else if (shaderType == "fs")
			shaderFileName[FS] = fileName;
		else
		{
			EngineLogger::getConsole()->critical("Unknown shader type file is given : {}", path);
			throw std::exception();
		}
	}
}

/**
* @ brief		reload shader program.
* @ details		reload shader context when it need to be refreshed (ex : original file changed).
*/
void GLShader::reloadAsset(void)
{
	glDeleteProgram(programID);

	loadAsset({}); //with empty initializer list, paths remain unchanged.
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
		EngineLogger::getConsole()->critical("ifstream Failure : {:<20}", e.what());
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
				glGetShaderInfoLog(target, sizeof(infoLog), nullptr, &infoLog[0]);
				EngineLogger::getConsole()->critical("Shader Compile Failed. info log :\n{}", &infoLog[0]);
				throw std::exception();
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
				glGetProgramInfoLog(target, sizeof(infoLog), nullptr, &infoLog[0]);
				EngineLogger::getConsole()->critical("Program Link Failed. info log :\n{}", &infoLog[0]);
				return false;
			}
			break;
		}
		default :
		{
			EngineLogger::getConsole()->critical("Unknown target type is given.");
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
	}
	else {
		glUniform1i(loc, b);
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
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
		EngineLogger::getConsole()->critical("Undefined Uniform Variable Name : {}", varName);
	}
	else {
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}
