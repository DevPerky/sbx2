#include "binding-generator.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <exception>

static const std::string functionPointerSetterParameterName = "function";

static std::unordered_map<LuaParameter::Type, std::string> parameterTypeToCParameterStringMap() {
	std::unordered_map<LuaParameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[LuaParameter::Type::Number] = "double";
	parameterTypeStrings[LuaParameter::Type::String] = "const char *";
	parameterTypeStrings[LuaParameter::Type::UserData] = "void *";

	return parameterTypeStrings;
}

static std::unordered_map<LuaParameter::Type, std::string> parameterTypeToLuaParameterStringMap() {
	std::unordered_map<LuaParameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[LuaParameter::Type::Number] = "number";
	parameterTypeStrings[LuaParameter::Type::String] = "string";
	parameterTypeStrings[LuaParameter::Type::UserData] = "userdata";
	parameterTypeStrings[LuaParameter::Type::Table] = "table";
	

	return parameterTypeStrings;
}

static const std::unordered_map<LuaParameter::Type, std::string> getParameterTypeCheckFunctions() {
	std::unordered_map<LuaParameter::Type, std::string> parameterCheckTypeFunctions;
	parameterCheckTypeFunctions[LuaParameter::Type::Number] = "lua_isnumber";
	parameterCheckTypeFunctions[LuaParameter::Type::String] = "lua_isstring";
	parameterCheckTypeFunctions[LuaParameter::Type::UserData] = "lua_isuserdata";
	parameterCheckTypeFunctions[LuaParameter::Type::Table] = "lua_istable";

	return parameterCheckTypeFunctions;
}

static std::unordered_map<LuaParameter::Type, std::string> getParameterValueFunctions() {
	std::unordered_map<LuaParameter::Type, std::string> parameterValueFunctions;
	parameterValueFunctions[LuaParameter::Type::String] = "lua_tostring";
	parameterValueFunctions[LuaParameter::Type::Number] = "lua_tonumber";
	parameterValueFunctions[LuaParameter::Type::UserData] = "lua_touserdata";

	return parameterValueFunctions;
}

static std::string generateCParameter(const LuaParameter &parameter, bool out) {
	std::stringstream stringStream;
	std::unordered_map<LuaParameter::Type, std::string> parameterTypeStrings;

	parameterTypeStrings[LuaParameter::Type::Number] = "double";
	parameterTypeStrings[LuaParameter::Type::String] = "const char *";
	parameterTypeStrings[LuaParameter::Type::UserData] = "void *";
	const std::string &typeString = parameterTypeStrings.at(parameter.type);

	if (typeString.back() != '*') { // ensure the pointer is right next to the identifier
		stringStream << " ";
	}
	if (out) {
		stringStream << "*";		// all "out parameters" are set via pointers
	}
	stringStream << parameter.name;

	stringStream << typeString;

	return stringStream.str();
}	

static std::string generateCParameterInstance(const LuaParameter &parameter, bool pointer) {
	std::stringstream stringStream;
	
	std::unordered_map<LuaParameter::Type, std::string> parameterTypeStrings = parameterTypeToCParameterStringMap();
	
	const std::string &typeString = (parameter.type == LuaParameter::Type::Table) ?
		parameter.typeName : parameterTypeStrings.at(parameter.type);

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

static std::string generateCParameterList(const std::vector<LuaParameter> &parameters, bool out) {
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

inline static const std::string generateCustomToFunctionName(const std::string &typeName) {
	return "AB_to_" + typeName;
}

inline static const std::string generateCustomPushFunctionName(const std::string &typeName) {
	return "AB_push_" + typeName;
}

inline static const std::string generateCustomGetterFunctionPrototype(const std::string &typeName) {
	return "int " + generateCustomToFunctionName(typeName) + "(lua_State *L, int index, " + typeName + " *to)";
}

inline static const std::string generateIndentation(int indentation) {
	std::string indentationString;
	for(int i = 0; i < indentation; i++) {
		indentationString += "\t";
	}
	return indentationString;
}

static void writeFunctionPointerTypes(
	const std::vector<LuaFunctionSpec> &functionSpecs,
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


static void writeIfTypeCorrectGet(const LuaParameter &param, int stackIndex, std::stringstream &stringStream, int indentationLevel) {
	std::unordered_map<LuaParameter::Type, std::string> parameterCheckTypeFunctions = getParameterTypeCheckFunctions();
	std::unordered_map<LuaParameter::Type, std::string> parameterValueFunctions = getParameterValueFunctions();
	std::unordered_map<LuaParameter::Type, std::string> luaParameterTypes = parameterTypeToLuaParameterStringMap();
	std::string indentation = generateIndentation(indentationLevel);
	// Generates an if-statement checking the type of the parameter
	stringStream << indentation << "if (" << parameterCheckTypeFunctions.at(param.type) << "(L, "
		<< stackIndex << ")) {" << std::endl;

	// Generates the code retrieving the parameter from the lua stack
	stringStream << indentation << "\t" << param.name << " = " << parameterValueFunctions.at(param.type)
		<< "(L, " << stackIndex << ");" << std::endl;
	
	stringStream << indentation << "}" << std::endl;
}

static void writeBindingImplementation(const LuaFunctionSpec &functionSpec, std::stringstream &stringStream) {
	std::unordered_map<LuaParameter::Type, std::string> 
		parameterTypeStrings = parameterTypeToCParameterStringMap();

	std::unordered_map<LuaParameter::Type, std::string>
		luaParameterTypes = parameterTypeToLuaParameterStringMap();

	std::unordered_map<LuaParameter::Type, std::string> parameterCheckTypeFunctions = getParameterTypeCheckFunctions();
	std::unordered_map<LuaParameter::Type, std::string> parameterValueFunctions = getParameterValueFunctions();

	std::unordered_map<LuaParameter::Type, std::string> luaPushValueFunctions;
	luaPushValueFunctions[LuaParameter::Type::Number] = "lua_pushnumber";
	luaPushValueFunctions[LuaParameter::Type::String] = "lua_pushstring";
	luaPushValueFunctions[LuaParameter::Type::UserData] = "lua_pushlightuserdata";

	stringStream << "static " << generateBindingFunctionPrototype(functionSpec.getName()) << " {" << std::endl;

	const std::vector<LuaParameter> inParams = functionSpec.getParametersIn();
	const std::vector<LuaParameter> outParams = functionSpec.getParametersOut();
	
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
		
		// TODO: Remove this and fix instead


		int luaIndex = -(inParams.size() - i);
		if(inParams[i].type == LuaParameter::Type::Table) {
			stringStream << "\t" << "if(lua_istable(L, " << luaIndex << ")) {" << std::endl;
			stringStream << "\t\t" << "if(!" << generateCustomToFunctionName(inParams[i].typeName) << "(L, " << luaIndex << ", &" << inParams[i].name << ")) {" << std::endl;
			stringStream << "\t\t\t" << "return luaL_error(L, \"Error loading custom type " << inParams[i].typeName << "\");" << std::endl;
			stringStream << "\t\t" << "}" << std::endl;
			stringStream << "\t" << "}" << std::endl;
		}
		else {
			writeIfTypeCorrectGet(inParams[i], luaIndex, stringStream, 1);
		}
		// Generates the code that determines what happens if a parameter of an incorrect
		// type was passed to the runtime.
		stringStream << "\t" << "else {" << std::endl;
		stringStream << "\t" << "\t" << "return luaL_error(L, \"Incorrect type for parameter " <<
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

static void writeBindingImplementations(const std::vector<LuaFunctionSpec> &functionSpecs, std::stringstream &stringStream) {
	for (auto &fs : functionSpecs) {
		writeBindingImplementation(fs, stringStream);
		stringStream << std::endl;
		if (&fs != &functionSpecs.back()) {
			stringStream << std::endl;
		}
	}
}

static void writeCustomType(const StructSpec &structSpecification, std::stringstream &stringStream) {
	stringStream << "typedef struct " << structSpecification.getName() << " {" << std::endl;
	for(const auto &member : structSpecification.getMembers()) {
		
		stringStream << "\t";
		stringStream << generateCParameterInstance(member, false) << ";" << std::endl;
		
	}
	stringStream << "}" << structSpecification.getName() << ";" << std::endl;
}

static void writeCustomTypes(const std::vector<StructSpec> &structSpecifications, std::stringstream &stringStream) {

	for(const auto &structSpec : structSpecifications) {
		writeCustomType(structSpec, stringStream);
		stringStream << std::endl;
	}
}


static void writeCustomGetters(const std::vector<StructSpec> &structSpecifications, std::stringstream &stringStream) {
	const std::unordered_map<LuaParameter::Type, std::string> &parameterTypeCheckFunctions = getParameterTypeCheckFunctions();
	const std::unordered_map<LuaParameter::Type, std::string> &parameterValueFunctions = getParameterValueFunctions();

	for(auto &structSpec : structSpecifications) {
		stringStream << generateCustomGetterFunctionPrototype(structSpec.getName()) << ";" << std::endl; 
	}

	stringStream << std::endl;
	for(auto &structSpec : structSpecifications) {

		stringStream << generateCustomGetterFunctionPrototype(structSpec.getName()) << " {" << std::endl;
		for (auto &member : structSpec.getMembers()) {
			stringStream << '\t' << generateCParameterInstance(member, false) << ';' << std::endl;
		}

		for(auto &member : structSpec.getMembers()) {
			stringStream << "\tlua_getfield(L, index, " << "\"" << member.name << "\");" << std::endl;
			
			if(member.type == LuaParameter::Type::Table) {
				stringStream << "\t" << "if (!" << generateCustomToFunctionName(member.typeName) << "(L, 1, &" << member.name << ")) {" << std::endl;
				stringStream << "\t\t" << "lua_pop(L, 1);" << std::endl;
				stringStream << "\t\t" << "return 0;" << std::endl;
				stringStream << "\t" << "}" << std::endl;
			}
			else {
				writeIfTypeCorrectGet(member, 1, stringStream, 1);
				stringStream << "\t" << "else {" << std::endl;
				stringStream << "\t\t" << "lua_pop(L, 1);" << std::endl;
				stringStream << "\t\t" << "return 0;" << std::endl;
				stringStream << "\t" << "}" << std::endl;
			}
			stringStream << "\t" << "to->" << member.name << " = " << member.name << ";" << std::endl;
			stringStream << "\t" << "lua_pop(L, 1);" << std::endl;
			stringStream << std::endl;
		}
		stringStream << std::endl;

		stringStream << "\treturn 1;" << std::endl;
		stringStream << "}" << std::endl << std::endl;
	}
}



const CFunctionSpec BindingGenerator::getRegisterModuleFunction() const {
	std::vector<CParameter> registerModuleParams;
	registerModuleParams.push_back(
		CParameter("L", CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1))
	);

	return CFunctionSpec(
		(std::string("AB_registerModule_") + m_autoBindFile.getModuleName()),
		CParameter::Type(CParameter::Type::CType::Void, "", 0),
		registerModuleParams
	);
}

const CFunctionSpec BindingGenerator::getFunctionPointerSetterFunction(const LuaFunctionSpec &functionSpec) const{
	std::vector<CParameter> cParams;
	cParams.push_back(
		CParameter(
			functionPointerSetterParameterName,
			CParameter::Type(
				CParameter::Type::CType::NonPrimitive,
				generateFunctionPointerTypeName(functionSpec.getName()),
				0
			)
		)
	);

	return CFunctionSpec(
		std::string("AB_bind_") + functionSpec.getName(),
		CParameter::Type(CParameter::Type::CType::Void, "", 0),
		cParams,
		false
	);

}

const CFunctionSpec BindingGenerator::getLuaRegisterFunction() const {
	std::vector<CParameter> cParams;

	cParams.push_back(
		CParameter("L", CParameter::Type (CParameter::Type::CType::NonPrimitive, "lua_State", 1))
	);
	cParams.push_back(
		CParameter("name", CParameter::Type (CParameter::Type::CType::Char, "", 1))
	);
	cParams.push_back(
		CParameter("bindingName", CParameter::Type (CParameter::Type::CType::Char, "", 1))
	);
	
	return CFunctionSpec(
		"lua_register",
		CParameter::Type (CParameter::Type::CType::Void, "", 0),
		cParams, false
	);
}

const CParameter::Type BindingGenerator::getCParameterTypeTranslation(const LuaParameter &param, bool out) const {
	CParameter::Type::CType cType;
	int pointerLevels = out ? 1 : 0;
	std::string typeName = "";

	switch(param.type) {
		case LuaParameter::Type::Number:
			cType = CParameter::Type::CType::Double;
		break;

		case LuaParameter::Type::String:
			cType = CParameter::Type::CType::Char;
			pointerLevels += 1;
		break;

		case LuaParameter::Type::UserData:
			cType = CParameter::Type::CType::Void;
			pointerLevels += 1;
		break;

		case LuaParameter::Type::Table:
			cType = CParameter::Type::CType::NonPrimitive;
			typeName = param.typeName;
		break;
	}

	return CParameter::Type(cType, typeName, pointerLevels);
}

const std::vector<CParameter> BindingGenerator::getCParameters(
	const std::vector<LuaParameter> &inParams,
	const std::vector<LuaParameter> &outParams,
	bool asFields
) const {
	std::vector<CParameter> cParams;
	for(auto &paramIn : inParams) {
		cParams.push_back(CParameter(
			paramIn.name,
			getCParameterTypeTranslation(paramIn, false)
		));
	}

	for(auto &paramOut : outParams) {
		cParams.push_back(CParameter(
			paramOut.name,
			getCParameterTypeTranslation(paramOut, !asFields)
		));
	}

	return cParams;
}

const CFunctionSpec BindingGenerator::getBindingCallBackFunction(const LuaFunctionSpec &functionSpec) const {
	std::vector<CParameter> cParams = getCParameters(functionSpec.getParametersIn(), functionSpec.getParametersOut());
	return CFunctionSpec(
		generateFunctionPointerTypeName(functionSpec.getName()),
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams
	);
}

const CFunctionSpec BindingGenerator::getBindingFunction(const LuaFunctionSpec &functionSpec) const {
	std::vector<CParameter> cParams;

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	return CFunctionSpec(
		generateBindingFunctionName(functionSpec.getName()),
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams,
		true
	);
}

const CFunctionSpec BindingGenerator::getLuaCheckTypeFunction(const LuaParameter::Type &luaType) const {
	std::string functionName = "error";
	std::vector<CParameter> cParams;

	switch(luaType) {
		case LuaParameter::Type::Number:
			functionName = "lua_isnumber";
			break;
		case LuaParameter::Type::String:
			functionName = "lua_isstring";
			break;
		case LuaParameter::Type::UserData:
			functionName = "lua_islightuserdata";
			break;
		case LuaParameter::Type::Table:
			functionName = "lua_istable";
			break;
	}

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"index",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	return CFunctionSpec(
		functionName,
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams
	);

}

const CFunctionSpec BindingGenerator::getLuaToFunction(const LuaParameter &parameter) const {
	std::string functionName = "error";
	std::vector<CParameter> cParams;

	switch(parameter.type) {
		case LuaParameter::Type::Number:
			functionName = "lua_tonumber";
			break;
		case LuaParameter::Type::String:
			functionName = "lua_tostring";
			break;
		case LuaParameter::Type::UserData:
			functionName = "lua_tolightuserdata";
			break;
		case LuaParameter::Type::Table:
			functionName = generateCustomToFunctionName(parameter.typeName);
			break;
	}

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"index",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	return CFunctionSpec(
		functionName,
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams
	);
}

const CFunctionSpec BindingGenerator::getLuaPushFunction(const CParameter::Type &cType) const {
	std::string functionName = "error";
	std::vector<CParameter> cParams;

	auto setToPushNumber = [&functionName, &cType]() {
		if(cType.pointerLevels == 0) {
			functionName = "lua_pushnumber";
		}	
	};

	switch(cType.cType) {
		case CParameter::Type::CType::Char:
			if(cType.pointerLevels == 1) {
				functionName = "lua_pushstring";
			}
			else if(cType.pointerLevels == 0) {
				functionName = "lua_pushnumber";
			}
			break;
		case CParameter::Type::CType::Double:
			setToPushNumber();
			break;
		case CParameter::Type::CType::Float:
			setToPushNumber();
			break;
		case CParameter::Type::CType::Int:
			setToPushNumber();
			break;
		case CParameter::Type::CType::Void:
			if(cType.pointerLevels > 0) {
				functionName = "lua_pushlightuserdata";
			}
			break;
		
		case CParameter::Type::CType::NonPrimitive:
			functionName = generateCustomPushFunctionName(cType.typeName);
			break;
	}

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"value",
			cType
		)
	);

	return CFunctionSpec(
		functionName,
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams
	);
}

const CFunctionSpec BindingGenerator::getLuaLErrorFunction() const {
	std::vector<CParameter> cParams;
	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"index",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	return CFunctionSpec(
		"luaL_error",
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams
	);
}


const std::string BindingGenerator::generateBindingInterface() const {
	std::stringstream stream;

	stream << "#include <lua.h>" << std::endl;
	CodeWriter codeWriter(stream);

	codeWriter.writeFunctionPrototype(getRegisterModuleFunction());
	codeWriter.writeNewLine();
	
	writeCustomTypes(m_autoBindFile.getStructSpecifications(), stream);
	stream << std::endl;
	
	for(auto &fs : m_autoBindFile.getFunctionSpecifications()) {
		codeWriter.writeFunctionPointerTypeDef(getBindingCallBackFunction(fs));
		codeWriter.writeNewLine();
	}

	for(auto &fs : m_autoBindFile.getFunctionSpecifications()) {
		codeWriter.writeFunctionPrototype(getFunctionPointerSetterFunction(fs));
		codeWriter.writeNewLine();
	}

	return stream.str();
}

const std::string BindingGenerator::generateBindingImplementation() const {
	std::stringstream stream;
	CodeWriter codeWriter(stream);
	stream << "#include \"" << getInterfaceFileName() << "\"" << std::endl;
	stream << "#include <lauxlib.h>" << std::endl;
	stream << std::endl;
	writeCustomGetters(m_autoBindFile.getStructSpecifications(), stream);
	stream << std::endl;

	auto &functionSpecifications = m_autoBindFile.getFunctionSpecifications();
	// Write function pointer setter implementations
	for(auto &fs : functionSpecifications) {
		auto &functionPointerSetterSpec = getFunctionPointerSetterFunction(fs);
		
		CParameter bindingVariable(
			generateFunctionPointerVariableName(fs.getName()),
			CParameter::Type(
				CParameter::Type::CType::NonPrimitive,
				functionPointerSetterSpec.getInputParams()[0].getType().typeName,
				0
			)
		);

		codeWriter.writeVariableInstance(bindingVariable, true);
		codeWriter.writeFunctionImplementation(functionPointerSetterSpec, [&](){
			codeWriter.writeVariableAssignment(
				bindingVariable.getName(),
				functionPointerSetterSpec.getInputParams()[0].getName()
			);
		});
		codeWriter.writeNewLine(2);
	}

	// Write binding implementations
	for(auto &fs : functionSpecifications) {
		const auto &bindingFunction = getBindingFunction(fs);
		codeWriter.writeFunctionImplementation(bindingFunction, [&, this](){
			auto &cParamVariables = this->getCParameters(fs.getParametersIn(), fs.getParametersOut(), true);
			auto &inParams = fs.getParametersIn();
			auto &outParams = fs.getParametersOut();

			codeWriter.writeInstantiateParameters(cParamVariables);
			codeWriter.writeNewLine(2);

			for(int i = inParams.size() - 1; i >= 0; i--) {
				int luaIndex = -(inParams.size() - i);

				codeWriter.writeIfStatement
				(
					[&]{ // If type on stack is correct
						const CFunctionSpec &luaCheckTypeFunction = this->getLuaCheckTypeFunction(inParams[i].type);
						std::vector<std::string> args;
						args.push_back(bindingFunction.getInputParams()[0].getName());
						args.push_back(std::to_string(luaIndex));
						codeWriter.writeFunctionCall(luaCheckTypeFunction, args, false);
					},
					[&]{ // Get valueFromStack
						const CFunctionSpec &luaGetFunction = this->getLuaToFunction(inParams[i]);
						std::vector<std::string> args;
						args.push_back(bindingFunction.getInputParams()[0].getName());
						args.push_back(std::to_string(luaIndex));
						codeWriter.writeVariableAssignment(inParams[i].name, luaGetFunction, args);
					}
				);
				codeWriter.writeNewLine();
				codeWriter.writeElseStatement(
					[&](){ // If type was incorrect - throw error
						const CFunctionSpec &luaLErrorFunction = this->getLuaLErrorFunction();
						std::vector<std::string> args;
						args.push_back(bindingFunction.getInputParams()[0].getName());
						args.push_back(
							"\"Error: Wrong type of parameter " + inParams[i].name + "! " +
							"Expected type was " + parameterTypeToLuaParameterStringMap().at(inParams[i].type) +
							"\""
						);

						codeWriter.writeReturnStatement([&]{
							codeWriter.writeFunctionCall(luaLErrorFunction, args, false);
						});
					}
				);
				codeWriter.writeNewLine(2);
			}
			std::string functionPointerName = generateFunctionPointerVariableName(fs.getName());

			codeWriter.writeIfStatement(
				[&]{
					codeWriter.writeComparisonOperation(
							[&]{ codeWriter.writeIdentifier(functionPointerName); },
							[&]{ codeWriter.writeNull(); },
							CodeWriter::ComparisonOperator::NotEqual
					);
				},
				[&]{
					codeWriter.writeIfStatement(
						[&]{
							codeWriter.writeComparisonOperation( // if function pointer call returns 0
								[&]{
									std::vector<std::string> args;
									for(auto &inParam : inParams) {
										args.push_back(inParam.name);
									}

									for(auto &outParam : outParams) {
										args.push_back("&" + outParam.name);
									}

									codeWriter.writeFunctionPointerCall(generateFunctionPointerVariableName(fs.getName()), args, false);
								},
								[&]{ codeWriter.writeNull(); },
								CodeWriter::ComparisonOperator::Equal
							);

						},
						[&]{ // return luaL_error;
							std::vector<std::string> args;
							args.push_back(bindingFunction.getInputParams()[0].getName());
							args.push_back("\"Runtime error: " + fs.getName() + " failed for some reason.\"");
							codeWriter.writeFunctionCall(getLuaLErrorFunction(), args);
						}
					);
				}
			);

			codeWriter.writeNewLine(2);

			for(auto &param : outParams) {
				const auto &pushFunction = getLuaPushFunction(getCParameterTypeTranslation(param, false));
				std::vector<std::string> args;
				args.push_back(bindingFunction.getInputParams()[0].getName());
				args.push_back(param.name);
				codeWriter.writeFunctionCall(pushFunction, args);
				codeWriter.writeNewLine();
			}

			codeWriter.writeReturnStatement([&outParams, &codeWriter]() {
				codeWriter.writeIntegerLiteral(outParams.size());
			});
			/*
			std::unordered_map<LuaParameter::Type, std::string> 
				parameterTypeStrings = parameterTypeToCParameterStringMap();

			std::unordered_map<LuaParameter::Type, std::string>
				luaParameterTypes = parameterTypeToLuaParameterStringMap();

			std::unordered_map<LuaParameter::Type, std::string> parameterCheckTypeFunctions = getParameterTypeCheckFunctions();
			std::unordered_map<LuaParameter::Type, std::string> parameterValueFunctions = getParameterValueFunctions();

			std::unordered_map<LuaParameter::Type, std::string> luaPushValueFunctions;
			luaPushValueFunctions[LuaParameter::Type::Number] = "lua_pushnumber";
			luaPushValueFunctions[LuaParameter::Type::String] = "lua_pushstring";
			luaPushValueFunctions[LuaParameter::Type::UserData] = "lua_pushlightuserdata";

			// Generates code checking the types for the in parameters on the lua stack
			// as well as retriving them if the type is correct
			for (int i = inParams.size() - 1; i >= 0; i--) {
				
				// TODO: Remove this and fix instead


				int luaIndex = -(inParams.size() - i);
				if(inParams[i].type == LuaParameter::Type::Table) {
					stringStream << "\t" << "if(lua_istable(L, " << luaIndex << ")) {" << std::endl;
					stringStream << "\t\t" << "if(!" << generateCustomToFunctionName(inParams[i].typeName) << "(L, " << luaIndex << ", &" << inParams[i].name << ")) {" << std::endl;
					stringStream << "\t\t\t" << "return luaL_error(L, \"Error loading custom type " << inParams[i].typeName << "\");" << std::endl;
					stringStream << "\t\t" << "}" << std::endl;
					stringStream << "\t" << "}" << std::endl;
				}
				else {
					writeIfTypeCorrectGet(inParams[i], luaIndex, stringStream, 1);
				}
				// Generates the code that determines what happens if a parameter of an incorrect
				// type was passed to the runtime.
				stringStream << "\t" << "else {" << std::endl;
				stringStream << "\t" << "\t" << "return luaL_error(L, \"Incorrect type for parameter " <<
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
			*/
		});
		codeWriter.writeNewLine();
	}

	//writeBindingImplementations(functionSpecifications, stream);
	//stream << std::endl;

	codeWriter.writeFunctionImplementation(getRegisterModuleFunction(), [&, this](){
		auto &luaRegisterFunction = this->getLuaRegisterFunction();
		for(auto &fs : functionSpecifications) {
			std::vector<std::string> args;

			args.push_back(getRegisterModuleFunction().getInputParams()[0].getName());
			args.push_back('\"' + fs.getName() + "\"");
			args.push_back('\"' + generateBindingFunctionName(fs.getName() + '\"'));

			codeWriter.writeFunctionCall(luaRegisterFunction, args);
			
			if(&fs != &functionSpecifications.back()) {
				codeWriter.writeNewLine();
			}
		}
	});

	return stream.str();
}