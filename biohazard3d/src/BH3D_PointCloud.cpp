#include "BH3D_PointCloud.hpp"
#include "BH3D_GL_GLM_core.hpp"

namespace bh3d
{


	void PointCloud::Create(const std::vector<glm::vec3>& vPositions, const std::vector<glm::vec4>& vColors, GLenum usage)
	{
		BH3D_GL_CHECK_ERROR;
		assert(vColors.capacity() == vPositions.capacity() && "The number of vertex have to be the same");

		//Destroy the existing point cloud
		Destroy();


		//Opengl call
		{
			//VAO setting
			glGenVertexArrays(1, &m_vertex_array);					//Creation VAO	
			assert(m_vertex_array > 0 && "VAO allocation KO");
			glBindVertexArray(m_vertex_array);

			//Opengl Buffer allocation
			m_position_buffer = AllocateVBO(vPositions, (GLuint)bh3d::ATTRIB_INDEX::POSITION, usage);		//Buffer allocation for Position vertex array
			assert(m_position_buffer > 0);

			m_color_buffer = AllocateVBO(vColors, (GLuint)bh3d::ATTRIB_INDEX::COLOR, usage);					//Buffer allocation for color vertex array
			assert(m_color_buffer > 0);
		}

		//Unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}



	void PointCloud::Create(const std::vector<float>& vPositions, const std::vector<float>& vColors, GLenum usage)
	{
		BH3D_GL_CHECK_ERROR;
		assert(vPositions.capacity() % 3 == 0);
		assert(vColors.capacity() % 4 == 0);
		assert((vPositions.capacity() / 3) == (vColors.capacity() / 4) && "The number of vertex have to be the same");
		
		//Destroy the existing point cloud
		Destroy();

		//Opengl call
		{
			//VAO setting
			glGenVertexArrays(1, &m_vertex_array);					//Creation VAO	
			assert(m_vertex_array > 0 && "VAO allocation KO");
			glBindVertexArray(m_vertex_array);

			//Opengl Buffer allocation
			m_position_buffer = AllocateVBO<3>(vPositions, (GLuint)bh3d::ATTRIB_INDEX::POSITION, usage);		//Buffer allocation for Position vertex array
			assert(m_position_buffer > 0);

			m_color_buffer = AllocateVBO<4>(vColors, (GLuint)bh3d::ATTRIB_INDEX::COLOR, usage);					//Buffer allocation for color vertex array
			assert(m_color_buffer > 0);
		}

		//Unbind
		glBindVertexArray(0);
		glDisableVertexAttribArray((GLuint)bh3d::ATTRIB_INDEX::POSITION);
		glDisableVertexAttribArray((GLuint)bh3d::ATTRIB_INDEX::COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void PointCloud::Create(std::size_t point_capacity, GLenum usage)
	{
		BH3D_GL_CHECK_ERROR;

		//Destroy the existing point cloud
		Destroy();

		//Opengl call
		{
			//VAO setting
			glGenVertexArrays(1, &m_vertex_array);					//Creation VAO	
			assert(m_vertex_array > 0 && "VAO allocation KO");
			glBindVertexArray(m_vertex_array);

			//Opengl Buffer allocation
			m_position_buffer = AllocateVBO<float, 3>(point_capacity, (GLuint)bh3d::ATTRIB_INDEX::POSITION, usage);		//Buffer allocation for Position vertex array
			assert(m_position_buffer > 0);

			m_color_buffer = AllocateVBO<float, 4>(point_capacity, (GLuint)bh3d::ATTRIB_INDEX::COLOR, usage);					//Buffer allocation for color vertex array
			assert(m_color_buffer > 0);
		}

		//Unbind
		glBindVertexArray(0);
		glDisableVertexAttribArray((GLuint)bh3d::ATTRIB_INDEX::POSITION);
		glDisableVertexAttribArray((GLuint)bh3d::ATTRIB_INDEX::COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}


	void PointCloud::Update(const std::vector<glm::vec3>& vPositions, const std::vector<glm::vec4>& vColors, GLenum usage)
	{
		assert(IsValid() && "Allocate the buffer before updating - Call Create function first");
		assert(vColors.size() == vPositions.size() && "The number of vertex have to be the same");

		BH3D_GL_CHECK_ERROR;

		// Update the buffers that OpenGL uses for rendering.

		UpdateVBO(m_position_buffer, vPositions, usage);
		UpdateVBO(m_color_buffer, vColors, usage);

		m_vertex_count = (GLsizei)vPositions.size();
	}

	void PointCloud::Update(const std::vector<float>& vPositions, const std::vector<float>& vColors, GLenum usage)
	{

		assert(IsValid() && "Allocate the buffer before updating - Call Create function first");
		assert(vPositions.size() % 3 == 0);
		assert(vColors.size() % 4 == 0);
		assert((vPositions.size() / 3) == (vColors.size() / 4) && "The number of vertex have to be the same");

		BH3D_GL_CHECK_ERROR;

		// Update the buffers that OpenGL uses for rendering.

		UpdateVBO(m_position_buffer, vPositions, usage);
		UpdateVBO(m_color_buffer, vColors, usage);

		m_vertex_count = (GLsizei)vPositions.size() / 3;
	}


	void PointCloud::Destroy()
	{
		BH3D_GL_CHECK_ERROR;

		//Make sure to unbind objects before deleting
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//Delete arrays and buffers
		glDeleteBuffers(1, &m_position_buffer);
		glDeleteBuffers(1, &m_color_buffer);
		glDeleteVertexArrays(1, &m_vertex_array);

		m_position_buffer = 0;
		m_color_buffer = 0;
		m_vertex_array = 0;
		m_vertex_count = 0;
	}

}