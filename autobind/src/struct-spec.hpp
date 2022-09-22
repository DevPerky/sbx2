#ifndef STRUCTSPEC_H
#define STRUCTSPEC_H

#include <string>
#include <vector>
#include "lua-parameter.hpp"

class StructSpec {
private:
    std::string m_name;
    std::vector<Parameter> m_members;

public:
    inline StructSpec(const std::string &name, const std::vector<Parameter> members)
        : m_name(name), m_members(members) {}

    inline const std::string &getName() const {
        return m_name;
    }

    inline const std::vector<Parameter> &getMembers() const {
        return m_members;
    }

};

#endif