#ifndef CODE_WRITER_H
#define CODE_WRITER_H
#include <sstream>
#include "c-function-spec.hpp"

class CodeWriter {
private:
    std::stringstream &m_stringStream;
    int m_indentationLevel;
    void writeFunctionHeader(const CFunctionSpec &functionSpec);
    const std::string generateIndentationString() const;

public:

    inline CodeWriter(std::stringstream &stringStream)
        : m_stringStream(stringStream) {}

    void writeFunctionProtoType(const CFunctionSpec &functionSpec);
    void writeCParameter(const CParameter &parameter);
    
    inline void setIndentationLevel(int indentationLevel) {
        m_indentationLevel = indentationLevel;
    }

    inline const std::string getCode() const {
        return m_stringStream.str();
    }

};

#endif