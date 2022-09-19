#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>

class Parameter {

	public:
		enum class Type {
			Number,
			String,
			UserData,
			Custom
		};

		std::string name;
		std::string typeName;
		Type type;

		Parameter(const std::string &name, Type type, const std::string &typeString)
			: name(name), type(type), typeName(typeString) {}
};
#endif