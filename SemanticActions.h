#ifndef SEMANTIC
#define SEMANTIC
#include <iostream>
#include "Function.h"
extern std::stack<QBit> globalVarStack;
extern std::stack< Function* > functionStack;
extern std::list< Function* > allFunctions;
extern int globalConstID;

extern FILE *yyin;
extern int yylineno;
int yylex();
void yyerror(const char *s);

class FunctionSemantics
{
	public:
		//Function Semantic Actions
		static void CreateTable(std::string* name)
		{
			for(auto& fi : allFunctions)
			{
				if(fi->name == (*name))
				{
					printf("Duplicate definition of function %s\n",name->c_str());
					yyerror("Duplicate function\n");
					exit(0);
				}
			}
			
			functionStack.push(new Function(*name));
			printf("Created Function %s\n",name->c_str());
		}
		static void PopTable()
		{
			Function* f = functionStack.top();
			//printf("Popped function\n");
			allFunctions.push_back(f);
			printf("Added Function %s to list\n",f->name.c_str());
			functionStack.pop();
		}
		static void CreateFuncArgVars(GenericList* typeList, GenericList* nameList)
		{
			Function* f = functionStack.top();
			while(typeList != NULL && nameList != NULL)
			{
				AddSymbol(FuncVar(*((std::string*)nameList->data),(int)typeList->data));
				f->symbolTable->nargs++;
				typeList = typeList->next;
				nameList = nameList->next;
			}
			if(!(typeList == NULL && nameList == NULL))
			{
				printf("function %s has too many/too few arguments \n",f->name.c_str());
				yyerror("No matching argument type");
				exit(0);
			}
		}
		
		static void SetFuncFQC(FQC* funcFqc)
		{
			Function* f = functionStack.top();
			f->funcFQC = funcFqc;
			f->funcFQC->g = funcFqc->a + funcFqc->h - funcFqc->b;
		}
		
		
		static void SetReturnType(int size)
		{
			Function* f = functionStack.top();
			f->returnType = size;
			//printf("Func %s returns type of size %d\n",f->name.c_str(),size);
		}
		
		static void VerifyFuncName(std::string* name)
		{
			Function* f = functionStack.top();
			if(!(f->name == (*name)))
			{
				printf("Expected implementation of function %s\n",f->name.c_str());
				yyerror("Wrong Function Name\n");
				exit(0);
			}
		}
		
		static void AddSymbol(FuncVar var)
		{
			SymbolTable* st = functionStack.top()->symbolTable;
			if(!st->AddSymbol(var))
			{
				printf("Duplicate FuncVar %s in function %s\n",var.name.c_str(),functionStack.top()->name.c_str());
				yyerror("Duplicate symbol");
				exit(0);
			}
			//printf("Added Symbol %s of type size %d\n",var.name.c_str(),var.size);
			
		}
		static bool VerifySymbolExists(std::string* name)
		{
			if(!functionStack.top()->symbolTable->contains(*name))
			{
				printf("Unknown var %s \n",name->c_str());
				yyerror("Unknown var");
				exit(0);
				return false;
			}
			return true;
		}
		static void AddLetVar(std::string name,FQC* fqc)
		{
			SymbolTable* st = functionStack.top()->symbolTable;
			st->letvars.push_back(LetVar(name,*fqc)); // *fqc because it stores only a copy of the FQC, not a reference to it
		}
		static void RemoveLetContext()
		{
			SymbolTable* st = functionStack.top()->symbolTable;
			st->RemoveLetVar();
		}

		static void VerifyValidArgs(std::string* funcName, GenericList* varList)
		{
			Function* currentFunc = functionStack.top();
			Function* func = NULL;
			//Verify if func exists
			for(auto& f : allFunctions)
			{
				if(f->name == *funcName)
				{
					func = f;
				}
			}
			if(func == NULL)
			{
				printf("Unknown function %s\n",funcName->c_str());
				yyerror("Unknown function");
				exit(0);
			}
			int n = 0;
			//Verify correct number of args
			GenericList* genericList = varList;
			while(genericList != NULL)	{
				n++;
				printf("Var %s\n",((std::string*)genericList->data)->c_str());
				genericList = genericList->next;
			}
			if(n != func->symbolTable->nargs)
			{
				printf("Incorrect number of arguments (%d) on function %s call (has %d args)\n",n,func->name.c_str(),func->symbolTable->nargs);
				yyerror("Incorrent number of arguments");
				exit(0);
			}
			
			//Verify Arg types
			n = 0;
			while(varList != NULL)
			{
				std::string varName = *((std::string*)varList->data );
				FuncVar var = *(currentFunc->symbolTable->GetSymbol(varName));
				
				if(func->symbolTable->vars.at(n).size != var.size)
				{
					printf("Incorrect argument size (%d) on argument %d of function %s\n",var.size,n,func->name.c_str());
					yyerror("Wrong argument size");
				}
				n++;
				varList = varList->next;
			}
		}
		
		static void VerifyReturnType(FQC* ret)
		{
			Function* f = functionStack.top();
			if(ret->b != f->returnType)
			{
				printf("Function %s return FuncVar of size %s does not match its return type %s\n",f->name.c_str(),ret->b,f->returnType);
				yyerror("Function return type does not match declaration");
			}
		}
	
		
};
class SemanticOperations
{
	public:
		static FQC* OperationFuncCall(std::string* fname,GenericList* argList)
		{
			Function* fatherFunc = functionStack.top();
			Function* f = NULL;
			for(auto& func : allFunctions)
			{
				if(func->name == (*fname))
				{
					f =  func;
				}
			}
			if(f == NULL)
			{
				yyerror("Unkown function name");
				return NULL;
			}
			
			int i = 0;
			FQC* funcFQCCopy = new FQC(f->funcFQC->a,f->funcFQC->h,f->funcFQC->b,f->funcFQC->g,f->funcFQC->phi);
			funcFQCCopy->inputVars = f->funcFQC->inputVars;
			funcFQCCopy->auxVars = f->funcFQC->auxVars;
			while(argList != NULL)
			{
				std::string argName = *((std::string*)argList->data);
				std::string newVarName = fatherFunc->name; newVarName.append(argName);// Adds Function Prefix
				
				FuncVar correspondingVar = f->symbolTable->vars.at(i);
				std::string newCorrVarName = f->name;  newCorrVarName.append(correspondingVar.name);
				
				Variable* fqcVar = f->funcFQC->GetVar(newCorrVarName);
				fqcVar->aliases.push_back(newVarName); //Add alias
				
				argList = argList->next;
				i++;
			}
			return funcFQCCopy;
		}
		
		static FQC* OperationIF(FQC* c, FQC* t, FQC* f)
		{
			//Check for output type of c
			if(c->b != 1)
			{
				printf("Size of conditional operator different than 1 (is %d) \n",c->b);
				yyerror("Invalid condition size");
				exit(0);
			}
			//Check for size difference in output from t and e
			if(t->b != f->b)
			{
				printf("Difference in size of \"then\" and \"else\" operators  (%d and %d) \n",t->b,f->b);
				yyerror("If size mismatch");
				exit(0);
			}
			Function* func = functionStack.top();
			printf("CON IF\n");
			FQC* Con = MakeContextFQC(MakeContextFQC(c,t),f); // Con =  context for operations on then and else			
			Con->printFQC();
			
			printf("THEN\n");
			t->printFQC();
			
			printf("ELSE\n");
			f->printFQC();
			
			FQC* ifFQC = new FQC(Con->a, Con->h ,t->b); //a = a CF , h = h CF + h cF + h [t|u]F , b = b [t|u]F , g = g [t|u]F + g cF
			printf("CREATING CONDITION\n");
			c->CreateCondition(t,f,true);
			printf("CREATED CONDITION\n");
			ifFQC->phi = Con->phi;
			ifFQC->phi->PlusUnitary(c->phi);
			ifFQC->inputVars = c->inputVars;
			ifFQC->auxVars = c->auxVars;
			
			printf("IF FQC\n");
			ifFQC->printFQC();
			return ifFQC;
		}
		static FQC* OperationIFQ(FQC* c, FQC* t, FQC* f)
		{
			//Check for output type of c
			if(c->b != 1)
			{
				printf("Size of conditional operator different than 1 (is %d) \n",c->b);
				yyerror("Invalid condition size");
				exit(0);
			}
			//Check for size difference in output from t and e
			if(t->b != f->b)
			{
				printf("Difference in size of \"then\" and \"else\" operators  (%d and %d) \n",t->b,f->b);
				yyerror("Ifq size mismatch");
				exit(0);
			}
			
			Function* func = functionStack.top();
			//Apply ortho verification
			if(!VerifyOrtho(t->phi,f->phi))
			{
				printf("IFq returns not orthogonal\n");
				yyerror("Non orthogonal ifq returns");
				exit(0);
			}
			
			FQC* Con = MakeContextFQC(MakeContextFQC(c,t),f); // Con =  context for operations on then and else
			
			
			FQC* ifqFQC = new FQC(Con->a, Con->h,t->b); //a = a CF , h = h CF + h cF + h [t|u]F , b = b [t|u]F , g = g [t|u]F + g cF
			
			
			c->CreateCondition(t,f,false);
			
			ifqFQC->phi = c->phi;
			ifqFQC->inputVars = c->inputVars;
			ifqFQC->auxVars = c->auxVars;

			return ifqFQC;
		}
		static bool VerifyOrtho(Unitary* t, Unitary* f)
		{
			Rotation inl = Rotation();	inl.InitNot();
			Rotation inr = Rotation();	inr.InitIdentity();
			if(t->nextP == NULL && t->nextT == NULL && f->nextP == NULL && f->nextT == NULL)
			{
				if(t->type == Unitary::UType::Rot && f->type == Unitary::UType::Rot)
				{
					if(*(t->rot)== inl && *(f->rot)== inr)
						return true;
					if(*(f->rot)== inl && *(t->rot)== inr)
						return true;
				}
			}
			return false;
		}
		static FQC* OperationLET(std::string* x, FQC* t, FQC* u)
		{
			printf("LET\n");
			Function* f = functionStack.top();
			//	FQC* c = MakeContextFQC(t,u);
			//	FQC* letFQC = new FQC(c->a,c->h+t->h+u->h,u->b); // a = a cF , h = h cF + h tF + h uF , b = b uF , g = g tF + g uF
				
				
		    //	c->phi->PlusUnitary(u->phi);
			//	letFQC->phi = c->phi;
			FQC* letFQC = u;				
			letFQC->printFQC();
			return letFQC; //Returns that FQC
		}
		static FQC* OperationCONST(QBit* constValue)
		{
			constValue->Normalize();
			FQC* constFQC = new FQC(0,1,1,0,new ConstRot(constValue->left,constValue->right));
			std::string constName = std::string("CONST");
			constName.append(std::to_string(globalConstID));
			globalConstID++;
			constFQC->AddAuxVar(new Variable(constName,1));
			return constFQC;
		}
		static FQC* OperationVAR(std::string* varname)
		{
			Function* f = functionStack.top();
			FuncVar* v = f->symbolTable->GetSymbol(*varname); // Gets FuncVar in context
			if(v == NULL) // If var does not exist, it might be a let var, 
			{
				LetVar* lv = f->symbolTable->GetLetVar(*varname);
				if(lv != NULL)
				{
					FQC* varFQC = new FQC(lv->varFQC.a,lv->varFQC.h,lv->varFQC.b,lv->varFQC.g);
					varFQC->inputVars = lv->varFQC.inputVars; 
					varFQC->auxVars = lv->varFQC.auxVars;
					varFQC->phi = lv->varFQC.phi;
					return varFQC;
				}
				else
				{
					printf("Unknown var %s in function %s\n",varname->c_str(),f->name.c_str());
					yyerror("Unknown ID\n");
					exit(0);
				}
			}
			int size = v->size;
			IdentityRot* identity = new IdentityRot(size); 
			
			FQC* varFQC = new FQC(size,0,size,0,identity);
			std::string newVarName = f->name; newVarName.append(v->name);// Adds Function Prefix
			varFQC->AddInputVar(new Variable(newVarName,v->size));
			printf("Added var %s to context\n",varname->c_str());
			return varFQC; //Returns that FQC
		}
		//let x = t in u
		static void AddLetContext(std::string* x, FQC* t)
		{
			Function* f = functionStack.top();	
			if(t == NULL)
			{
				yyerror("Let Size Mismatch");
				exit(0);
			}
			//Makes x and alias of t`s output
			if(t->inputVars.size() == 1)
			{
				t->inputVars.at(0)->aliases.push_back(*x);
			}
			else if (t->auxVars.size() == 1)
			{
				t->auxVars.at(0)->aliases.push_back(*x);
			}
			FunctionSemantics::AddLetVar(*x,t);
			
		}
		static FQC* MakeContextFQC(FQC* gamma, FQC* delta)
		{
			std::vector<Variable*> gammaVars = gamma->inputVars;
			std::vector<Variable*> deltaVars = delta->inputVars;
			
		
			FQC* contextFQC = new FQC(0,0,0,0);
			std::vector<Variable*> permutationVars;
			std::vector<Variable*> copyVars;	
			//Add all gamma vars
			for(auto gVar : gammaVars){ contextFQC->AddInputVar(gVar);	permutationVars.push_back(gVar); } 
			
			//Check for conflicts
			Unitary* copies = NULL;
			for(auto dVar : deltaVars)
			{
				bool conflict = false;
				std::string copyName = std::string("Copy of ");
				for(auto pVar : permutationVars)
				{
					if((*dVar) == (*pVar))
					{
						copyName.append(pVar->name);
						delta->RenameVar(dVar->name,copyName);
						copyVars.push_back(dVar);
						contextFQC->AddAuxVar(dVar);
						if(copies == NULL)
						{
							copies = new CopyOp(pVar,dVar);
						}
						else
						{
							copies->PlusUnitary(new CopyOp(pVar,dVar));
						}
						conflict = true;
						break;
					}
				}
				if(!conflict ) contextFQC->AddInputVar(dVar);
				permutationVars.push_back(dVar);
			
			}
			//This vector exists to add aux vars to the permutation, since they dont need duplicate check
			std::vector<Variable*> varsToPermutate;
			//Gamma vars
			for(int i = 0; i < gamma->inputVars.size();i++)
				varsToPermutate.push_back(permutationVars.at(i));
			for(int i = 0; i < gamma->auxVars.size();i++)
			{
				varsToPermutate.push_back(gamma->auxVars.at(i));
				contextFQC->AddAuxVar(gamma->auxVars.at(i));
			}
			int gammaSize = gamma->inputVars.size();
			//Delta vars
			for(int i = 0; i < delta->inputVars.size();i++)
				varsToPermutate.push_back(permutationVars.at(i+gammaSize));
			for(int i = 0; i < delta->auxVars.size();i++)
			{
				varsToPermutate.push_back(delta->auxVars.at(i));
				contextFQC->AddAuxVar(delta->auxVars.at(i));
			}
			
			//Gera FQC do contexto
			int naux = varsToPermutate.size() - permutationVars.size();
			contextFQC->a = permutationVars.size() - copyVars.size();
			contextFQC->b = varsToPermutate.size();
			contextFQC->h = naux+copyVars.size();
			contextFQC->phi = new PermOp(varsToPermutate);
			contextFQC->phi->PlusUnitary(copies);
						
			return contextFQC;
		}	
};
#endif
