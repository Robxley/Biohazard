#pragma once
#ifndef _BH3D_GL_H_
#define _BH3D_GL_H_

#if !defined(BH3D_IMPL_OPENGL_LOADER_GL3W) \
 && !defined(BH3D_IMPL_OPENGL_LOADER_GLEW) \
 && !defined(BH3D_IMPL_OPENGL_LOADER_GLAD) \
 && !defined(BH3D_IMPL_OPENGL_LOADER_CUSTOM)
    #if defined(__has_include)
        #if __has_include(<GL/glew.h>)
            #define BH3D_IMPL_OPENGL_LOADER_GLEW
        #elif __has_include(<glad/glad.h>)
            #define BH3D_IMPL_OPENGL_LOADER_GLAD
        #elif __has_include(<GL/gl3w.h>)
            #define BH3D_IMPL_OPENGL_LOADER_GL3W
        #else
            #error "Cannot detect OpenGL loader!"
        #endif
    #else
        #define BH3D_IMPL_OPENGL_LOADER_GL3W       // Default to GL3W
    #endif
#endif


#if defined(BH3D_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Needs to be initialized with gl3wInit() in user's code
#elif defined(BH3D_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Needs to be initialized with glewInit() in user's code
#elif defined(BH3D_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Needs to be initialized with gladLoadGL() in user's code
#else
#ifdef _WIN32
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif
#endif

#include <iostream>

inline bool glxxInit()
{
    try
    {
    #if defined(BH3D_IMPL_OPENGL_LOADER_GL3W)
        if(gl3wInit())
        {
            std::cout<<"Failed to initialize gl3w." << std::endl;
            return false;
        }
    #elif defined(BH3D_IMPL_OPENGL_LOADER_GLEW)
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            std::cout<<"Error: " << glewGetErrorString(err) << std::endl;
            return false;
        }
    #elif defined(BH3D_IMPL_OPENGL_LOADER_GLAD)
         if(!gladLoadGL())        {
            std::cout<<"Failed to initialize glad." << std::endl;
            return false;
        }
    #endif 
    }
    catch(...)
    {
        return false;
    }

    return true;
}


#endif