#ifndef AUTOBIND_FILE_H
#define AUTOBIND_FILE_H
#include <vector>
#include "function-spec.h"
#include "struct-spec.hpp"

class AutoBindFile {

private:
	const std::vector<FunctionSpec> m_functionSpecifications;
	const std::vector<StructSpec> m_structSpecifications;
	const std::string m_moduleName;

public:
	AutoBindFile(
		std::string moduleName, 
		const std::vector<FunctionSpec> &functionSpecifications, 
		const std::vector<StructSpec> &structSpecifications
	) : m_moduleName(moduleName),
		m_functionSpecifications(functionSpecifications),
		m_structSpecifications(structSpecifications) {}

	inline const std::vector<FunctionSpec> &getFunctionSpecifications() const {
		return m_functionSpecifications;
	}

	inline const std::string &getModuleName() const {
		return m_moduleName;
	}

	static AutoBindFile LoadFromXML(std::string filePath);
};
#endif