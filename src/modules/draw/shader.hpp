#ifndef SHADER_H
#define SHADER_H
#include <glad.h>
#include <string>

#define SHADER_MAXIMUM_LOG_SIZE 4096
class Shader {
public:
    enum class Type {
        Basic,
        Count
    };

private:
    GLuint m_handle;
    Shader(GLuint handle);
    static char s_vertexLogBuffer[SHADER_MAXIMUM_LOG_SIZE];
    static char s_fragmentLogBuffer[SHADER_MAXIMUM_LOG_SIZE];

public:
    Shader Shader::compileSource(
        Type type,  
        const std::string &vertexSource,
        const std::string &fragmentSource);

    inline static const char *getVertexShaderCompileLog() {
        return s_vertexLogBuffer;
    }
    
    inline static const char *getFragmentShaderCompileLog() {
        return s_fragmentLogBuffer;
    }

    ~Shader();
    

};
#endif