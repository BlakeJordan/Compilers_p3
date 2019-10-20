#include "ast.hpp"
#include "symbol_table.hpp"
#include "err.hpp"
#include <iostream>

namespace lake{

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	 throw new ToDoError("This function should have been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	symTab->AddScope();
	nameAnalysisOk = this->myDeclList->nameAnalysis(symTab);
	symTab->DropScope();
	return nameAnalysisOk;
}

bool DeclListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	for (auto decl : *myDecls){
		nameAnalysisOk = decl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}


bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	SemSymbol * symbol = new SemSymbol();
	int deref = myType->getPtrDepth();
	std::string derefs = "";
	for(int i = 0; i < deref; i++) {
		derefs += "@";
	}
	symbol->SetId(myID->getString());
	symbol->SetType(myType->getType() + derefs);	
	symbol->SetKind("VarDecl");

	if(myType->getType() == "void") {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Non-function declared void");
		nameAnalysisOk = false;
	}
	if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
		nameAnalysisOk = false;
	}
	nameAnalysisOk = symTab->AddSymbol(myID->getString(), symbol) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool VarDeclListNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	for (auto varDecl : *myDecls) {
		nameAnalysisOk = varDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	bool first = true;
	std::string type;
	SemSymbol * symbol = new SemSymbol();
	if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
		nameAnalysisOk = false;
	}
	for (auto formal : *myFormals->GetFormals()) {
		if (first) { 
			first = false; 
		}
		else {
			 type += ","; 
		}
		int deref = formal->getTypeNode()->getPtrDepth();
		std::string derefs = "";
		for(int i = 0; i < deref; i++) {
			derefs += "@";
		}
		type += formal->getTypeNode()->getType() + derefs;
	}
	int deref = myRetAST->getPtrDepth();
	if(myRetAST->getType() == "void" && deref > 0) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Invalid pointer type");
		nameAnalysisOk = false;
	} else {
		type += "->" + myRetAST->getType();
	}

	symbol->SetType(type);
	symbol->SetKind("FnDecl");
	symbol->SetId(myID->getString());

	symTab->AddSymbol(myID->getString(), symbol);

	symTab->AddScope();
	nameAnalysisOk = myFormals->nameAnalysis(symTab) && nameAnalysisOk;
	nameAnalysisOk = myBody->nameAnalysis(symTab) && nameAnalysisOk;
	symTab->DropScope();
	return nameAnalysisOk;
}

bool FormalsListNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	for (auto *formalDecl: *myFormals) {
		nameAnalysisOk = formalDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	SemSymbol * symbol = new SemSymbol();
	int deref = myType->getPtrDepth();
	std::string derefs = "";
	for(int i = 0; i < deref; i++) {
		derefs += "@";
	}
	symbol->SetType(myType->getType() + derefs);
	symbol->SetId(myID->getString());
	if(myType->getType() == "void")
	{
		Err::semanticReport(myID->getLine(),myID->getCol(), "Non-function declared void");
		nameAnalysisOk = false;
	}
	else if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
		nameAnalysisOk = false;
	}
	else{
		nameAnalysisOk = symTab->AddSymbol(myID->getString(), symbol) && nameAnalysisOk;
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

bool FnBodyNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myVarDecls->nameAnalysis(symTab)
					&& myStmtList->nameAnalysis(symTab)
					&& nameAnalysisOk;
	return nameAnalysisOk;
}

bool IdNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	if(!symTab->LookUp(myStrVal)) {
		Err::semanticReport(getLine(), getCol(), "Undeclared identifier");
		nameAnalysisOk = false;
	}
	else {
		myStrVal += "(" + symTab->GetTable(myStrVal)->GetType(myStrVal) + ")";
	}
	
	return nameAnalysisOk;
}

bool AssignNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myTgt->nameAnalysis(symTab)
					&& mySrc->nameAnalysis(symTab)
					&& nameAnalysisOk;
	return nameAnalysisOk;
}

bool ExpListNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	for(ExpNode * expr : *myExps)
	{
		nameAnalysisOk = expr->nameAnalysis(symTab) && nameAnalysisOk;	
	}
	
	return nameAnalysisOk;
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myAssign->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool ReadStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool WriteStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab) && nameAnalysisOk;
	symTab->AddScope();
	nameAnalysisOk = myDecls->nameAnalysis(symTab) 
				&& myStmts->nameAnalysis(symTab) 
				&& nameAnalysisOk; 
	symTab->DropScope();
	return nameAnalysisOk;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab) && nameAnalysisOk;
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
	nameAnalysisOk = myExp->nameAnalysis(symTab) && nameAnalysisOk;
	symTab->AddScope();
	nameAnalysisOk = myDecls->nameAnalysis(symTab) 
				&& myStmts->nameAnalysis(symTab) 
				&& nameAnalysisOk; 
	symTab->DropScope();
	return nameAnalysisOk;
}

bool CallStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myCallExp->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool CallExpNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;

	if(symTab->LookUp(myId->getString())) {
		nameAnalysisOk = myId->nameAnalysis(symTab) && nameAnalysisOk;
		nameAnalysisOk = myExpList->nameAnalysis(symTab) && nameAnalysisOk;
	}
	else {
		Err::semanticReport(getLine(), getCol(), "Undeclared identifier");
		nameAnalysisOk = false;
	}
	return nameAnalysisOk;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab) {
	if(myExp == nullptr) { return true; }
	return myExp->nameAnalysis(symTab);
}

bool DerefNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myTgt->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

}
