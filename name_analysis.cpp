#include "ast.hpp"
#include "symbol_table.hpp"
#include "err.hpp"
#include <iostream>

namespace lake{

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	//  throw new ToDoError("This function should have"
	//  	"been overriden in the subclass!");
	return true;
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	symTab->AddScope();
	nameAnalysisOk = this->myDeclList->nameAnalysis(symTab);
	// add validity check
	symTab->DropScope();
	return nameAnalysisOk;
}

bool DeclListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
std::cout<<"\nDeclList Node Postition: "<<this->getPosition()<<"\n";
	for (auto decl : *myDecls){
		nameAnalysisOk = decl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}


bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
std::cout<<"\nVarDecl Node Postition: "<<this->getPosition()<<"\n";
	SemSymbol * symbol = new SemSymbol();
	symbol->SetId(myID->getString());
	symbol->SetType(myType->getType());	
	symbol->SetKind("VarDecl");

	// error check
	if(myType->getType() == "void") {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Non-function declared void");
		return false;
	}

	return symTab->AddSymbol(myID->getString(), symbol);
}

bool VarDeclListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
std::cout<<"\nVarDeclList Node Postition: "<<this->getPosition()<<"\n";
	for (auto varDecl : *myDecls) {
		nameAnalysisOk = varDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
std::cout<<"\nFnDecl Node Postition: "<<this->getPosition()<<"\n";
	bool first = true;
	std::string type;
	SemSymbol * symbol = new SemSymbol();
	if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
		return false;
	}

	for (auto formal : *myFormals->GetFormals()) {
		if (first) { 
			first = false; 
		}
		else {
			 type += ","; 
		}
		type += formal->getType() + ",";
	}
	// type += "->" + getReturnTypeNode()->getType();
	type += "->" + myRetAST->getType();
	std::cout<<"\nFnDecl Return Type: "<<myRetAST->getType()<<"\n";

	symbol->SetType(type);
	symbol->SetKind("FnDecl");
	symbol->SetId(myID->getString());

	symTab->AddSymbol(myID->getString(), symbol);

	symTab->AddScope();
	myFormals->nameAnalysis(symTab);
	myBody->nameAnalysis(symTab);
	symTab->DropScope();
	// return nameAnalysisOk;
	return true;
}

bool FormalsListNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nFormalsList Node Postition: "<<this->getPosition()<<"\n";
	for (auto *formalDecl: *myFormals) {
		nameAnalysisOk = formalDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab) {
	// bool nameAnalysisOk = true;
std::cout<<"\nFormalDecl Node Postition: "<<this->getPosition()<<"\n";
	// SemSymbol * symbol = new SemSymbol();
	// symbol->SetType(myType->getType());
	// symbol->SetId(myID->getString());
	// if(myType->getType() == "void") {
	// 	Err::semanticReport(myID->getLine(), myID->getCol(), "Non-function declared void");
	//	return false;
	// }
	// if(symTab->LookUp(myID->getString())) {
	// 	Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
	//	return false;
	// }
	// else {
	// 	nameAnalysisOk = symTab->AddSymbol(myID->getString(), symbol);
	// }

	// return nameAnalysisOk;
	if(myType->getType() == "void")
	{
		Err::semanticReport(myID->getLine(),myID->getCol(), "Non-function declared void");
		return false;
	}
	return true;
}

bool StmtListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
std::cout<<"\nStmtList Node Postition: "<<this->getPosition()<<"\n";
	for(auto stmt : *myStmts) {
		nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FnBodyNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nFnBody Node Postition: "<<this->getPosition()<<"\n";
	myVarDecls->nameAnalysis(symTab);
	myStmtList->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool IdNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nId Node Postition: "<<this->getPosition()<<"\n";
	if(!symTab->LookUp(myStrVal)) {
		Err::semanticReport(getLine(), getCol(), "Undeclared identifier");
		return false;
	}
	else
	{
		myStrVal += "(" + symTab->GetTable(myStrVal)->GetType(myStrVal) + ")";
	}
	
	return nameAnalysisOk;
}

bool AssignNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nAssign Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myTgt->nameAnalysis(symTab)
					&& mySrc->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool ExpListNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nExpList Node Postition: "<<this->getPosition()<<"\n";
	for(ExpNode * expr : *myExps)
	{
		nameAnalysisOk = expr->nameAnalysis(symTab) && nameAnalysisOk;	
	}
	
	return nameAnalysisOk;
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myAssign->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool ReadStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool WriteStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	symTab->AddScope();
	nameAnalysisOk = myDecls->nameAnalysis(symTab) 
				&& myStmts->nameAnalysis(symTab) 
				&& nameAnalysisOk; 
	symTab->DropScope();
	return nameAnalysisOk;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
		nameAnalysisOk = myExp->nameAnalysis(symTab);
	symTab->AddScope();
	nameAnalysisOk = myDeclsT->nameAnalysis(symTab) 
				&& myStmtsT->nameAnalysis(symTab) 
				&& nameAnalysisOk;
	symTab->DropScope();
	symTab->AddScope();
	nameAnalysisOk = myDeclsF->nameAnalysis(symTab) 
			    && myStmtsF->nameAnalysis(symTab)
				&& nameAnalysisOk;
	return nameAnalysisOk;
}

bool WhileStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	symTab->AddScope();
	nameAnalysisOk = myDecls->nameAnalysis(symTab) 
				&& myStmts->nameAnalysis(symTab) 
				&& nameAnalysisOk; 
	symTab->DropScope();
	return nameAnalysisOk;
}

bool CallExpNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
std::cout<<"\nNode Postition: "<<this->getPosition()<<"\n";
	std::cout<<"\nCallExp ID: "<<myId->getString()<<"\n";
	if(symTab->LookUp(myId->getString())) {
		nameAnalysisOk = myExpList->nameAnalysis(symTab);
	}
	else {
		//throw err
	}
	return nameAnalysisOk;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab) {
	if(myExp == nullptr) { return true; }
	return myExp->nameAnalysis(symTab);
}

}
