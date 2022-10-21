#pragma once
#include <glad/glad.h>

#include <string>
#include <string_view>

// for glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Shader {
	explicit Shader(std::string_view sv);
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) noexcept = delete;
	Shader& operator=(Shader&&) noexcept = delete;
	inline auto getId() const noexcept -> unsigned int { return m_shader_id; }
protected:
	unsigned int m_shader_id;
	std::string m_source;
};

struct VertexShader : public Shader {
	explicit VertexShader(std::string_view path);
};

struct FragmentShader : public Shader {
	explicit FragmentShader(std::string_view path);
};


struct ShaderProgram {
	explicit ShaderProgram(std::string_view vertex_shader_path, std::string_view fragment_shader_path);
	~ShaderProgram();

	inline unsigned int getId() const noexcept;

	void use() const noexcept;

	// uniform helper function
	void set_uniform(std::string_view name, bool value) const noexcept;
	void set_uniform(std::string_view name, int value) const noexcept;
	void set_uniform(std::string_view name, float value) const noexcept;
	void set_uniform(std::string_view name, glm::mat4& value) const noexcept;
	// todo: glm::mat4&/glm::mat3&/glm::mat2&
private:
	unsigned int m_program_id;
};

