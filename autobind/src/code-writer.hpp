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

    inline CodeWriter(std::stringstream &stringStream)
        : m_stringStream(stringStream) {}

    void writeFunctionPrototype(const CFunctionSpec &functionSpec);
    void writeFunctionImplementation(const CFunctionSpec &functionSpec, std::function<void()> writeContents);
    void writeFunctionPointerTypeDef(const CFunctionSpec &functionSpec);
    void writeCParameter(const CParameter &parameter, bool isStatic = false);
    void writeVariableInstance(const CParameter &parameter, bool isStatic = false);
    void writeVariableAssignment(const std::string &variableName, const std::string &value);
    void writeFunctionCall(const CFunctionSpec &functionSpec, const std::vector<std::string> &args);
    void writeNewLine();
    
    inline void setIndentationLevel(int indentationLevel) {
        m_indentationLevel = indentationLevel;
    }

    inline const std::string getCode() const {
        return m_stringStream.str();
    }

};

#endif