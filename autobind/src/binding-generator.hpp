#ifndef BINDING_GENERATOR_H
#define BINDING_GENERATOR_H
#include <vector>
#include "lua-function-spec.hpp"
#include "autobind-file.hpp"
#include "code-writer.hpp"

#include <string>

class BindingGenerator {
private:
    const AutoBindFile &m_autoBindFile;

	const CFunctionSpec getRegisterModuleFunction() const;
	const CFunctionSpec getFunctionPointerSetterFunction(const LuaFunctionSpec &functionSpec) const;
	const CFunctionSpec getLuaRegisterFunction() const;
	
public:
    inline BindingGenerator(const AutoBindFile &autoBindFile) 
		: m_autoBindFile(autoBindFile) {}

    const std::string generateBindingInterface() const;
	const std::string generateBindingImplementation() const;

	inline const std::string getInterfaceFileName() const {
		return "AB-" + m_autoBindFile.getModuleName() + "-interface.h";
	}
	inline const std::string getImplementationFileName() const {
		return "AB-" + m_autoBindFile.getModuleName() + "-implementation.c";
	}


};

#endif
