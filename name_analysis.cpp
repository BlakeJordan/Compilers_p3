#include "ast.hpp"
#include "symbol_table.hpp"
#include "err.hpp"
#include <iostream>

namespace lake{

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	 throw new ToDoError("This function should have been overriden in the subclass!");
	// std::cout<<"\nNode position: "<<
	return true;
}

// bool ExpNode::nameAnalysis(SymbolTable * symTab){
// 	throw new ToDoError("EXP NODE: This function should have been overriden in the subclass!");
// 	// std::cout<<"\nNode position: "<<
// 	return true;
// }

bool ProgramNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	symTab->AddScope();
	nameAnalysisOk = this->myDeclList->nameAnalysis(symTab);
	// add validity check
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
	symbol->SetId(myID->getString());
	symbol->SetType(myType->getType());	
	symbol->SetKind("VarDecl");
	std::cout<<"\nVarDecl Node Postition: "<<this->getPosition();
	std::cout<<"\nID: "<<symbol->GetId();
	std::cout<<"\nType: "<<symbol->GetType()<<"\n";

	if(myType->getType() == "void") {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Non-function declared void");
		nameAnalysisOk = false;
	}
	if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "")
	}


	return symTab->AddSymbol(myID->getString(), symbol);
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
		type += formal->getTypeNode()->getType();
	}
	// type += "->" + getReturnTypeNode()->getType();
	type += "->" + myRetAST->getType();
	std::cout<<"\nFnDecl Node Postition: "<<this->getPosition()<<"\n";
	std::cout<<"Type: "<<type<<"\n";

	symbol->SetType(type);
	symbol->SetKind("FnDecl");
	symbol->SetId(myID->getString());

	symTab->AddSymbol(myID->getString(), symbol);

	symTab->AddScope();
	nameAnalysisOk = myFormals->nameAnalysis(symTab);
	nameAnalysisOk = myBody->nameAnalysis(symTab);
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
	// bool nameAnalysisOk = true;
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
	std::cout<<"\nFormalDecl Node Postition: "<<this->getPosition()<<"\n";
	std::cout<<"Type: "<<myType->getType()<<"\n";
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
	for(auto stmt : *myStmts) {
		nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	}
	return nameAnalysisOk;
}

bool FnBodyNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	std::cout<<"\nFnBody Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myVarDecls->nameAnalysis(symTab);
	nameAnalysisOk = myStmtList->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool IdNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	std::cout<<"\nId Node Postition: "<<this->getPosition()<<"\n";
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
	std::cout<<"\nAssign Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myTgt->nameAnalysis(symTab)
					&& mySrc->nameAnalysis(symTab)
					&& nameAnalysisOk;
	return nameAnalysisOk;
}

bool ExpListNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	for(ExpNode * expr : *myExps)
	{
		std::cout<<"\nExp node position: "<<expr->getPosition()<<"\n";
		nameAnalysisOk = expr->nameAnalysis(symTab) && nameAnalysisOk;	
	}
	
	return nameAnalysisOk;
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myAssign->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool ReadStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	std::cout<<"\nRead Stmt Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool WriteStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	std::cout<<"\nWrite Stmt Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	std::cout<<"\nIfStmt Node Postition: "<<this->getPosition()<<"\n";
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
	std::cout<<"\nIfElseStmt Node Postition: "<<this->getPosition()<<"\n";
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
	std::cout<<"\nWhileStmt Node Postition: "<<this->getPosition()<<"\n";
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	symTab->AddScope();
	nameAnalysisOk = myDecls->nameAnalysis(symTab) 
				&& myStmts->nameAnalysis(symTab) 
				&& nameAnalysisOk; 
	symTab->DropScope();
	return nameAnalysisOk;
}

bool CallStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myCallExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool CallExpNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;

	std::cout<<"\nCallExp Node Postition: "<<this->getPosition()<<"\n";
	std::cout<<"ID: "<<myId->getString()<<"\n";

	if(symTab->LookUp(myId->getString())) {
		nameAnalysisOk = myId->nameAnalysis(symTab) && nameAnalysisOk;
		nameAnalysisOk = myExpList->nameAnalysis(symTab) && nameAnalysisOk;
	}
	else {
		Err::semanticReport(getLine(), getCol(), "Non-function declared void");
		nameAnalysisOk = false;
	}
	return nameAnalysisOk;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab) {
	std::cout<<"\nReturnStmt Node Postition: "<<this->getPosition()<<"\n";
	if(myExp == nullptr) { return true; }
	return myExp->nameAnalysis(symTab);
}

}
