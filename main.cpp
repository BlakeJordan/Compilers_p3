#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "scanner.hpp"
#include "symbol_table.hpp"

using namespace lake;

static void usageAndDie(){
	std::cerr << "Usage: lakec <infile>"
	<< " [-t <tokensFile>]"
	<< " [-p <unparseFile>]"
	<< " [-n <nameAnalysisFile>]"
	;
	exit(1);
}

static ProgramNode * parse(const char * inFile){
	std::ifstream inStream(inFile);
	if (!inStream.good()){
		std::string msg = "Bad input stream ";
		msg += inFile;
		throw new InternalError(msg.c_str());
	}

	lake::Scanner scanner(&inStream);
	ProgramNode * root = NULL;
	lake::Parser parser(scanner, &root);
	int errCode = parser.parse();
	if (errCode != 0){ return NULL; }

	return root;
}

static void writeTokenStream(const char * inPath, const char * outPath){
	std::ifstream inStream(inPath);
	if (!inStream.good()){
		std::string msg = "Bad input stream";
		msg += inPath;
		throw new InternalError(msg.c_str());
	}
	if (outPath == nullptr){
		std::string msg = "No tokens output file given";
		throw new InternalError(msg.c_str());
	}

	Scanner scanner(&inStream);
	if (strcmp(outPath, "--") == 0){
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()){
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
		outStream.close();
	}
}

static void unparse(ASTNode * astRoot, const char * outFile){
	if (outFile == nullptr){
		throw new InternalError("Null unparse file given");
	}
	if (strcmp(outFile, "--") == 0){
		astRoot->unparse(std::cout, 0);
	} else {
		std::ofstream outStream(outFile);
		astRoot->unparse(outStream, 0);
		outStream.close();
	}
}

int 
main( const int argc, const char **argv )
{
	if (argc == 0){
		usageAndDie();
	}
	const char * inFile = NULL;
	const char * tokensFile = NULL;
	const char * unparseFile = NULL;
	const char * nameAnalysisFile = NULL;
	bool doTypeChecking = false;
	const char * flattenFile = NULL;
	const char * outputFile = NULL;
	bool verbose = false;
	bool useful = false;
	int i = 1;
	for (int i = 1 ; i < argc ; i++){
		if (argv[i][0] == '-'){
			if (argv[i][1] == 't'){
				i++;
				tokensFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'p'){
				i++;
				unparseFile = argv[i];
				useful = true;
			} else if (argv[i][1] == 'n'){
				i++;
				nameAnalysisFile = argv[i];
				useful = true;
			} 
		} else {
			if (inFile == NULL){
				inFile = argv[i];
			} else {
				std::cerr << "Only 1 input file allowed";
				std::cerr << argv[i] << std::endl;
				usageAndDie();
			}
		}
	}
	if (inFile == NULL){
		usageAndDie();
	}
	if (!useful){
		std::cerr << "Whoops, you didn't tell lakec what to do!\n";
		usageAndDie();
	}

	int retCode = 0;

	if (tokensFile != NULL){
		try {
			writeTokenStream(inFile, tokensFile);
		} catch (InternalError * e){
			std::cerr << "Error: " << e->what() << std::endl;
		}
	}

	if (unparseFile != NULL){
		try {
			ASTNode * astRoot = parse(inFile);
			if (astRoot == NULL){
				std::cerr << "Parsing Error\n";
				exit(1);
			}
			unparse(astRoot, unparseFile);
		} catch (ToDoError * e){
			std::cerr << "ToDo: " << e->what() << std::endl;
			exit(1);
		}
	}
	
	if (nameAnalysisFile != NULL){
		try {
			ASTNode * astRoot = parse(inFile);
			if (astRoot == NULL){
				std::cerr << "Parsing Error\n";
				exit(1);
			}
			SymbolTable * symTab = new SymbolTable();
			if (astRoot->nameAnalysis(symTab)){
				unparse(astRoot, nameAnalysisFile);
			}
		} catch (ToDoError * e){
			std::cerr << "ToDo: " << e->what() << std::endl;
			exit(1);
		}
	}
	return retCode;
}
