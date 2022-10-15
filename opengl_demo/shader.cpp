#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(std::string_view path) : m_shader_id{ 0 } {
	std::ifstream ifs{};
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		ifs.open(std::string{ path });
		std::stringstream ss{};
		ss << ifs.rdbuf();
		ifs.close();
		m_source = ss.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Error::Shader::File_Not_Successfully_Read" << std::endl;
	}
}

Shader::~Shader() {
	if (m_shader_id != 0) {
		glDeleteShader(m_shader_id);
	}
}

VertexShader::VertexShader(std::string_view path) : Shader{ path } {
	m_shader_id = glCreateShader(GL_VERTEX_SHADER);
	auto src_str = m_source.data();
	glShaderSource(m_shader_id, 1, &src_str, nullptr);
	glCompileShader(m_shader_id);

	int success{};
	char log[512];
	glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &success);
	if (success != 0) {
		glGetShaderInfoLog(m_shader_id, 512, nullptr, log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
	}
}

FragmentShader::FragmentShader(std::string_view path) : Shader{ path } {
	m_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	auto src_str = m_source.data();
	glShaderSource(m_shader_id, 1, &src_str, nullptr);
	glCompileShader(m_shader_id);

	int success{};
	char log[512];
	glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &success);
	if (success != 0) {
		glGetShaderInfoLog(m_shader_id, 512, nullptr, log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
	}
}


ShaderProgram::ShaderProgram(std::string_view vertex_shader_path, std::string_view fragment_shader_path) : m_program_id{ 0 } {
	VertexShader vertexShader{ vertex_shader_path };
	FragmentShader fragmentShader{ fragment_shader_path };
	m_program_id = glCreateProgram();
	glAttachShader(m_program_id, vertexShader.getId());
	glAttachShader(m_program_id, fragmentShader.getId());
	glLinkProgram(m_program_id);

	int success{};
	char log[512];
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
	if (success != 0) {
		glGetProgramInfoLog(m_program_id, 512, nullptr, log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
	}
}

ShaderProgram::~ShaderProgram() {
	if (m_program_id != 0) {
		glDeleteProgram(m_program_id);
	}
}

void ShaderProgram::use() const noexcept {
	glUseProgram(m_program_id);
}

void ShaderProgram::set_uniform(std::string_view name, bool value) const noexcept {
	glUniform1i(glGetUniformLocation(m_program_id, name.data()), static_cast<int>(value));
}

void ShaderProgram::set_uniform(std::string_view name, int value) const noexcept {
	glUniform1i(glGetUniformLocation(m_program_id, name.data()), value);
}

void ShaderProgram::set_uniform(std::string_view name, float value) const noexcept {
	glUniform1f(glGetUniformLocation(m_program_id, name.data()), value);
}