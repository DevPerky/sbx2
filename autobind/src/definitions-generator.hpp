#ifndef DEFINITIONS_GENERATOR_H
#define DEFINITIONS_GENERATOR_H
#include "autobind-file.hpp"
#include <string>
#include <sstream>


class DefinitionsGenerator {
private:
    enum class AnnotationType {
        Class
    };

    const AutoBindFile &m_autoBindFile;
    std::stringstream m_stringStream;
    void writeAnnotation(const AnnotationType annotationType, ...);
    void writeClassDefinition(const StructSpec &structSpec);
public:
    inline DefinitionsGenerator(const AutoBindFile &autoBindFile) 
        : m_autoBindFile(autoBindFile) {}

    const std::string generateDefinitions();    
};


#endif