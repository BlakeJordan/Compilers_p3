#include "ast.hpp"
#include "symbol_table.hpp"
#include "err.hpp"

namespace lake{

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab) {
	// symTab->enterScope
	return this->myDeclList->nameAnalysis(symTab);
	// add validity check
	// smyTab->exitScope
}

bool DeclListNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	for (auto decl : *myDecls){
		result = decl->nameAnalysis(symTab) && result;
	}
	return result;
}

bool VarDeclListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	for (auto varDecl : *myDecls) {
		nameAnalysisOk = varDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool StmtListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	for(auto stmt : *myStmts) {
		nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	SemSymbol * symbol = new SemSymbol();
	symbol->SetId(myID->getString());
	symbol->SetType(myType->getType());	
	symbol->SetKind("VarDecl");

	// error check

	return symTab->AddSymbol(myID->getString(), symbol);
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	bool first = true;
	std::string type;
	SemSymbol * symbol = new SemSymbol();
	for (auto formal : *myFormals->GetFormals()) {
		if (first){ first = false; }
		else { type += ","; }
		type += formal->getType();
	}
	type += "->" + myRetAST->getType();
	symbol->SetType(type);
	symbol->SetKind("FnDecl");
	symbol->SetId(myID->getString());
	
	// if(symTab->findByName(getId()))
	// {
	// 	error checking
	// }
	// else
	// {
	// 	symTab->addItem(myID->getString(), symbol);
	// }

	symTab->AddScope();
	myFormals->nameAnalysis(symTab);
	myBody->nameAnalysis(symTab);
	symTab->DropScope();
	return nameAnalysisOk;
}
}
