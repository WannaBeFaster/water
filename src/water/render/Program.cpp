#include "../../headers.h"

#include "../anim/Utils.h"
#include "../utils/Log.h"
#include "Program.h"

using namespace std;

//-------------------------------------------------------------------------------------------------
Program::~Program()
{
    glDeleteProgram(id_);
}

//-------------------------------------------------------------------------------------------------
// slightly refactored code taken from samples

void Program::load(const std::string& vertexPath, const std::string& fragmentPath)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
    PLOG_INFO << "Loading vertex shader: " << StreamColors(termcolor::cyan) << vertexPath;

    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertexPath, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
    }
    else
    {
        std::string txt = "Failed to open: " + vertexPath;
        throw std::runtime_error(txt);
	}

	// Read the Fragment Shader code from the file
    PLOG_INFO << "Loading fragment shader: " << StreamColors(termcolor::cyan) << fragmentPath;

    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragmentPath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
    else
    {
        std::string txt = "Failed to open: " + fragmentPath;
        throw std::runtime_error(txt);
    }

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);

        std::string txt = std::string("Failed to compile vertex shader: ") + std::string(VertexShaderErrorMessage.begin(), VertexShaderErrorMessage.end());
        throw std::runtime_error(txt);
	}

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);

        std::string txt = "Failed to compile fragment shader: " + std::string(FragmentShaderErrorMessage.begin(), FragmentShaderErrorMessage.end());
        throw std::runtime_error(txt);
	}

	// Link the program
    PLOG_INFO << "Linking program";

    id_ = glCreateProgram();
    glAttachShader(id_, VertexShaderID);
    glAttachShader(id_, FragmentShaderID);
    glLinkProgram(id_);

	// Check the program
    glGetProgramiv(id_, GL_LINK_STATUS, &Result);
    glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(id_, InfoLogLength, NULL, &ProgramErrorMessage[0]);

        std::string txt = "Failed to link the program: " + std::string(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
        throw std::runtime_error(txt);
	}
	
    glDetachShader(id_, VertexShaderID);
    glDetachShader(id_, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniform1i(const std::string& name, int val)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());
    int v = val;
    glUniform1iv(id, 1, &v);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniform1f(const std::string& name, float val)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());        
    glUniform1fv(id, 1, &val);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniformVec2(const std::string& name, const glm::vec2& val)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());
    glUniform2f(id, val.x, val.y);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniformVec3(const std::string& name, const glm::vec3& val)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());
    glUniform3f(id, val.x, val.y, val.z);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniformTexture(const std::string& name, GLuint tx, GLenum txLevel)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());
    glActiveTexture(txLevel);
    glBindTexture(GL_TEXTURE_2D, tx);
    glUniform1i(id, txLevel - GL_TEXTURE0);
}

//-------------------------------------------------------------------------------------------------
void Program::setUniformCubemapTexture(const std::string& name, GLuint tx, GLenum txLevel)
{
    GLuint id = glGetUniformLocation(id_, name.c_str());
    glActiveTexture(txLevel);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tx);
    glUniform1i(id, txLevel - GL_TEXTURE0);
}
