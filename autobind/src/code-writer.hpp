#ifndef CODE_WRITER_H
#define CODE_WRITER_H
#include <sstream>
#include "c-function-spec.hpp"
#include <functional>

class CodeWriter {
private:
    std::stringstream &m_stringStream;
    int m_indentationLevel = 0;

    void writeFunctionHeader(const CFunctionSpec &functionSpec, bool isStatic = false);
    void writeParameterList(const std::vector<CParameter> &parameters);

    const std::string generateIndentationString() const;

public:
    enum class ComparisonOperator {
        Equal,
        NotEqual,
        MoreThan,
        LessThan,
        LessThanOrEqual,
        MoreThanOrEqual
    };

    inline CodeWriter(std::stringstream &stringStream)
        : m_stringStream(stringStream) {}

    void writeFunctionPrototype(const CFunctionSpec &functionSpec);
    void writeFunctionImplementation(const CFunctionSpec &functionSpec, std::function<void()> writeContents);
    void writeFunctionPointerTypeDef(const CFunctionSpec &functionSpec);
    
    void writeCParameter(const CParameter &parameter, bool isStatic = false);
    void writeVariableInstance(const CParameter &parameter, bool isStatic = false);
    void writeVariableAssignment(const std::string &variableName, const std::string &value);
    void writeVariableAssignment(const std::string &variableName, const CFunctionSpec &function, const std::vector<std::string> &args);
    void writeComparisonOperation(std::function<void()> expressionA, std::function<void()> expressionB, ComparisonOperator op);
    void writeFunctionCall(const CFunctionSpec &functionSpec, const std::vector<std::string> &args, bool atomically = true);
    void writeFunctionPointerCall(const std::string functionPointerName, const std::vector<std::string> &args, bool atomically = true);
    
    void writeIfStatement(std::function<void()> writePredicate, std::function<void()> writeIfTrue);
    void writeElseStatement(std::function<void()> content);
    void writeReturnStatement(std::function<void()> content);

    void writeIdentifier(const std::string &identifier);
    void writeIntegerLiteral(int number);
    void writeNull();
    
    void writeInstantiateParameters(const std::vector<CParameter> &parameters);
    void writeNewLine(int n = 1);
    
    inline void setIndentationLevel(int indentationLevel) {
        m_indentationLevel = indentationLevel;
    }

    inline const std::string getCode() const {
        return m_stringStream.str();
    }

};

#endif