#ifndef AUTOBIND_FILE_H
#define AUTOBIND_FILE_H
#include <vector>
#include <unordered_map>
#include <memory>
#include "function-spec.h"
#include "struct-spec.hpp"

class AutoBindFile {

private:
	const std::vector<FunctionSpec> m_functionSpecifications;
	const std::vector<StructSpec> m_structSpecifications;
	const std::string m_moduleName;
	
	std::unordered_map<std::string, const StructSpec*> m_mapStructSpecifications;

public:
	AutoBindFile(
		std::string moduleName, 
		const std::vector<FunctionSpec> &functionSpecifications, 
		const std::vector<StructSpec> &structSpecifications
	);

	inline const StructSpec &getStructSpec(const std::string &name) const {
		return *m_mapStructSpecifications.at(name);
	}

	inline const std::vector<StructSpec> &getStructSpecifications() const {
		return m_structSpecifications;
	}

	inline const std::vector<FunctionSpec> &getFunctionSpecifications() const {
		return m_functionSpecifications;
	}

	inline const std::string &getModuleName() const {
		return m_moduleName;
	}

	static AutoBindFile LoadFromXML(std::string filePath);
};
#endif