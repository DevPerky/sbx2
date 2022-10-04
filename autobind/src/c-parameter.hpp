#ifndef C_PARAMETER_H
#define C_PARAMETER_H
#include <string>

class CParameter {
public:
    struct Type {
        enum class CType {
            NonPrimitive,
            Int,
            Float,
            Double,
            Void,
            Char,
            Lua_Integer,
            Invalid
        }const cType;
        const std::string typeName;
        const unsigned int pointerLevels;
        const bool m_isConst;

        inline Type(CType cType, const std::string &typeName, const unsigned int pointerLevels, bool isConst = false)
            : cType(cType), typeName(typeName), pointerLevels(pointerLevels), m_isConst(isConst) {}

        inline const bool isConst() const {
            return m_isConst;
        }
        
    };

private:
    const Type m_type;
    const std::string m_name;

public:  
    inline CParameter(const std::string &name, Type type)
        : m_name(name), m_type(type){}

    inline const Type getType() const {
        return m_type;
    }

    inline const std::string getName() const {
        return m_name;
    }
};


#endif