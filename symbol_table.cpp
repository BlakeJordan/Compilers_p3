#include "symbol_table.hpp"
namespace lake{

	ScopeTable::ScopeTable(){
		symbols = new HashMap<std::string, SemSymbol *>();
	}

	bool ScopeTable::LookUp(std::string id) {
		return(symbols->count(id) > 0);
	}

	std::string ScopeTable::GetType(std::string id) {
		return symbols->find(id)->second->GetType();
	}

	bool ScopeTable::AddSymbol(std::string id, SemSymbol * symbol) {
		if (!LookUp(id)) {
			symbols->insert({id, symbol});
			return true;
		}
		else
		{
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

	void SymbolTable::AddScope() {
		scopeTableChain->push_front(new ScopeTable());
	}

	void SymbolTable::DropScope() {
		scopeTableChain->pop_front();
	}

	bool SymbolTable::LookUp(std::string id) {
		int tables = scopeTableChain->size();
		for(ScopeTable * scopeTable: * scopeTableChain) {
			if(scopeTable->LookUp(id)) {
				return true;
			}
 		}
		 return false;
	}

	bool SymbolTable::AddSymbol(std::string id, SemSymbol * symbol) {
		return scopeTableChain->front()->AddSymbol(id, symbol);
	}

	// get table containing id ???

	SemSymbol::SemSymbol(){}

	void SemSymbol::SetId(std::string id) {
		m_id = id;
	}

	std::string SemSymbol::GetId() {
		return m_id;
	}

	void SemSymbol::SetType(std::string type) {
		m_type = type;
	}

	std::string SemSymbol::GetType() {
		return m_type;
	}

	void SemSymbol::SetKind(std::string kind) {
		m_kind = kind;
	}

	std::string SemSymbol::GetKind() {
		return m_kind;
	}

}
