#ifndef LAKE_SYMBOL_TABLE_HPP
#define LAKE_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace lake{

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols 
// exist for the lifetime of a scope in the 
// symbol table. 
class SemSymbol {
	//TODO add the fields that 
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields

	public:
		SemSymbol();
		std::string GetId();
		void SetId(std::string id);
		std::string GetType();
		void SetType(std::string type);
		std::string GetKind();
		void SetKind(std::string kind);
		
	private:
		std::string m_id;
		std::string m_type;
		std::string m_kind;
};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
		std::string GetType(std::string id);
		bool AddSymbol(std::string id, SemSymbol * symbol);
		bool LookUp(std::string id);
	private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
		void AddScope();
		void DropScope();
		std::string GetType(std::string id);
		bool AddSymbol(std::string id, SemSymbol * symbol);
		bool LookUp(std::string id);
		ScopeTable* GetTable(std::string id);
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif
