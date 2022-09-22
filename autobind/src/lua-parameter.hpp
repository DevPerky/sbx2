#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>

class LuaParameter {

	public:
		enum class Type {
			Number,
			String,
			UserData,
			Table
		};

		std::string name;
		std::string typeName;
		Type type;

		LuaParameter(const std::string &name, Type type, const std::string &typeString)
			: name(name), type(type), typeName(typeString) {}
};
#endif