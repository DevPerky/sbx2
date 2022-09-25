#include "autobind-file.hpp"
#include "binding-generator.hpp"
#include "definitions-generator.hpp"

#include <iostream>
#include <fstream>

int main(int argc, const char **argv) {
	
	if (argc <= 3) {

		std::cout << "[Autobind] error: not enough arguments supplied!" << std::endl;
	}

	std::string inputFile(argv[1]);
	std::string outputFolder(argv[2]);
	std::string luaDefinitionsFolder(argv[3]);

	std::string outputImplementationFile, outputInterfaceFile, outputLuaDefinitionsFile;

	try {
		auto autobindFile = AutoBindFile::LoadFromXML(inputFile);
		
		BindingGenerator bindingGenerator(autobindFile);
		DefinitionsGenerator definitionsGenerator(autobindFile);

		const std::string bindingInterface = bindingGenerator.generateBindingInterface();
		const std::string bindingImplementation = bindingGenerator.generateBindingImplementation();
		const std::string luaDefinitions = definitionsGenerator.generateDefinitions();

		std::string implementationFileName = outputFolder + "/" + bindingGenerator.getImplementationFileName();
		std::string interfaceFileName = outputFolder + "/" + bindingGenerator.getInterfaceFileName();
		std::string definitionsFileName = luaDefinitionsFolder + "/" + definitionsGenerator.getDefinitionsFileName();

		outputImplementationFile = implementationFileName;
		outputInterfaceFile = interfaceFileName;
		outputLuaDefinitionsFile = definitionsFileName;

		std::ofstream outStream;
		outStream.open(implementationFileName, std::ios::out);
		outStream.write(bindingImplementation.c_str(), bindingImplementation.size());
		outStream.close();

		outStream.open(interfaceFileName, std::ios::out);
		outStream.write(bindingInterface.c_str(), bindingInterface.size());
		outStream.close();

		outStream.open(definitionsFileName, std::ios::out);
		outStream.write(luaDefinitions.c_str(), luaDefinitions.size());
		outStream.close();
	}
	catch (std::exception e) {
		std::cout << "[Autobind] error: " << e.what();
	}

	std::cout << "[Autobind] " << inputFile << " -> " << outputInterfaceFile << 
		", " << outputImplementationFile << ", " << outputLuaDefinitionsFile << std::endl;
		
	return 0;
}