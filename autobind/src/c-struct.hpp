#ifndef C_STRUCT_H
#define C_STRUCT_H
#include <string>
#include <vector>
#include "c-parameter.hpp"

class CStruct {
private:
    std::string m_name;
    std::vector<CParameter> m_members;

public:
    inline CStruct(const std::string name, const std::vector<CParameter> &members)
        : m_name(name), m_members(members) {}

    inline const std::string &getName() const {
        return m_name;
    }

    inline const std::vector<CParameter> &getMembers() const {
        return m_members;
    }
    
};

#endif