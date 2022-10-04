#include "code-writer.hpp"
#include <unordered_map>
#include <assert.h>

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

    if(parameter.getType().isConst()) {
        m_stringStream << "const ";
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
    m_stringStream << generateIndentationString();
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
    writeParameterList(functionSpec.getInputParams());
    m_stringStream << ")";
}

void CodeWriter::writeParameterList(const std::vector<CParameter> &parameters) {
    for(auto &parameter : parameters) {
        writeCParameter(parameter);
        if(&parameter != &parameters.back()) {
            m_stringStream << ", ";
        }
    }
}

void CodeWriter::writeStruct(const CStruct &cStruct) {
    setIndentationLevel(0);
    m_stringStream << "typedef struct " << cStruct.getName() << " {";
    writeNewLine();
    setIndentationLevel(1);
    for(auto &param : cStruct.getMembers()) {
        writeVariableInstance(param, false);
    }
    setIndentationLevel(0);
    m_stringStream << "}" << cStruct.getName() << ";";
}


void CodeWriter::writeFunctionCall(const CFunctionSpec &functionSpec, const std::vector<std::string> &args, bool atomically) {
    assert(functionSpec.getInputParams().size() == args.size());

    if(atomically) {
        m_stringStream << generateIndentationString();
    }

    m_stringStream << functionSpec.getName();
    m_stringStream << "(";
    for(auto &arg : args) {
        m_stringStream << arg;
        if(&arg != &args.back()) {
            m_stringStream << ", ";
        }
    }
    m_stringStream << ")";

    if(atomically) {
        m_stringStream << ";";
    }
}

void CodeWriter::writeFunctionPointerCall(const std::string functionPointerName, const std::vector<std::string> &args, bool atomically) {
    if(atomically) {
        m_stringStream << generateIndentationString();
    }

    m_stringStream << functionPointerName;
    m_stringStream << "(";
    for(auto &arg : args) {
        m_stringStream << arg;
        if(&arg != &args.back()) {
            m_stringStream << ", ";
        }
    }
    m_stringStream << ")";

    if(atomically) {
        m_stringStream << ";";
    }
}

void CodeWriter::writeFunctionPrototype(const CFunctionSpec &functionSpec) {
    if(functionSpec.isStatic()) {
        m_stringStream << "static ";
    }

    writeFunctionHeader(functionSpec);
    m_stringStream << ";";
}

void CodeWriter::writeFunctionImplementation(const CFunctionSpec &functionSpec, std::function<void()> writeContents) {
    
    if(functionSpec.isStatic()) {
        m_stringStream << "static ";
    }

    writeFunctionHeader(functionSpec);
    m_stringStream << " {" << std::endl;
    int startIndentation = m_indentationLevel;
    m_indentationLevel++;
    writeContents();
    m_indentationLevel = startIndentation;
    writeNewLine();
    m_stringStream << "}";
}

void CodeWriter::writeIfStatement(std::function<void()> writePredicate, std::function<void()> writeIfTrue) {
    m_stringStream << generateIndentationString();
    m_stringStream << "if(";
    writePredicate();
    m_stringStream << ") {";
    writeNewLine();
    int startIndentation = m_indentationLevel;
    m_indentationLevel++;
    writeIfTrue();
    m_indentationLevel = startIndentation;
    writeNewLine();
    m_stringStream << generateIndentationString();
    m_stringStream << "}";
}

void CodeWriter::writeElseStatement(std::function<void()> content) {
    m_stringStream << generateIndentationString();
    m_stringStream << "else {";
    writeNewLine();
    int startIndentation = m_indentationLevel;
    m_indentationLevel++;
    content();
    m_indentationLevel = startIndentation;
    writeNewLine();
    m_stringStream << generateIndentationString();
    m_stringStream << "}";
}

void CodeWriter::writeReturnStatement(std::function<void()> content) {
    m_stringStream << generateIndentationString();
    m_stringStream << "return ";
    content();
    m_stringStream << ";";
}


void CodeWriter::writeInstantiateParameters(const std::vector<CParameter> &parameters) {
    for(auto &parameter : parameters) {
        m_stringStream << generateIndentationString();
        writeCParameter(parameter);
        m_stringStream << ";";
        if(&parameter != &parameters.back()) {
            writeNewLine();
        }
    }
}

void CodeWriter::writeFunctionPointerTypeDef(const CFunctionSpec &functionSpec) {
    m_stringStream << "typedef ";

    if(functionSpec.getReturnType().cType == CParameter::Type::CType::NonPrimitive) {
        m_stringStream << functionSpec.getReturnType().typeName;
    }
    else {
        m_stringStream << cTypeStringMap().at(functionSpec.getReturnType().cType);
    }
    
    m_stringStream << " (*" << functionSpec.getName() << ")(";
    writeParameterList(functionSpec.getInputParams());
    m_stringStream << ");";
}

void CodeWriter::writeVariableAssignment(const std::string &variableName, const std::string &value) {
    m_stringStream << generateIndentationString();
    m_stringStream << variableName << " = " << value << ";";
}

void CodeWriter::writeComparisonOperation(std::function<void()> expressionA, std::function<void()> expressionB, ComparisonOperator op) {
    std::string comparisonString = "error";
    switch(op) {
        case ComparisonOperator::Equal:
            comparisonString = "==";
            break;
        case ComparisonOperator::LessThan:
            comparisonString = "<";
            break;
        case ComparisonOperator::LessThanOrEqual:
            comparisonString = "<=";
            break;
        case ComparisonOperator::MoreThan:
            comparisonString = ">";
            break;
        case ComparisonOperator::MoreThanOrEqual:
            comparisonString = "=>";
            break;
        case ComparisonOperator::NotEqual:
            comparisonString = "!=";
            break;
    }
    
    expressionA();
    m_stringStream << " " << comparisonString + " ";
    expressionB();
}


void CodeWriter::writeVariableAssignment(
    const std::string &variableName,
    const CFunctionSpec &function,
    const std::vector<std::string> &args
) {
    m_stringStream << generateIndentationString();
    m_stringStream << variableName << " = ";
    writeFunctionCall(function, args, false);
    m_stringStream << ";";
}


void CodeWriter::writeNewLine(int n) {
    for(int i = 0; i < n; i++) {
        m_stringStream << std::endl;
    }
}

void CodeWriter::writeIdentifier(const std::string &identifier) {
    m_stringStream << identifier;
}

void CodeWriter::writeIntegerLiteral(int number) {
    m_stringStream << number;
}


void CodeWriter::writeNull() {
    m_stringStream << "0";
}
