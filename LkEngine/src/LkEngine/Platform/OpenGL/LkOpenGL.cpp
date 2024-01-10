#include "LKpch.h"
#include "LkOpenGL.h"


namespace LkEngine {

	void OpenGL_ClearError() 
	{ 
		while (glGetError() != GL_NO_ERROR); 
	}

    bool OpenGL_LogCall(const char* function, const char* file, int line)
    {
        while (GLenum error = glGetError())
        {
            std::cout << "[OpenGL Error] (" << error << ")\nFUNCTION: " << function <<
                "\nFILE: " << file << "\nLINE: " << line << std::endl;
            return false;
        }
        return true;
    }

	const char* OpenGL_GetVersion()
	{
		char buf[120];
		sprintf(buf, "%s", glGetString(GL_VERSION));
		return buf;
	}

}