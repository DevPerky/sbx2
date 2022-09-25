#include "definitions-generator.hpp"
extern "C" {
#include <stdarg.h>
}

void DefinitionsGenerator::writeAnnotation(const AnnotationType annotationType, ...) {
    m_stringStream << "---@";
    std::string annotationString = "error";
    int argCount = 0;

    switch (annotationType)
    {
    case AnnotationType::Class: 
        argCount = 1;
        annotationString = "class";
        break;
    }

    m_stringStream << " ";

    va_list vl;
    va_start(vl, annotationType);
    for(int i = 0; i < argCount; i++) {
        const char *arg = va_arg(vl, const char*);
        m_stringStream << arg;
        if(i < argCount - 1) {
            m_stringStream << " ";
        }
    }
    va_end(vl);
}

void DefinitionsGenerator::writeClassDefinition(const StructSpec &structSpec) {
    writeAnnotation(AnnotationType::Class, structSpec.getName().c_str());
}


const std::string DefinitionsGenerator::generateDefinitions() {
    m_stringStream.clear();
    auto &structSpecifications = m_autoBindFile.getStructSpecifications();

    return m_stringStream.str();
}    
