#include "definitions-generator.hpp"
extern "C" {
#include <stdarg.h>
}

const std::string DefinitionsGenerator::getDefinitionsFileName() const {
    return m_autoBindFile.getModuleName() + "-lib.lua";
}


const std::string DefinitionsGenerator::getLuaTypeName(const LuaParameter &luaParameter) {
    switch(luaParameter.type) {
        case LuaParameter::Type::Number: return "number";
        case LuaParameter::Type::String: return "string";
        case LuaParameter::Type::UserData: return "lightuserdata";
        case LuaParameter::Type::Integer: return "integer";
        case LuaParameter::Type::Table: return luaParameter.typeName;
        
        default: return "error";
    }
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
    case AnnotationType::Field:
        argCount = 3;
        annotationString = "field";
        break;
    case AnnotationType::Param:
        argCount = 3;
        annotationString = "param";
        break;
    case AnnotationType::Return:
        argCount = 2;
        annotationString = "return";
        break;
    case AnnotationType::Meta:
        argCount = 0;
        annotationString = "meta";
        break;
    }

    m_stringStream << annotationString << " ";

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

    m_stringStream << std::endl;
}

void DefinitionsGenerator::writeClassDefinition(const StructSpec &structSpec) {
    writeAnnotation(AnnotationType::Class, structSpec.getName().c_str());
    for(auto &member : structSpec.getMembers()) {
        const std::string typeName = getLuaTypeName(member);
        writeAnnotation(AnnotationType::Field, member.name.c_str(), typeName.c_str(), "");
    }
    m_stringStream << std::endl;
}

void DefinitionsGenerator::writeFunctionDefinition(const LuaFunctionSpec &functionSpec) {

    for(auto &param : functionSpec.getParametersIn()) {
        const std::string typeName = getLuaTypeName(param);
        writeAnnotation(AnnotationType::Param, param.name.c_str(), typeName.c_str(), "");
    }

    for(auto &param : functionSpec.getParametersOut()) {
        const std::string typeName = getLuaTypeName(param);
        writeAnnotation(AnnotationType::Return, typeName.c_str(), param.name.c_str());
    }

    const auto &paramsIn = functionSpec.getParametersIn();
    m_stringStream << "function " << functionSpec.getName();
    m_stringStream << "(";
    for(auto &param : paramsIn) {
        m_stringStream << param.name;
        if(&param != &paramsIn.back()) {
            m_stringStream << ", ";
        }
    }
    m_stringStream << ") end";
    m_stringStream << std::endl;
}


const std::string DefinitionsGenerator::generateDefinitions() {
    m_stringStream.clear();
    auto &structSpecifications = m_autoBindFile.getStructSpecifications();
    auto &functionSpecifications = m_autoBindFile.getFunctionSpecifications();
    writeAnnotation(AnnotationType::Meta);
    
    for(auto &ss : structSpecifications) {
        writeClassDefinition(ss);
    }

    for(auto &fs : functionSpecifications) {
        writeFunctionDefinition(fs);
        if(&fs != &functionSpecifications.back()) {
            m_stringStream << std::endl;
        }
    }
    return m_stringStream.str();
}    
