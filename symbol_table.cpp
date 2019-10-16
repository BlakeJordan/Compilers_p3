#include "symbol_table.hpp"
namespace lake{

	ScopeTable::ScopeTable(){
		symbols = new HashMap<std::string, SemSymbol *>();
	}

	bool ScopeTable::LookUp(std::string id) {
		int count = symbols->count(id);
		if (count != 0) { 
			return true;
		} else {
			return false;
		}
	}

	SymbolTable::SymbolTable(){
		//TODO: implement the list of hashtables approach
		// to building a symbol table:
		// Upon entry to a scope a new scope table will be 
		// entered into the front of the chain and upon exit the 
		// latest scope table will be removed from the front of 
		// the chain.
		scopeTableChain = new std::list<ScopeTable *>();
	}

	void SymbolTable::AddScope(ScopeTable * newScope) {
		scopeTableChain->push_front(newScope);
	}

	void SymbolTable::DropScope() {
		scopeTableChain->pop_front();
	}

	SemSymbol::SemSymbol(std::string type) {
		m_type = type;
	}

	void SemSymbol::SetType(std::string type) {
		m_type = type;
	}

	std::string SemSymbol::GetType() {
		return m_type;
	}

}
