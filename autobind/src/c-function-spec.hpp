#ifndef C_FUNCTION_SPEC_H
#define C_FUNCTION_SPEC_H
#include <vector>
#include "c-parameter.hpp"

class CFunctionSpec {

private:
    const std::string m_name;
    const CParameter::Type m_returnType;
    const std::vector<CParameter> m_inputParams;
    const bool m_isStatic;

public:
    inline CFunctionSpec(const std::string &name,
        const CParameter::Type &returnType,
        const std::vector<CParameter> &inputParams,
        bool isStatic = false
    ) : m_name(name), m_returnType(returnType), m_inputParams(inputParams), m_isStatic(isStatic) {}
    
    inline const std::string &getName() const {
        return m_name;
    }

    inline const CParameter::Type &getReturnType() const {
        return m_returnType;
    }

    inline const std::vector<CParameter> &getInputParams() const {
        return m_inputParams;
    }

    inline const bool isStatic() const {
        return m_isStatic;
    }
};


#endif