#include "BH3D_SphericalAxis.hpp"
#include "BH3D_TinyShader.hpp"

namespace bh3d
{

	void SphericalAxis::Init(float radius, int sectorCount)
	{

		assert(radius != 0.0f);
		BH3D_GL_CHECK_ERROR;

		m_vbo.Destroy();

		if (!m_shader.IsValid())
		{
			m_shader.LoadRaw(bh3d::TinyShader::COLOR_VERTEX(), bh3d::TinyShader::COLOR_FRAGMENT());
			assert(m_shader.IsValid());
		}

		constexpr float pi = glm::pi<float>();
		float sectorStep = 2 * pi / sectorCount;
		m_axis_elements = sectorCount;

		std::vector<glm::vec3> vSectors;
		vSectors.reserve(sectorCount * 3);  //x3 (for x, y ,z)

		float sectorAngle = 0;
		for (int i = 0; i < sectorCount; i++)
		{
			vSectors.emplace_back(glm::vec3{ radius * glm::cos(sectorAngle), radius * glm::sin(sectorAngle), 0.0f });
			sectorAngle += sectorStep;
		}

		for (int i = 0; i < sectorCount; i++) {
			auto& v = vSectors[i];
			vSectors.emplace_back(glm::vec3{ v.x, 0.0f, v.y });
		}

		for (int i = 0; i < sectorCount; i++) {
			auto& v = vSectors[i];
			vSectors.emplace_back(glm::vec3{ 0.0f, v.x, v.y });
		}

		//Color
		std::vector<glm::vec3> cvertex;
		cvertex.reserve(vSectors.size());
		cvertex.resize(sectorCount, glm::vec3{ 1.0f, 0.0f, 0.0f });
		cvertex.resize(sectorCount * 2, glm::vec3{ 0.0f, 1.0f, 0.0f });
		cvertex.resize(sectorCount * 3, glm::vec3{ 0.0f, 0.0f, 1.0f });

		assert(!vSectors.empty());
		assert(cvertex.size() == vSectors.size());

		//VBO 
		m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::POSITION, vSectors);
		m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::COLOR, cvertex);

		m_vbo.Create(GL_STATIC_DRAW);

		assert(m_vbo.IsValid());

	}

	void SphericalAxis::Draw(const glm::mat4& projection_modelview_transform) const
	{
		BH3D_GL_CHECK_ERROR;

		assert(m_shader.IsValid());
		assert(m_axis_elements > 0);

		m_shader.Enable();
		m_shader.SendProjectionModelviewTransform(projection_modelview_transform);

		assert(m_vbo.IsValid());
		m_vbo.Enable();

		int step = 0;
		glDrawArrays(GL_LINE_LOOP, 0, m_axis_elements);		//X axis
		step += m_axis_elements;

		glDrawArrays(GL_LINE_LOOP, step, m_axis_elements);	//Y axis
		step += m_axis_elements;

		glDrawArrays(GL_LINE_LOOP, step, m_axis_elements);	//Z axis

	}

}