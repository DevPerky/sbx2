#ifndef AUTOBIND_FILE_H
#define AUTOBIND_FILE_H
#include <vector>
#include "function-spec.h"

class AutoBindFile {

private:
	const std::vector<FunctionSpec> m_functionSpecifications;
	const std::string m_moduleName;

public:
	AutoBindFile(std::string moduleName, std::vector<FunctionSpec>& functionSpecifications)
		: m_moduleName(moduleName), m_functionSpecifications(functionSpecifications) {}

	inline const std::vector<FunctionSpec> &getFunctionSpecifications() const {
		return m_functionSpecifications;
	}

	inline const std::string &getModuleName() const {
		return m_moduleName;
	}

	static AutoBindFile LoadFromXML(std::string filePath);
};
#endif