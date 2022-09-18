#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>

class Parameter {

	public:
		enum class Type {
			Number,
			String,
			UserData
		};

		std::string name;
		Type type;

		Parameter(const std::string& name, Type type)
			: name(name), type(type) {}
};
#endif