#ifndef FUNCTION_SPEC_H
#define FUNCTION_SPEC_H

#include <vector>
#include <string>
#include "lua-parameter.hpp"

class LuaFunctionSpec {

public:
	inline const std::string& getName() const {
		return m_name;
	}

	inline const std::vector<LuaParameter>& getParametersIn() const {
		return m_parametersIn;
	}

	inline const std::vector<LuaParameter>& getParametersOut() const {
		return m_parametersOut;
	}

	LuaFunctionSpec(const std::string name,
		std::vector<LuaParameter> parametersIn,
		std::vector<LuaParameter> parametersOut)
		: m_name(name), m_parametersIn(parametersIn), m_parametersOut(parametersOut) {}

private:
	const std::string m_name;
	const std::vector<LuaParameter> m_parametersIn, m_parametersOut;
};

#endif