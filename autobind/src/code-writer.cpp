#include "code-writer.hpp"
#include <unordered_map>

static const std::unordered_map<CParameter::Type::CType, std::string> cTypeStringMap() {
    std::unordered_map<CParameter::Type::CType, std::string> map;

    // TODO: Add more parameters
    map[CParameter::Type::CType::Int] = "int";
    map[CParameter::Type::CType::Double] = "double";
    map[CParameter::Type::CType::Float] = "float";
    map[CParameter::Type::CType::Void] = "void";

    return map;
}

const std::string CodeWriter::generateIndentationString() const {
    std::string indentationString;
    indentationString.resize(m_indentationLevel, '\t');
}

void CodeWriter::writeCParameter(const CParameter &parameter) {
	auto &cTypeStrings = cTypeStringMap();

    if(parameter.getType().cType != CParameter::Type::CType::Struct) {
        m_stringStream << cTypeStrings.at(parameter.getType().cType);
    }
    else {
        m_stringStream << parameter.getType().typeName;
    }

    m_stringStream << " ";
    for(int i = 0; i < parameter.getType().pointerLevels; i++) {
        m_stringStream << "*";
    } 
    
    m_stringStream << parameter.getName();
}	

void CodeWriter::writeFunctionHeader(const CFunctionSpec &functionSpec) {
    auto &cTypeStrings = cTypeStringMap();
    auto &indentationString = generateIndentationString();

    const std::string &returnTypeString = cTypeStrings.at(functionSpec.getReturnType().cType);
    m_stringStream << indentationString;

    if(functionSpec.isStatic()) {
        m_stringStream << "static ";
    }

    m_stringStream << returnTypeString << " ";
    m_stringStream << functionSpec.getName();
    m_stringStream << "(";
    for(auto &parameter : functionSpec.getInputParams()) {
        writeCParameter(parameter);
        if(&parameter != &functionSpec.getInputParams().back()) {
            m_stringStream << ", ";
        }
    }

    m_stringStream << ")";
}

void CodeWriter::writeFunctionProtoType(const CFunctionSpec &functionSpec) {
    writeFunctionHeader(functionSpec);
    m_stringStream << ";";
}

void CodeWriter::writeNewLine() {
    m_stringStream << std::endl;
}
