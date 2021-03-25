#include "BH3D_TinyEngine.hpp"
#include "BH3D_GLCheckError.hpp"

namespace bh3d
{
	namespace 
	{
		//Static singleton trick to init glad (Thread safe)
		static bool BH3D_InitGL()
		{
			class InitGL {
				public:
					bool ok = false;
					InitGL() { 
						ok = glxxInit(); 
					}
					operator bool() { return ok; }
			};
			static InitGL initGL;
			assert(initGL == true);
			return initGL;
		}
	}

	void TinyEngine::InitOpenGL()
	{
		//Glad initialization
		if (!BH3D_InitGL())
		{
			BH3D_LOGGER_ERROR("gladLoadGL failed - Create the opengl context before calling gladLoadGL");
			return;
		}

#ifdef OPENGL_DEBUG
		assert((bool)glGetIntegerv && "A OpenGL context have to be created before using BH3D library");
		{
			bh3d::GLCheckError::InitGLDebugMsg();

			GLint major_version = 0;
			GLint minor_version = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major_version);
			glGetIntegerv(GL_MINOR_VERSION, &minor_version);

			auto & logGL = bh3d::GLCheckError::GetLogDebugGL();
			logGL << "OPENGL CONTEXT VERSION:" << std::endl;
			logGL << "\tGL_MAJOR_VERSION :" << major_version << std::endl;
			logGL << "\tGL_MINOR_VERSION :" << minor_version << std::endl << std::endl;
		}
#endif 

		BH3D_GL_CHECK_ERROR;

		//Don't draw the hidden object
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//Draw the "front" face only
		glEnable(GL_CULL_FACE);

		//Enable the blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);

		glEnable(GL_MULTISAMPLE);

	}


}  // Namespace bh3d