#ifndef C_FUNCTION_SPEC_H
#define C_FUNCTION_SPEC_H
#include <vector>
#include "c-parameter.hpp"

class CFunctionSpec {

private:
    const std::string m_name;
    const CParameter::Type m_returnType;
    const std::vector<CParameter> m_inputParams;

public:
    inline CFunctionSpec(const std::string &name, const CParameter::Type &returnType, const std::vector<CParameter> &inputParams)
        : m_name(name), m_returnType(returnType), m_inputParams(inputParams) {}
    
    inline const std::string &getName() const {
        return m_name;
    }

    inline const CParameter::Type &getReturnType() const {
        return m_returnType;
    }

    inline const std::vector<CParameter> &getInputParams() const {
        return m_inputParams;
    }
    
    

};


#endif