#ifndef EXTERNALMAIN
#define EXTERNALMAIN
#include <stdio.h>
#include <list>
#include <vector>
#include <stack>
#include <string>
#include "FQC.h"

//flex lexical.l && bison -dy sintatical.y
typedef struct GenericList
{
	intptr_t data;
	struct GenericList * next;
} GenericList;

class LetVar
{
	public:
		LetVar(std::string n, FQC fqc): name(n),varFQC(fqc) {}
		FQC varFQC;
		std::string name;
};
class FuncVar
{
	public:
		FuncVar(std::string n,int s): name(n),size(s){} 
		int size;
		std::string name;
};

class SymbolTable
{
	public:
		std::vector<FuncVar> vars;
		std::vector<LetVar> letvars;
		int nargs = 0;		
		bool AddSymbol(FuncVar v) 
		{ 
			if(contains(v.name))	return false;
			vars.push_back(v);	
			return true;
		}
		bool contains(std::string name)
		{
			for(auto& var : vars)
			{
				if(var.name == name) return true;
			}
			for(auto&var : letvars)
			{
				if(var.name == name) return true;
			}
			return false;
		}
		FuncVar* GetSymbol(std::string name)
		{
			for(auto& var : vars)
			{
				if(var.name == name) return &var;
			}
			return NULL;
		}
		LetVar* GetLetVar(std::string name)
		{
			for(auto& var : letvars)
			{
				if(var.name == name) return &var;
			}
			return NULL;
		}
		void RemoveLetVar()
		{
			letvars.pop_back();
		}
};
class Function
{
	public:
		Function(std::string str): name(str) {	symbolTable = new SymbolTable();}
		SymbolTable* symbolTable;
		std::string name;
		int returnType;
		FQC* funcFQC;
	
};
#endif

