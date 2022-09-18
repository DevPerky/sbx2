#ifndef STRUCTSPEC_H
#define STRUCTSPEC_H

#include <string>
#include <vector>
#include "parameter.hpp"

class StructSpec {
private:
    std::string m_name;
    std::vector<Parameter> m_members;
};

#endif