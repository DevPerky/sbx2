#ifndef DEFINITIONS_GENERATOR_H
#define DEFINITIONS_GENERATOR_H
#include "autobind-file.hpp"

class DefinitionsGenerator {
private:
    const AutoBindFile &m_autoBindFile;

public:
    inline DefinitionsGenerator(const AutoBindFile &autoBindFile) 
        : m_autoBindFile(autoBindFile) {}

    std::string generateDefinitions();    
};


#endif