#include "shader.hpp"

char Shader::s_vertexLogBuffer[SHADER_MAXIMUM_LOG_SIZE];
char Shader::s_fragmentLogBuffer[SHADER_MAXIMUM_LOG_SIZE];

Shader Shader::compileSource(
    Type type,
    const std::string &vertexSource,
    const std::string &fragmentSource) {
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char *vertexHeads[(int)Type::Count] = { 0 };
    const char *fragmentHeads[(int)Type::Count] = { 0 };
    
    vertexHeads[(int)Type::Basic] = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 VERTEX_POSITION;\n"
        "out vec4 VERTEX_COLOR;\n"; 

    
    fragmentHeads[(int)Type::Basic] = 
        "#version 330 core\n"
        "out vec4 FRAGMENT_COLOR;\n"
        "in vec4 VERTEX_COLOR;\n";  

    const char *vertexSources[] = {
        vertexHeads[(int)type],
        vertexSource.c_str()
    };
    
    const char *fragmentSources[] = {
        fragmentHeads[(int)type],
        fragmentSource.c_str()
    };

    glShaderSource(vertexShader, 2, vertexSources, NULL);
    glShaderSource(fragmentShader, 2, fragmentSources, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    glGetShaderInfoLog(vertexShader, SHADER_MAXIMUM_LOG_SIZE, NULL, s_vertexLogBuffer);
    glGetShaderInfoLog(fragmentShader, SHADER_MAXIMUM_LOG_SIZE, NULL, s_fragmentLogBuffer);

    GLuint programHandle = glCreateProgram();

    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return Shader(programHandle);
}
