#include <vector>
#include <fstream>
#include "Shader.h"

Shader::Shader(std::string vertexShader, std::string fragmentShader, bool isDeffered):programID(0), isDeffered(isDeffered)
{
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertexShader, std::ios::in);
  if (VertexShaderStream.is_open()) {
    std::string Line;
    while (getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }
  else {
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertexShader.c_str());

  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragmentShader, std::ios::in);
  if (FragmentShaderStream.is_open()) {
    std::string Line;
    while (getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;


  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertexShader.c_str());
  char const* VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage((size_t)InfoLogLength + 1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragmentShader.c_str());
  char const* FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage((size_t)InfoLogLength + 1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
    printf("%s\n", FragmentShaderErrorMessage.data());
  }


  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();

  programID = ProgramID;
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage((size_t)InfoLogLength + 1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }


  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

}

void Shader::setBool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
  glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string& name, float value) const
{
  glUniform1f(glGetUniformLocation(programID, name.c_str()), (float)value);
}

void Shader::setVec2(const std::string& name, glm::vec2 value)
{
  glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value.x);
}

void Shader::setVec3(const std::string& name, glm::vec3 value)
{
  glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value.x);
}

void Shader::setVec4(const std::string& name, glm::vec4 value)
{
  glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value.x);
}


void Shader::UseShader()
{
  glUseProgram(programID);
}

