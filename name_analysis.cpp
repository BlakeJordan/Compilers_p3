#include "ast.hpp"
#include "symbol_table.hpp"
#include "err.hpp"

namespace lake{

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	//  throw new ToDoError("This function should have"
	//  	"been overriden in the subclass!");
	return true;
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	symTab->AddScope();
	nameAnalysisOk = this->myDeclList->nameAnalysis(symTab);
	// add validity check
	symTab->DropScope();
	return nameAnalysisOk;
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
		// if (first) { 
		// 	first = false; 
		// }
		// else {
		// 	 type += ","; 
		// }
		type += formal->getType() + ",";
	}
	type += "->" + getReturnTypeNode()->getType();
	symbol->SetType(type);
	symbol->SetKind("FnDecl");
	symbol->SetId(myID->getString());
	
	if(symTab->LookUp(myID->getString()))
	{
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
	}
	else
	{
		symTab->AddSymbol(myID->getString(), symbol);
	}
	
	symTab->AddScope();
	myFormals->nameAnalysis(symTab);
	myBody->nameAnalysis(symTab);
	symTab->DropScope();
	return nameAnalysisOk;
}

bool FnBodyNode::nameAnalysis(SymbolTable * symTab)
{
	bool nameAnalysisOk = true;
	myVarDecls->nameAnalysis(symTab);
	myStmtList->nameAnalysis(symTab);
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
	symbol->SetType(myType->getType());
	symbol->SetId(myID->getString());
	if(myType->getType() == "void") {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Non-function declared void");
	}
	if(symTab->LookUp(myID->getString())) {
		Err::semanticReport(myID->getLine(), myID->getCol(), "Multiply declared identifier");
	}
	else {
		nameAnalysisOk = symTab->AddSymbol(myID->getString(), symbol);
	}

	return nameAnalysisOk;
}

bool IdNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	if(!symTab->LookUp(myStrVal)) {
		//Err::semanticReport(myID->getLine(), myID->getCol(), "Undeclared identifier");
	}
	else
	{
		myStrVal += "(" + symTab->GetTable(myStrVal)->GetType(myStrVal) + ")";
	}
	
	return nameAnalysisOk;
}

bool AssignNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myTgt->nameAnalysis(symTab)
					&& mySrc->nameAnalysis(symTab);
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
	nameAnalysisOk = myAssign->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool ReadStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool WriteStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
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
	if(symTab->LookUp(myId->getString())) {
		nameAnalysisOk = myExpList->nameAnalysis(symTab);
	}
	else {
		//throw err
	}
	return nameAnalysisOk;
}

}
