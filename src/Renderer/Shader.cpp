#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string>

Shader::~Shader()
{
	if(m_id != 0)
		Delete();
}

void Shader::Delete()
{
	glDeleteProgram(m_id);
  m_id = 0;
}

void Shader::Use()
{
	glUseProgram(m_id);
}

bool Shader::CreateFromMemory(std::string& src)
{
  constexpr std::string_view DirectiveVertex = "#shader vertex\n";
  constexpr std::string_view DirectiveFragment = "#shader fragment\n";

  size_t vertexStart = src.find(DirectiveVertex);
  size_t fragmentStart = src.find(DirectiveFragment);
  if (vertexStart == std::string::npos ||
    fragmentStart == std::string::npos)
  {
    LOG_ERROR("Cannot parse shader");
    return true;
  }

  GLuint vertex = 0;
  GLuint fragment = 0;
  if (fragmentStart > vertexStart)
  {
    vertexStart += DirectiveVertex.size();
    vertex = LoadShader(src.c_str() + vertexStart, fragmentStart - vertexStart, GL_VERTEX_SHADER);
    fragmentStart += DirectiveFragment.size();
    fragment = LoadShader(src.c_str() + fragmentStart, src.size() - fragmentStart, GL_FRAGMENT_SHADER);
  }
  else
  {
    fragmentStart += DirectiveFragment.size();
    fragment = LoadShader(src.c_str() + fragmentStart, vertexStart - fragmentStart, GL_FRAGMENT_SHADER);
    vertexStart += DirectiveVertex.size();
    vertex = LoadShader(src.c_str() + vertexStart, src.size() - vertexStart, GL_VERTEX_SHADER);
  }

  SetID(LinkShaders(vertex, fragment));

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return GetID() == 0;
}

bool Shader::Create(const std::string& path)
{
  try
  {
    std::string src = ReadFile(path);
    return CreateFromMemory(src);
  }
  catch (const std::exception& err)
  {
    LOG_ERROR("Cannot create shader: {}", err.what());
    return true;
  }
}

bool Shader::CreateFromMemory(std::string& srcVertex, std::string& srcFragment)
{
  ProfileScope;

  if (m_id != 0)
    glDeleteProgram(m_id);

	GLuint vertex   = LoadShader(srcVertex.c_str(), -1, GL_VERTEX_SHADER);
	GLuint fragment = LoadShader(srcFragment.c_str(), -1, GL_FRAGMENT_SHADER);

	SetID(LinkShaders(vertex, fragment));

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return GetID() == 0;
}

bool Shader::Create(const std::string& pathVertex, const std::string& pathFragment)
{
  ProfileScope;

	try
	{
		std::string vert = ReadFile(pathVertex);
		std::string frag = ReadFile(pathFragment);
		return CreateFromMemory(vert, frag);
	}
	catch (const std::exception& err)
	{
    LOG_ERROR("Cannot create shader: {}", err.what());
		return true;
	}
}

#ifdef CELLNTA_HAS_GEOMETRY_SHADER
bool Shader::CreateFromMemory(std::string& srcVertex, std::string& srcFragment, std::string& srcGeometry)
{
  ProfileScope;

  if (m_id != 0)
    glDeleteProgram(m_id);

	GLuint vertex   = LoadShader(srcVertex.c_str(), -1, GL_VERTEX_SHADER);
	GLuint fragment = LoadShader(srcFragment.c_str(), -1, GL_FRAGMENT_SHADER);
	GLuint geometry = LoadShader(srcGeometry.c_str(), -1, GL_GEOMETRY_SHADER);

  SetID(LinkShaders(vertex, fragment, geometry));

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
	return GetID() == 0;
}

bool Shader::Create(const std::string& pathVertex, const std::string& pathFragment, const std::string& pathGeometry)
{
  ProfileScope;

	try
	{
		std::string vert = ReadFile(pathVertex);
		std::string frag = ReadFile(pathFragment);
		std::string geom = ReadFile(pathGeometry);
		return CreateFromMemory(vert, frag, geom);
	}
	catch (const std::exception& err)
	{
    LOG_ERROR("Cannot create shader: {}", err.what());
		return true;
	}
}
#endif //CELLNTA_RENDERER_GLES3

GLuint Shader::LoadShader(const char* src, GLint length, GLenum type)
{
  ProfileScope;

  std::vector<const char*> data;
  std::vector<GLint> dataLength;

  //check first line is not a "#version ..."
  if (src[0] == '#')
  {
    if (std::strncmp(&src[1], "version", 7) != 0)
    {
      data.push_back(m_version.c_str());
      dataLength.push_back(m_version.size());
    }
  }

  data.push_back(src);
  dataLength.push_back(length);
  assert(data.size() == dataLength.size() && "Size must be equal");

  GLuint shader = glCreateShader(type);
	glShaderSource(shader, data.size(), data.data(), dataLength.data());
	glCompileShader(shader);

	std::string errorData;
  if (GetShaderErrorStatus(shader, errorData))
    LOG_ERROR("Error compiling \"{}\" shader: {}{}",
        GetShaderTypeString(type), m_version, errorData);

	return shader;
}

GLuint Shader::LinkShaders(GLuint vertex, GLuint fragment, GLuint geometry)
{
  ProfileScope;

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  if (geometry != 0)
    glAttachShader(program, geometry);
  glLinkProgram(program);

  std::string errorData;
  if (GetLinkStatusError(program, errorData))
  {
    LOG_ERROR("Error linking shaders:\n: {}", errorData);
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

bool Shader::GetLinkStatusError(GLuint program, std::string& out)
{
  ProfileScope;

	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status)
	{
		GLint size = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);

		out.resize(size);
		glGetProgramInfoLog(program, size, NULL, out.data());
		return 1;
	}
	return 0;
}

bool Shader::GetShaderErrorStatus(GLuint shader, std::string& out)
{
  ProfileScope;

	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		GLint size = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

		out.resize(size);
		glGetShaderInfoLog(shader, size, NULL, out.data());
		return true;
	}
	return false;
}

const char* Shader::GetShaderTypeString(GLenum type) const
{
  switch (type)
  {
  case GL_VERTEX_SHADER:   return "Vertex";
  case GL_FRAGMENT_SHADER: return "Fragment";
#ifdef CELLNTA_HAS_GEOMETRY_SHADER
  case GL_GEOMETRY_SHADER: return "Geometry";
#endif
  }
  return "Unknown";
}

std::string Shader::ReadFile(const std::string& path)
{
  ProfileScope;

	std::string out;
	std::ifstream file;

	LOG_INFO("Reading shader: {}", path);
	file.open(path);
  if (!file.is_open())
  {
    LOG_ERROR("Cannot create shader: {}", path);
    return "";
  }

	std::getline(file, out, '\0');
	file.close();
	return out;
}

void Shader::SetID(GLuint id)
{
	m_id = id;
}

GLuint Shader::GetID()
{
	return m_id;
}

GLint Shader::GetUniformLocation(const std::string& location)
{
  ProfileScope;

	auto res = m_uniforms.find(location);
	if (res != m_uniforms.end())
		return res->second;
	GLint loc = glGetUniformLocation(m_id, location.c_str());
	if (loc != -1)
		m_uniforms[location] = loc;
	return loc;
}

GLint Shader::GetAttribLocation(const std::string& Location)
{
	return glGetAttribLocation(m_id, Location.c_str());
}

void Shader::Set(const std::string& location, const GLfloat value)
{
	glUniform1f(GetUniformLocation(location), value);
}

void Shader::Set(const std::string& location, const GLint value)
{
	glUniform1i(GetUniformLocation(location), value);
}


void Shader::Set(const std::string& location, const Eigen::Vector2i& value)
{
	glUniform2iv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector3i& value)
{
	glUniform3iv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector4i& value)
{
	glUniform4iv(GetUniformLocation(location), 1, value.data());
}


void Shader::Set(const std::string& location, const Eigen::Vector2f& value)
{
	glUniform2fv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector3f& value)
{
	glUniform3fv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector4f& value)
{
	glUniform4fv(GetUniformLocation(location), 1, value.data());
}

#ifdef CELLNTA_RENDERER_GLES3
void Shader::Set(const std::string& location, const Eigen::Vector2d& value)
{
	const Eigen::Vector2f& tmp = value.cast<float>();
	glUniform2fv(GetUniformLocation(location), 1, tmp.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector3d& value)
{
	const Eigen::Vector3f& tmp = value.cast<float>();
	glUniform3fv(GetUniformLocation(location), 1, tmp.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector4d& value)
{
	const Eigen::Vector4f& tmp = value.cast<float>();
	glUniform4fv(GetUniformLocation(location), 1, tmp.data());
}

#else
void Shader::Set(const std::string& location, const Eigen::Vector2d& value)
{
	glUniform2dv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector3d& value)
{
	glUniform3dv(GetUniformLocation(location), 1, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Vector4d& value)
{
	glUniform4dv(GetUniformLocation(location), 1, value.data());
}

#endif // CELLNTA_RENDERER_GLES3

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 2, 2>& value)
{
	glUniformMatrix2fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 2, 3>& value)
{
	glUniformMatrix2x3fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 2, 4>& value)
{
	glUniformMatrix2x4fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}


void Shader::Set(const std::string& location, const Eigen::Matrix<float, 3, 3>& value)
{
	glUniformMatrix3fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 3, 2>& value)
{
	glUniformMatrix3x2fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 3, 4>& value)
{
	glUniformMatrix3x4fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}


void Shader::Set(const std::string& location, const Eigen::Matrix<float, 4, 4>& value)
{
	glUniformMatrix4fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 4, 2>& value)
{
	glUniformMatrix4x2fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

void Shader::Set(const std::string& location, const Eigen::Matrix<float, 4, 3>& value)
{
	glUniformMatrix4x3fv(GetUniformLocation(location), 1, GL_FALSE, value.data());
}

