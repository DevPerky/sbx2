#include "code-writer.hpp"
#include <unordered_map>

static const std::unordered_map<CParameter::Type::CType, std::string> cTypeStringMap() {
    std::unordered_map<CParameter::Type::CType, std::string> map;

    // TODO: Add more parameters
    map[CParameter::Type::CType::Int] = "int";
    map[CParameter::Type::CType::Double] = "double";
    map[CParameter::Type::CType::Float] = "float";
    map[CParameter::Type::CType::Void] = "void";
    map[CParameter::Type::CType::Char] = "char";

    return map;
}

const std::string CodeWriter::generateIndentationString() const {
    std::string indentationString;
    indentationString.resize(m_indentationLevel, '\t');
    return indentationString;
}

void CodeWriter::writeCParameter(const CParameter &parameter, bool isStatic) {
	auto &cTypeStrings = cTypeStringMap();

    if(isStatic) {
        m_stringStream << "static ";
    }

    if(parameter.getType().cType != CParameter::Type::CType::NonPrimitive) {
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

void CodeWriter::writeVariableInstance(const CParameter &parameter, bool isStatic) {
    writeCParameter(parameter, isStatic);
    m_stringStream << ";" << std::endl;
}


void CodeWriter::writeFunctionHeader(const CFunctionSpec &functionSpec, bool isStatic) {
    auto &cTypeStrings = cTypeStringMap();
    auto &indentationString = generateIndentationString();

    const std::string &returnTypeString = (functionSpec.getReturnType().cType != CParameter::Type::CType::NonPrimitive) ?
        cTypeStrings.at(functionSpec.getReturnType().cType) : functionSpec.getReturnType().typeName;
    
    m_stringStream << indentationString;

    if(isStatic) {
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

void CodeWriter::writeFunctionCall(const CFunctionSpec &functionSpec, const std::vector<std::string> &args) {
    m_stringStream << generateIndentationString();
    m_stringStream << functionSpec.getName();
    m_stringStream << "(";
    for(auto &arg : args) {
        m_stringStream << arg;
        if(&arg != &args.back()) {
            m_stringStream << ", ";
        }
    }
    m_stringStream << ");";
}


void CodeWriter::writeFunctionPrototype(const CFunctionSpec &functionSpec) {
    writeFunctionHeader(functionSpec);
    m_stringStream << ";";
}

void CodeWriter::writeFunctionImplementation(const CFunctionSpec &functionSpec, std::function<void()> writeContents) {
    writeFunctionHeader(functionSpec);
    m_stringStream << " {" << std::endl;
    int startIndentation = m_indentationLevel;
    m_indentationLevel++;
    writeContents();
    m_indentationLevel = startIndentation;
    writeNewLine();
    m_stringStream << "}";
}

void CodeWriter::writeVariableAssignment(const std::string &variableName, const std::string &value) {
    m_stringStream << generateIndentationString();
    m_stringStream << variableName << " = " << value << ";";
}

void CodeWriter::writeNewLine() {
    m_stringStream << std::endl;
}
