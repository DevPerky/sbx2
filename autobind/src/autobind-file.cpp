#include <exception>
#include <vector>
#include "function-spec.h"
#include "autobind-file.h"
#include <tinyxml2.h>
#include <unordered_map>
#include <iterator>

static bool elementNameEquals(const tinyxml2::XMLElement *element, const std::string& string) {
	return std::string(element->Value()).compare(string) == 0;
}

static Parameter parseParameterElement(const tinyxml2::XMLElement *parameterElement) {
	auto nameAttribute = parameterElement->FindAttribute("name");
	
	std::unordered_map<std::string, Parameter::Type> parameterTypes;

	parameterTypes["number"] = Parameter::Type::Number;
	parameterTypes["string"] = Parameter::Type::String;
	parameterTypes["userdata"] = Parameter::Type::UserData;


	if (!nameAttribute) {
		throw "parameter missing name";
	}

	const char *name = nameAttribute->Value();

	auto typeAttribute = parameterElement->FindAttribute("type");
	if (!typeAttribute) {
		throw "parameter missing type";
	}

	return Parameter(name, parameterTypes[typeAttribute->Value()]);
}

static std::vector<Parameter> parseParameterElements(const tinyxml2::XMLElement *root) {
	std::vector<Parameter> parameters;

	const tinyxml2::XMLElement *parameterElement = root->FirstChildElement("Parameter");

	while (parameterElement) {

		parameters.push_back(parseParameterElement(parameterElement));
		parameterElement = parameterElement->NextSiblingElement("Parameter");
	}

	return parameters;
}

static FunctionSpec parseFunctionElement(const tinyxml2::XMLElement *functionElement) {
	if (!elementNameEquals(functionElement, "FunctionSpec")) {
		throw "Wrong kind of element passed to function";
	}

	const char *name = functionElement->FindAttribute("name")->Value();
	std::vector<Parameter> parametersIn, parametersOut;
	const tinyxml2::XMLElement *element = 0;

	// Parse in elements
	// TODO: find a way around this code repetition 
	element = functionElement->FirstChildElement("In");
	if (element) {
		std::vector<Parameter> params = parseParameterElements(element);
		std::copy(params.begin(), params.end(), std::back_inserter(parametersIn));
	}

	// Parse out elements
	element = functionElement->FirstChildElement("Out");
	if (element) {
		std::vector<Parameter> params = parseParameterElements(element);
		std::copy(params.begin(), params.end(), std::back_inserter(parametersOut));
	}

	//Check for same name parameters - Illegal!
	bool doubleParams = false;
	for (const Parameter &pIn : parametersIn) {
		for (auto pOut : parametersOut) {
			if (pIn.name == pOut.name) {
				doubleParams = true;
				break;
			}
		}

		for (const Parameter &pIn2 : parametersIn) {
			if (pIn.name == pIn2.name && &pIn != &pIn2) {
				doubleParams = true;
				break;
			}
		}

		if (doubleParams) {
			break;
		}
	}

	if (doubleParams) {
		throw "Two parameters can't have the same name!";
	}

	return FunctionSpec(name, parametersIn, parametersOut);
}

static StructSpec parseStructElement(const tinyxml2::XMLElement *structElement) {
	if (!elementNameEquals(structElement, "StructSpec")) {
		throw "Wrong kind of element passed to function";
	}

	const char *name = structElement->FindAttribute("name")->Value();
	const tinyxml2::XMLElement *element = 0;

	std::vector<Parameter> members;
	// Parse member elements
	// TODO: find a way around this code repetition 
	element = structElement->FirstChildElement("Members");
	if (element) {
		std::vector<Parameter> members = parseParameterElements(element);
	}

	//Check for same name parameters - Illegal!
	bool doubleParams = false;
	for (const auto &member : members) {
		for (const auto &other : members) {
			if (member.name == other.name && &member != &other) {
				doubleParams = true;
				break;
			}
		}

		if(doubleParams) {
			break;
		}
	}

	if (doubleParams) {
		throw "Two parameters can't have the same name!";
	}

	return StructSpec(name, members);
}

AutoBindFile::AutoBindFile(
	std::string moduleName, 
	const std::vector<FunctionSpec> &functionSpecifications, 
	const std::vector<StructSpec> &structSpecifications
) : m_moduleName(moduleName),
		m_functionSpecifications(functionSpecifications),
		m_structSpecifications(structSpecifications) {


}
AutoBindFile AutoBindFile::LoadFromXML(std::string filePath) {
	tinyxml2::XMLDocument document;
	if (document.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
		throw "Could not load file";
	}
	
	const tinyxml2::XMLNode *root = document.RootElement();
	if (!root) {
		throw "No root element";
	}

	if (!elementNameEquals((tinyxml2::XMLElement*)root, "Bindings")) {
		throw "Root element must be <Bindings>";
	}
	const tinyxml2::XMLElement *element = (tinyxml2::XMLElement*)root;

	auto moduleNameAttribute = element->FindAttribute("module-name");
	if (!moduleNameAttribute) {
		throw "Root element missing attribute module-name";
	}

	element = element->FirstChildElement();
	std::vector<FunctionSpec> functionSpecifications;
	std::vector<StructSpec> structSpecifications;
	while(element) {
		if (elementNameEquals(element, "FunctionSpec")) {
			functionSpecifications.push_back(parseFunctionElement(element));
		}

		if (elementNameEquals(element, "StructSpec")) {
			structSpecifications.push_back(parseStructElement(element));
		}

		element = element->NextSiblingElement();
	}

	return AutoBindFile(moduleNameAttribute->Value(), functionSpecifications, structSpecifications);

}