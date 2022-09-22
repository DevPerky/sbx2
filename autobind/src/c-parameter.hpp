#ifndef C_PARAMETER_H
#define C_PARAMETER_H
#include <string>

class CParameter {
public:
    enum class Type {
        Struct,
        Int,
        Float,
        Double,
        Void
    };

private:
    const Type m_type;
    const std::string m_typeName;
    const std::string m_name;
    const unsigned int m_pointerLevels;


public:  
    inline CParameter(const std::string &name, Type type, unsigned int pointerLevels = 0, const std::string &typeName = nullptr)
        : m_name(name), m_type(type), m_typeName(typeName), m_pointerLevels(pointerLevels) {}
};


#endif