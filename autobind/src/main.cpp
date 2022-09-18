#include "autobind-file.h"
#include "binding-generator.h"

#include <iostream>
#include <fstream>

int main(int argc, const char **argv) {
	
	if (argc <= 2) {

		std::cout << "[Autobind] error: not enough arguments supplied!" << std::endl;
	}

	std::string inputFile(argv[1]);
	std::string outputFolder(argv[2]);

	std::string outputImplementationFile, outputInterfaceFile;

	try {
		auto autobindFile = AutoBindFile::LoadFromXML(inputFile);
		
		BindingGenerator generator(autobindFile);
		const std::string bindingInterface = generator.generateBindingInterface();
		const std::string bindingImplementation = generator.generateBindingImplementation();

		//std::cout << "Binding interface:" << std::endl << bindingInterface << std::endl;
		//std::cout << "Binding implementation:" << std::endl << bindingImplementation << std::endl;

		std::string implementationFileName = outputFolder + "/" + generator.getImplementationFileName();
		std::string interfaceFileName = outputFolder + "/" + generator.getInterfaceFileName();

		outputImplementationFile = implementationFileName;
		outputInterfaceFile = interfaceFileName;

		std::ofstream outStream;
		outStream.open(implementationFileName, std::ios::out);
		outStream.write(bindingImplementation.c_str(), bindingImplementation.size());
		outStream.close();

		outStream.open(interfaceFileName, std::ios::out);
		outStream.write(bindingInterface.c_str(), bindingInterface.size());
		outStream.close();
	}
	catch (std::exception e) {
		std::cout << "[Autobind] error: " << e.what();
	}

	std::cout << "[Autobind] " << inputFile << " -> " << outputInterfaceFile << 
		", " << outputImplementationFile<< std::endl;
		
	return 0;
}