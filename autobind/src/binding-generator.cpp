#include "binding-generator.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <exception>

inline static const std::string generateFunctionPointerTypeName(const std::string &functionName) {
	return "AB_" + functionName;
}

inline static const std::string generateFunctionPointerVariableName(const std::string &functionName) {
	return "AB_" + functionName + "_binding";
}

inline static const std::string generateBindingFunctionName(const std::string &functionName) {
	return "l_" + functionName;
}

inline static const std::string generateCustomToFunctionName(const std::string &typeName) {
	return "AB_to_" + typeName;
}

inline static const std::string generateCustomPushFunctionName(const std::string &typeName) {
	return "AB_push_" + typeName;
}

const std::string BindingGenerator::getLuaTypeString(const LuaParameter &param) const {
	switch(param.type) {
		case LuaParameter::Type::Number: return "number";
		case LuaParameter::Type::String: return "string";
		case LuaParameter::Type::UserData: return "userdata";
		case LuaParameter::Type::Table: return "table";
		case LuaParameter::Type::Integer: return "integer";
	}
	return "error";
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
			"function",
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
	CParameter::Type::CType cType = CParameter::Type::CType::Invalid;
	int pointerLevels = out ? 1 : 0;
	std::string typeName = "";
	bool isConst = false;

	switch(param.type) {
		case LuaParameter::Type::Number:
			cType = CParameter::Type::CType::Double;
		break;

		case LuaParameter::Type::String:
			cType = CParameter::Type::CType::Char;
			isConst = true;
			pointerLevels += 1;
		break;

		case LuaParameter::Type::Integer:
			cType = CParameter::Type::CType::Int;
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

	return CParameter::Type(cType, typeName, pointerLevels, isConst);
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

const CParameter BindingGenerator::getBindingPointerVariable(const LuaFunctionSpec &functionSpec) const {
	auto &functionPointerSetterSpec = getFunctionPointerSetterFunction(functionSpec);

	return CParameter(
		generateFunctionPointerVariableName(functionSpec.getName()),
		CParameter::Type(
			CParameter::Type::CType::NonPrimitive,
			functionPointerSetterSpec.getInputParams()[0].getType().typeName,
			0
		)
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
		case LuaParameter::Type::Integer:
			functionName = "lua_isinteger";
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
	const auto returnType = getCParameterTypeTranslation(parameter, false);

	switch(parameter.type) {
		case LuaParameter::Type::Number:
			functionName = "lua_tonumber";
			break;
		case LuaParameter::Type::String:
			functionName = "lua_tostring";
			break;
		case LuaParameter::Type::UserData:
			functionName = "lua_touserdata";
			break;
		case LuaParameter::Type::Integer:
			functionName = "lua_tointeger";
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
		returnType,
		cParams,
		true
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
			if(cType.pointerLevels == 0) {
				functionName = "lua_pushinteger";
			}
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

const CStruct BindingGenerator::getCStruct(const StructSpec &structSpec) const {
	std::vector<CParameter> cParams;
	for(auto &param : structSpec.getMembers()) {
		CParameter::Type type = getCParameterTypeTranslation(param, false);
		cParams.push_back(CParameter(param.name, type));
	}

	return CStruct(
		structSpec.getName(),
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

const CFunctionSpec BindingGenerator::getLuaGetFieldFunction() const {
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

	cParams.push_back(
		CParameter(
			"k",
			CParameter::Type(CParameter::Type::CType::Char, "", 1)
		)
	);

	return CFunctionSpec(
		"lua_getfield",
		CParameter::Type(CParameter::Type::CType::Int, "", 0),
		cParams,
		false
	);
}

const CFunctionSpec BindingGenerator::getLuaPopFunction () const {
	std::vector<CParameter> cParams;

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"n",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	return CFunctionSpec(
		"lua_pop",
		CParameter::Type(CParameter::Type::CType::Void, "", 0),
		cParams,
		false
	);
}

const CFunctionSpec BindingGenerator::getLuaCreateTableFunction() const {
	std::vector<CParameter> cParams;

	cParams.push_back(
		CParameter(
			"L",
			CParameter::Type(CParameter::Type::CType::NonPrimitive, "lua_State", 1)
		)
	);

	cParams.push_back(
		CParameter(
			"narr",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	cParams.push_back(
		CParameter(
			"nrec",
			CParameter::Type(CParameter::Type::CType::Int, "", 0)
		)
	);

	return CFunctionSpec(
		"lua_createtable",
		CParameter::Type(CParameter::Type::CType::Void, "", 0),
		cParams,
		false
	);
}

const CFunctionSpec BindingGenerator::getLuaSetTableFunction() const {
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
		"lua_settable",
		CParameter::Type(CParameter::Type::CType::Void, "", 0),
		cParams,
		false
	);
}


const std::string BindingGenerator::generateBindingInterface() const {
	std::stringstream stream;

	const auto &structSpecifications = m_autoBindFile.getStructSpecifications();

	stream << "#include <lua.h>" << std::endl;
	CodeWriter codeWriter(stream);

	for(auto &sp : structSpecifications) {
		auto cStruct = getCStruct(sp);
		codeWriter.writeStruct(cStruct);
		codeWriter.writeNewLine(2);
	}

	codeWriter.writeFunctionPrototype(getRegisterModuleFunction());
	codeWriter.writeNewLine();

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

	auto &functionSpecifications = m_autoBindFile.getFunctionSpecifications();
	auto &structSpecifications = m_autoBindFile.getStructSpecifications();

	// Write binding function pointer variables
	for(auto &fs : functionSpecifications) {
		CParameter bindingVariable = getBindingPointerVariable(fs);
		codeWriter.writeVariableInstance(bindingVariable, true);
	}

	codeWriter.writeNewLine();

	// Write AB_to_X - function prototypes
	for(auto &sp : structSpecifications) {
		LuaParameter luaParam("", LuaParameter::Type::Table, sp.getName());
		auto &cFunctionSpec = getLuaToFunction(luaParam);
		codeWriter.writeFunctionPrototype(cFunctionSpec);
		codeWriter.writeNewLine();
	}

	// Write AB_push_X - function prototypes
	for(auto &sp : structSpecifications) {
		LuaParameter luaParam("", LuaParameter::Type::Table, sp.getName());
		auto &cParam = getCParameterTypeTranslation(luaParam, false);
		auto &cFunctionSpec = getLuaPushFunction(cParam);
		codeWriter.writeFunctionPrototype(cFunctionSpec);
		codeWriter.writeNewLine();
	}

	codeWriter.writeNewLine();

	// Write AB_to_x - function implementations
	for(auto &sp : structSpecifications) {
		LuaParameter luaParam("", LuaParameter::Type::Table, sp.getName());
		auto &cFunctionSpec = getLuaToFunction(luaParam);

		codeWriter.writeFunctionImplementation(cFunctionSpec, [&](){
			std::string resultName = "result";
			auto &luaGetField = getLuaGetFieldFunction();
			auto &cStruct = getCStruct(sp);
			codeWriter.writeVariableInstance(CParameter(resultName, cFunctionSpec.getReturnType()));
			codeWriter.writeNewLine();

			std::vector<std::string> getFieldArgs;
			getFieldArgs.resize(3);
			getFieldArgs[0] = cFunctionSpec.getInputParams()[0].getName();
			getFieldArgs[1] = cFunctionSpec.getInputParams()[1].getName();
			for(auto &member : sp.getMembers()) {
				getFieldArgs[2] = '\"' + member.name + "\"";
				codeWriter.writeFunctionCall(luaGetField, getFieldArgs, true);
				codeWriter.writeNewLine();

				auto &luaToFunction = getLuaToFunction(member);
				std::vector<std::string> toFunctionArgs;
				toFunctionArgs.resize(2);
				toFunctionArgs[0] = cFunctionSpec.getInputParams()[0].getName();
				toFunctionArgs[1] = "-1";
				
				codeWriter.writeVariableAssignment(resultName + "." + member.name, luaToFunction, toFunctionArgs);
				codeWriter.writeNewLine();
				std::vector<std::string> popFunctionArgs;
				popFunctionArgs.resize(2);
				popFunctionArgs[0] = cFunctionSpec.getInputParams()[0].getName();
				popFunctionArgs[1] = "1";

				auto &luaPopFunction = getLuaPopFunction();
				codeWriter.writeFunctionCall(luaPopFunction, popFunctionArgs, true);
				codeWriter.writeNewLine();
			}

			codeWriter.writeNewLine();

			codeWriter.writeReturnStatement([&codeWriter, &resultName](){
				codeWriter.writeIdentifier(resultName);
			});
		});
		codeWriter.writeNewLine(2);
	}

	// Write AB_push_X - function prototypes
	for(auto &sp : structSpecifications) {
		LuaParameter luaParam("", LuaParameter::Type::Table, sp.getName());
		auto &cParam = getCParameterTypeTranslation(luaParam, false);
		auto &cFunctionSpec = getLuaPushFunction(cParam);
		codeWriter.writeFunctionImplementation(cFunctionSpec, [&cFunctionSpec, &codeWriter, &sp, this]{
			auto &luaCreateTableFunction = getLuaCreateTableFunction();
			std::vector<std::string> createTableArgs(3);
			createTableArgs[0] = cFunctionSpec.getInputParams()[0].getName();
			createTableArgs[1] = "0";
			createTableArgs[2] = std::to_string(sp.getMembers().size());
			
			codeWriter.writeFunctionCall(luaCreateTableFunction, createTableArgs, true);
			codeWriter.writeNewLine(2);
			
			std::vector<std::string> pushStringArgs(2);
			std::vector<std::string> pushValueArgs(2);
			std::vector<std::string> setTableArgs(2);

			pushValueArgs[0] = cFunctionSpec.getInputParams()[0].getName();
			pushStringArgs[0] = cFunctionSpec.getInputParams()[0].getName();

			setTableArgs[0] = cFunctionSpec.getInputParams()[0].getName();
			setTableArgs[1] = "-3";

			auto &pushStringFunction = getLuaPushFunction(CParameter::Type(CParameter::Type::CType::Char, "", 1));
			auto &members = sp.getMembers();
			for(auto &member : members) {
				auto &pushValueFunction = getLuaPushFunction(getCParameterTypeTranslation(member, false));
				auto &setTableFunction = getLuaSetTableFunction();

				pushStringArgs[1] = '\"' + member.name + '\"';
				pushValueArgs[1] = cFunctionSpec.getInputParams()[1].getName() + "." + member.name;
				codeWriter.writeFunctionCall(pushStringFunction, pushStringArgs, true);
				codeWriter.writeNewLine();
				codeWriter.writeFunctionCall(pushValueFunction, pushValueArgs, true);
				codeWriter.writeNewLine();
				codeWriter.writeFunctionCall(setTableFunction, setTableArgs, true);
				if(&member != &members.back()) {
					codeWriter.writeNewLine(2);
				}
			}
		});
		codeWriter.writeNewLine(2);
	}

	// Write function pointer setter implementations
	for(auto &fs : functionSpecifications) {
		auto &functionPointerSetterSpec = getFunctionPointerSetterFunction(fs);
		CParameter bindingVariable = getBindingPointerVariable(fs);
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
							"\" Error: Wrong type of parameter " + inParams[i].name + "! " +
							"Expected type was " + getLuaTypeString(inParams[i]) + 
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
		});
		codeWriter.writeNewLine(2);
	}

	codeWriter.writeFunctionImplementation(getRegisterModuleFunction(), [&, this](){
		auto &luaRegisterFunction = this->getLuaRegisterFunction();
		for(auto &fs : functionSpecifications) {
			std::vector<std::string> args;

			args.push_back(getRegisterModuleFunction().getInputParams()[0].getName());
			args.push_back('\"' + fs.getName() + "\"");
			args.push_back(generateBindingFunctionName(fs.getName()));

			codeWriter.writeFunctionCall(luaRegisterFunction, args);
			
			if(&fs != &functionSpecifications.back()) {
				codeWriter.writeNewLine();
			}
		}
	});

	return stream.str();
}