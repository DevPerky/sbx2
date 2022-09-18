#include "binding-generator.h"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <exception>

static const std::string functionPointerSetterParameterName = "function";

static std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeToCParameterStringMap() {
	std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[FunctionSpec::Parameter::Type::Number] = "double";
	parameterTypeStrings[FunctionSpec::Parameter::Type::String] = "const char *";
	parameterTypeStrings[FunctionSpec::Parameter::Type::UserData] = "void *";

	return parameterTypeStrings;
}

static std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeToLuaParameterStringMap() {
	std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[FunctionSpec::Parameter::Type::Number] = "number";
	parameterTypeStrings[FunctionSpec::Parameter::Type::String] = "string";
	parameterTypeStrings[FunctionSpec::Parameter::Type::UserData] = "userdata";

	return parameterTypeStrings;
}

static std::string generateCParameter(const FunctionSpec::Parameter &parameter, bool out) {
	std::stringstream stringStream;
	std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[FunctionSpec::Parameter::Type::Number] = "double";
	parameterTypeStrings[FunctionSpec::Parameter::Type::String] = "const char *";
	parameterTypeStrings[FunctionSpec::Parameter::Type::UserData] = "void *";
	const std::string &typeString = parameterTypeStrings.at(parameter.type);

	if (typeString.back() != '*') { // ensure the pointer is right next to the identifier
		stringStream << " ";
	}
	if (out) {
		stringStream << "*";		// all "out parameters" are set via pointers
	}
	stringStream << parameter.name;

	stringStream << typeString;
}

static std::string generateCParameterInstance(const FunctionSpec::Parameter &parameter, bool pointer) {
	std::stringstream stringStream;
	
	std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterTypeStrings = parameterTypeToCParameterStringMap();
	const std::string &typeString = parameterTypeStrings.at(parameter.type);

	stringStream << typeString;

	if (typeString.back() != '*') { // ensure the pointer is right next to the identifier
		stringStream << " ";
	}
	if (pointer) {
		stringStream << "*";		// all "out parameters" are set via pointers
	}

	stringStream << parameter.name;

	return stringStream.str();
}

static std::string generateCParameterList(const std::vector<FunctionSpec::Parameter> &parameters, bool out) {
	std::stringstream stringStream;	

	for (auto &param : parameters) {
		stringStream << generateCParameterInstance(param, out);

		if (&param != &parameters.back()) { // only add a comma if there are more parameters to write
			stringStream << ", ";
		}

	}

	return stringStream.str();
}

inline static const std::string generateFunctionPointerTypeName(const std::string &functionName) {
	return "AB_" + functionName;
}

inline static const std::string generateRegisterFunctionsPrototype(const std::string &moduleName) {
	return "void AB_registerModule_" + moduleName + "(lua_State *L)";
}

inline static const std::string generateFunctionPointerVariableName(const std::string &functionName) {
	return "AB_" + functionName + "_binding";
}

inline static const std::string generateBindingFunctionName(const std::string &functionName) {
	return "l_" + functionName;
}

inline static const std::string generateBindingFunctionPrototype(const std::string &functionName) {
	return "int " + generateBindingFunctionName(functionName) + "(lua_State *L)";
}

static void writeFunctionPointerTypes(
	const std::vector<FunctionSpec> &functionSpecs,
	std::stringstream &stringStream) {
	
	for (auto &fs : functionSpecs) {
		stringStream << "typedef int (*" << generateFunctionPointerTypeName(fs.getName()) << ")";
		stringStream << "(" << generateCParameterList(fs.getParametersIn(), false);
		if (!fs.getParametersOut().empty()) {
			if (!fs.getParametersIn().empty()) {
				stringStream << ", ";
			}
			stringStream << generateCParameterList(fs.getParametersOut(), true);
		}

		stringStream << ");" << std::endl;
	}
}

static void writeFunctionPointerSetterProtype(const FunctionSpec &functionSpec, std::stringstream &stringStream) {
	stringStream << "void AB_bind_" << functionSpec.getName()
		<< "(" << generateFunctionPointerTypeName(functionSpec.getName())
		<< " " << functionPointerSetterParameterName << ")";

}

static void writeFunctionPointerSetterProtypes(const std::vector<FunctionSpec> &functionSpecs, std::stringstream &stringStream) {
	for (auto fs : functionSpecs) {
		writeFunctionPointerSetterProtype(fs, stringStream);
		stringStream << ";" << std::endl;
	}
}

static void writeFunctionPointerSetterImplementation(const FunctionSpec &functionSpec, std::stringstream &stringStream) {
	stringStream << "static " << generateFunctionPointerTypeName(functionSpec.getName()) << " "
		<< generateFunctionPointerVariableName(functionSpec.getName()) << ";" << std::endl;

	writeFunctionPointerSetterProtype(functionSpec, stringStream);
	stringStream << " {" << std::endl;
	stringStream << "\t" << generateFunctionPointerVariableName(functionSpec.getName()) << " = ";
	stringStream << functionPointerSetterParameterName << ";" << std::endl;
	stringStream << "}" << std::endl;

}

static void writeFunctionPointerSetterImplementations(const std::vector<FunctionSpec> &functionSpecs, std::stringstream &stringStream) {
	for (auto fs : functionSpecs) {

		writeFunctionPointerSetterImplementation(fs, stringStream);
		stringStream << std::endl;
	}
}

static void writeBindingImplementation(const FunctionSpec &functionSpec, std::stringstream &stringStream) {
	std::unordered_map<FunctionSpec::Parameter::Type, std::string> 
		parameterTypeStrings = parameterTypeToCParameterStringMap();

	std::unordered_map<FunctionSpec::Parameter::Type, std::string>
		luaParameterTypes = parameterTypeToLuaParameterStringMap();

	std::unordered_map<FunctionSpec::Parameter::Type, std::string> parameterCheckTypeFunctions;
	parameterCheckTypeFunctions[FunctionSpec::Parameter::Type::Number] = "lua_isnumber";
	parameterCheckTypeFunctions[FunctionSpec::Parameter::Type::String] = "lua_isstring";
	parameterCheckTypeFunctions[FunctionSpec::Parameter::Type::UserData] = "lua_isuserdata";

	std::unordered_map<FunctionSpec::Parameter::Type, std::string> getParameterValueFunctions;
	getParameterValueFunctions[FunctionSpec::Parameter::Type::Number] = "lua_tonumber";
	getParameterValueFunctions[FunctionSpec::Parameter::Type::String] = "lua_tostring";
	getParameterValueFunctions[FunctionSpec::Parameter::Type::UserData] = "lua_touserdata";

	std::unordered_map<FunctionSpec::Parameter::Type, std::string> luaPushValueFunctions;
	luaPushValueFunctions[FunctionSpec::Parameter::Type::Number] = "lua_pushnumber";
	luaPushValueFunctions[FunctionSpec::Parameter::Type::String] = "lua_pushstring";
	luaPushValueFunctions[FunctionSpec::Parameter::Type::UserData] = "lua_pushlightuserdata";

	stringStream << "static " << generateBindingFunctionPrototype(functionSpec.getName()) << " {" << std::endl;

	const std::vector<FunctionSpec::Parameter> inParams = functionSpec.getParametersIn();
	const std::vector<FunctionSpec::Parameter> outParams = functionSpec.getParametersOut();
	
	for (auto inParam : inParams) {
		stringStream << '\t' << generateCParameterInstance(inParam, false) << ';' << std::endl;
	}

	stringStream << std::endl;

	for (auto outParam : outParams) {
		stringStream << '\t' << generateCParameterInstance(outParam, false) << ';' << std::endl;
	}

	stringStream << std::endl;

	// Generates code checking the types for the in parameters on the lua stack
	// as well as retriving them if the type is correct
	for (int i = inParams.size() - 1; i >= 0; i--) {
		int luaIndex = inParams.size() - i;

		// Generates an if-statement checking the type of the parameter
		stringStream << "\t" << "if (" << parameterCheckTypeFunctions.at(inParams[i].type) << "(L, -"
			<< luaIndex << ")) {" << std::endl;

		// Generates the code retrieving the parameter from the lua stack
		stringStream << "\t\t" << inParams[i].name << " = " << getParameterValueFunctions.at(inParams[i].type)
			<< "(L, -" << luaIndex << ");" << std::endl;
		
		stringStream << "\t" << "}" << std::endl;

		// Generates the code that determines what happens if a parameter of an incorrect
		// type was passed to the runtime.
		stringStream << "\t" << "else {" << std::endl;
		stringStream << "\t\t" << "return luaL_error(L, \"Incorrect type for parameter " <<
			inParams[i].name << ". Expected type was " << luaParameterTypes.at(inParams[i].type) << ".\");" << std::endl;

		stringStream << "\t" << "}" << std::endl << std::endl;
	}

	std::string functionPointerName = generateFunctionPointerVariableName(functionSpec.getName());

	// Generates the code which calling the bound function in the runtime, if there is one.
	stringStream << "\t" << "if (" << functionPointerName << ") {" << std::endl;
	stringStream << "\t\t" << "if (!" << generateFunctionPointerVariableName(functionSpec.getName());
	stringStream << "(";

	for (auto &param : inParams) {
		stringStream << param.name;
		if (&param != &inParams.back()) {
			stringStream << ", ";
		}
	}

	if (!outParams.empty()) {
		if(!inParams.empty()) {
			stringStream << ", ";
		}

		for (auto &param : outParams) {
			stringStream << "&" << param.name;

			if (&param != &outParams.back()) {
				stringStream << ", ";
			}
		}
	}

	stringStream << ")) {" << std::endl;

	stringStream << "\t\t\t" << "return luaL_error(L, \"Runtime error: " << functionSpec.getName() 
		<< " failed for unknown reason!\");" << std::endl;
	stringStream << "\t\t}" << std::endl;
	stringStream << "\t" << "}" << std::endl;
	stringStream << "\t" << "else {" << std::endl;
	stringStream << "\t\t" << "return luaL_error(L, \"Runtime error: " << functionSpec.getName() 
		<< " wasn't bound!\");" << std::endl;
	stringStream << "\t" << "}" << std::endl;
	stringStream << std::endl;

	// Generates the code pushing the results 
	for (auto &param : outParams) {
		const std::string &luaPushValueFunction = luaPushValueFunctions.at(param.type);
		stringStream << "\t" << luaPushValueFunction << "(L, " << param.name << ");" << std::endl;
	}

	stringStream << std::endl;
	stringStream << "\t" << "return " << outParams.size() << ";" << std::endl;

	stringStream << "}" << std::endl;
}

static void writeBindingImplementations(const std::vector<FunctionSpec> &functionSpecs, std::stringstream &stringStream) {
	for (auto &fs : functionSpecs) {
		writeBindingImplementation(fs, stringStream);
		stringStream << std::endl;
		if (&fs != &functionSpecs.back()) {
			stringStream << std::endl;
		}
	}
}

static void writeRegisterModuleImplementation(const std::string moduleName, const std::vector<FunctionSpec> &functionSpecs, std::stringstream &stringStream) {
	stringStream << generateRegisterFunctionsPrototype(moduleName) << " {" << std::endl;
	
	for (auto fs : functionSpecs) {
		stringStream << "\t" << "lua_register(L, \"" << fs.getName() << "\", " << generateBindingFunctionName(fs.getName()) << ");" << std::endl;
	}
	
	stringStream << "}" << std::endl;
}


const std::string BindingGenerator::generateBindingInterface() const {
	std::stringstream stream;

	stream << "#include <lua.h>" << std::endl;
	
	stream << generateRegisterFunctionsPrototype(m_autoBindFile.getModuleName()) << ";" << std::endl;
	
	writeFunctionPointerTypes(m_autoBindFile.getFunctionSpecifications(), stream);
	stream << std::endl;

	writeFunctionPointerSetterProtypes(m_autoBindFile.getFunctionSpecifications(), stream);
	stream << std::endl;

	return stream.str();
}

const std::string BindingGenerator::generateBindingImplementation() const {
	std::stringstream stream;

	stream << "#include \"" << getInterfaceFileName() << "\"" << std::endl;
	stream << "#include <lauxlib.h>" << std::endl;
	stream << std::endl;
	writeFunctionPointerSetterImplementations(m_autoBindFile.getFunctionSpecifications(), stream);
	stream << std::endl;
	writeBindingImplementations(m_autoBindFile.getFunctionSpecifications(), stream);
	stream << std::endl;
	writeRegisterModuleImplementation(m_autoBindFile.getModuleName(), m_autoBindFile.getFunctionSpecifications(), stream);

	return stream.str();
}