#ifndef FUNCTION_SPEC_H
#define FUNCTION_SPEC_H

#include <vector>
#include <string>
#include "parameter.hpp"

class FunctionSpec {

public:
	inline const std::string& getName() const {
		return m_name;
	}

	inline const std::vector<Parameter>& getParametersIn() const {
		return m_parametersIn;
	}

	inline const std::vector<Parameter>& getParametersOut() const {
		return m_parametersOut;
	}

	FunctionSpec(const std::string name,
		std::vector<Parameter> parametersIn,
		std::vector<Parameter> parametersOut)
		: m_name(name), m_parametersIn(parametersIn), m_parametersOut(parametersOut) {}

private:
	const std::string m_name;
	const std::vector<Parameter> m_parametersIn, m_parametersOut;
};

#endif