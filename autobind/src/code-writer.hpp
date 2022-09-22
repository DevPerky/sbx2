#ifndef CODE_WRITER_H
#define CODE_WRITER_H
#include <sstream>
#include "c-function-spec.hpp"

class CodeWriter {
private:
    std::stringstream &m_stringStream;
    int m_indentationLevel = 0;

    void writeFunctionHeader(const CFunctionSpec &functionSpec, bool isStatic = false);
    const std::string generateIndentationString() const;

public:

    inline CodeWriter(std::stringstream &stringStream)
        : m_stringStream(stringStream) {}

    void writeFunctionPrototype(const CFunctionSpec &functionSpec);
    void writeCParameter(const CParameter &parameter);
    void writeNewLine();
    
    inline void setIndentationLevel(int indentationLevel) {
        m_indentationLevel = indentationLevel;
    }

    inline const std::string getCode() const {
        return m_stringStream.str();
    }

};

#endif