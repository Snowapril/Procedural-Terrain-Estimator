#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		std::cout << "Usage : [vertex shader file path] [fragment shader file path]" << std::endl;
		std::cout << "Allowed options : " << std::endl;
		std::cout << "	-o [name]	output cache file name" << std::endl;
		std::cout << "	-t [TCS TES]	tessellation shader file path" << std::endl;
		std::cout << "	-g [GS]		geometry shader file path" << std::endl;
		return 0;
	}

	std::string vertPath, fragPath, tcsPath, tesPath, gsPath;
	std::string outputName = "a.glslcache";
	bool useTessellation(false);
	bool useGeometryShader(false);

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'o' :
				outputName = argv[++i];
				break;
			case 't' :
				useTessellation = true;
				tcsPath = argv[++i];
				tesPath = argv[++i];
				break;
			case 'g' :
				useGeometryShader = true;
				gsPath = argv[++i];
				break;
			default:
				std::cerr << "given option not exists" << std::endl;
				break;
			}
		}
		else
		{
			if (vertPath.empty())
				vertPath = argv[i];
			else if (fragPath.empty())
				fragPath = argv[i];
		}
	}

	std::string vsString, fsString, tesString, tcsString, gsString;
	{
		std::ifstream vsFile, fsFile, tesFile, tcsFile, gsFile;
		vsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		tesFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		tcsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		gsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		try
		{
			{
				vsFile.open(vertPath);
				std::ostringstream osstr;
				osstr << vsFile.rdbuf();
				vsString = osstr.str();
			}

			{
				fsFile.open(fragPath);
				std::ostringstream osstr;
				osstr << fsFile.rdbuf();
				fsString = osstr.str();
			}

			if (useTessellation)
			{
				{
					tesFile.open(tesPath);
					std::ostringstream osstr;
					osstr << tesFile.rdbuf();
					tesString = osstr.str();
				}
				{
					tcsFile.open(tcsPath);
					std::ostringstream osstr;
					osstr << tcsFile.rdbuf();
					tcsString = osstr.str();
				}
			}

			if (useGeometryShader)
			{
				gsFile.open(gsPath);
				std::ostringstream osstr;
				osstr << gsFile.rdbuf();
				gsString = osstr.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cerr << e.what() << std::endl;
			return -1;
		}
	}

	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_FORWARE_COMPAT);
#endif

	GLFWwindow* window = glfwCreateWindow(1, 1, "empty", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		std::cerr << "GLAD initialization failed" << std::endl;
		return -1;
	}

	const char* vsSource = vsString.c_str();
	const char* fsSource = fsString.c_str();

	GLuint vs, fs, tcs, tes, gs;

	int success;
	char infoLog[512];

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSource, nullptr);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, sizeof(infoLog), nullptr, &infoLog[0]);
		std::clog << "VERTEX SHADER INFOLOG" << std::endl;
		std::clog << infoLog << std::endl;
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSource, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, sizeof(infoLog), nullptr, &infoLog[0]);
		std::clog << "FRAGMENT SHADER INFOLOG" << std::endl;
		std::clog << infoLog << std::endl;
	}

	if (useTessellation)
	{
		const char* tcsSource = tcsString.c_str();
		const char* tesSource = tesString.c_str();

		tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcs, 1, &tcsSource, nullptr);
		glCompileShader(tcs);

		glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tcs, sizeof(infoLog), nullptr, &infoLog[0]);
			std::clog << "TESSELLATION CONTROL SHADER INFOLOG" << std::endl;
			std::clog << infoLog << std::endl;
		}

		tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tes, 1, &tesSource, nullptr);
		glCompileShader(tes);

		glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tes, sizeof(infoLog), nullptr, &infoLog[0]);
			std::clog << "TESSELLATION EVALUATION SHADER INFOLOG" << std::endl;
			std::clog << infoLog << std::endl;
		}
	}

	if (useGeometryShader)
	{
		const char* gsSource = gsString.c_str();

		gs = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(gs, 1, &gsSource, nullptr);
		glCompileShader(gs);

		glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(gs, sizeof(infoLog), nullptr, &infoLog[0]);
			std::clog << "GEOMETRY SHADER INFOLOG" << std::endl;
			std::clog << infoLog << std::endl;
		}
	}

	GLuint programID = glCreateProgram();
	
	glAttachShader(programID, vs);
	if (useTessellation)
	{
		glAttachShader(programID, tcs);
		glAttachShader(programID, tes);
	}
	if (useGeometryShader)
	{
		glAttachShader(programID, gs);
	}

	glAttachShader(programID, fs);

	glProgramParameteri(programID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, &infoLog[0]);
		std::clog << "SHADER PROGRAM LINK INFOLOG" << std::endl;
		std::clog << infoLog << std::endl;
	}

	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);
	GLint len = 0;
	glGetProgramiv(programID, GL_PROGRAM_BINARY_LENGTH, &len);
	char* binary = new char[len];
	glGetProgramBinary(programID, len, NULL, (GLenum*)binaryFormats, binary);
	std::ofstream outFile(outputName, std::ios::out | std::ios::binary);
	outFile.write((char*)(&len), sizeof(len));
	outFile.write(binary, len);
	outFile.close();

	delete[] binary;

	return 0;
}