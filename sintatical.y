%code requires {
   	#include "Function.h"
	#include "SemanticActions.h"
}
%{
#include "Function.h"
#include "SemanticActions.h"
#include <iostream>
#include <fstream>
extern FILE *yyin;
extern int yylineno;
std::stack< Function* > functionStack;
std::list< Function* > allFunctions;
int globalConstID = 0;
int yylex();
void yyerror(const char *s);

GenericList* NewElement(GenericList* list, intptr_t data)
{
	GenericList* newElement = new GenericList();
	newElement->data = data;
	newElement->next = list;
	return newElement;
}

std::string AddMeasures(std::string str,int nqubits)
{
	std::string finalstr;
	finalstr.append(str);
	for(int i = 0; i < nqubits; i++)
	{
		char buffer[25]; 
		sprintf(buffer,"Measure %d\n",i);	
		finalstr.append(buffer);
	}
	return finalstr;
}
void AttributeQubits(std::stack<QBit*> qubitStack,FQC* mainFQC)
{
	std::stack<QBit*> auxStack;
	printf("ATTRIBUTING %d qubits, %d input and %d aux\n",mainFQC->a+mainFQC->h,mainFQC->a,mainFQC->h);
	for(auto var : mainFQC->inputVars)
	{
		var->value = qubitStack.top();
		qubitStack.pop();
		printf("%s[0] = %d\n",var->name.c_str(),auxStack.size());
		auxStack.push(var->value);
		for(int i = 1; i < var->size; i++) // Accounts for variable sizes
		{
			auxStack.push(qubitStack.top());
			qubitStack.pop();
		}
	}
	for(auto var : mainFQC->auxVars)
	{
		var->value = qubitStack.top();
		qubitStack.pop();
		printf("%s[0] = %d\n",var->name.c_str(),auxStack.size());
		auxStack.push(var->value);
		for(int i = 1; i < var->size; i++) // Accounts for variable sizes
		{
			auxStack.push(qubitStack.top());
			qubitStack.pop();
		}
	}
	//Restores Original Stack
	while(!auxStack.empty()) { qubitStack.push(auxStack.top()); auxStack.pop(); }
}
void CompileMain(FQC* mainFQC)
{
	//Creates Output file
	std::ofstream  outputfile;
	outputfile.open("circuit.txt");
	if(!outputfile) { printf(" ERROR OPENING circuit.txt\n"); }
	
	//Inputs Header
	int totalSize =  mainFQC->a + mainFQC->h;
	outputfile << (totalSize) << std::endl;
	
	//Creates Qubit stack
	std::stack<QBit*> qubitStack;
	for(int i = 0; i < totalSize; i++)
	{
		qubitStack.push(new QBit());
	}
	//printf("MAIN FQC =\n");
	//mainFQC->printFQC();
	AttributeQubits(qubitStack,mainFQC);
	
	std::string fqcStr = mainFQC->phi->Compile(0,&qubitStack);
	printf("CIRCUIT =\n%s",fqcStr.c_str());
	//Add measures for all qubits
	fqcStr = AddMeasures(fqcStr,totalSize);
	outputfile << fqcStr;
	outputfile.close();
	
	printf("FINAL VAR IDXs\n");
	for(auto var : mainFQC->inputVars)		printf("%s ends on [%d]\n",var->name.c_str(),var->GetStackIdx(&qubitStack));
	for(auto var : mainFQC->auxVars)		printf("%s ends on [%d]\n",var->name.c_str(),var->GetStackIdx(&qubitStack));
	
}


%}
%locations
%union { GenericList* genList; std::string* strName; int varSize;FQC* fqc; Complex* complexValue; QBit* qbitValue; };

%token MAIN IF IFQ ELSE THEN INL INR LET IN EQU INC DEC 
%token OPPAR CLPAR OPBRK CLBRK COMMA VBAR COL RARROW 
%token PLUS TENP
%token TRUE FALSE Q1
%token <strName> ID
%token <complexValue> NUM
%type <qbitValue> CONSTVALUE
%type <genList> ARGS ARGNAMES IDLIST
%type <varSize> VARTYPE
%type <fqc> MAINF STM VAR LETIN FUNCCALL COND
// Operator precedence and associativity:
%right '='  
%%
// RULES SECTION: 

S : FUNCS MAINF{ FunctionSemantics::PopTable(); CompileMain($2); exit(0); }

MAINF: MAIN COL {FunctionSemantics::CreateTable(new std::string("MAIN"));} STM { $$ = $4; }
FUNCS: FUNC FUNCS  | 
FUNC: ID COL {FunctionSemantics::CreateTable($1);} ARGS ID {FunctionSemantics::VerifyFuncName($5);} ARGNAMES {FunctionSemantics::CreateFuncArgVars($4,$7);} 
	  EQU STM {FunctionSemantics::VerifyReturnType($10); FunctionSemantics::SetFuncFQC($10); FunctionSemantics::PopTable(); printf("FUNC FQC");($10)->printFQC();}
	  
ARGS: VARTYPE RARROW ARGS { $$ = NewElement($3,(intptr_t)$1); } |  VARTYPE { FunctionSemantics::SetReturnType($1); $$ = NULL; }
VARTYPE: Q1 PLUS Q1 { $$ = 1; } | VARTYPE TENP VARTYPE { $$ = $1 + $3; }
ARGNAMES: ID ARGNAMES { $$ = NewElement($2,(intptr_t)$1);} | {$$ = NULL;}
STM: VAR { $$ = $1; } | LETIN { $$ = $1; } | FUNCCALL { $$ = $1; } | COND { $$ = $1; }

VAR: ID { $$ = SemanticOperations::OperationVAR($1); } | CONSTVALUE { $$ = SemanticOperations::OperationCONST($1); } 

CONSTVALUE: OPBRK NUM INL OPPAR CLPAR VBAR NUM INR OPPAR CLPAR CLBRK { $$ = new QBit(*$2,*$7);}

LETIN: LET ID EQU STM  {SemanticOperations::AddLetContext($2,$4);}  IN STM { $$ = SemanticOperations::OperationLET($2,$4,$7); FunctionSemantics::RemoveLetContext();  } 

FUNCCALL:ID OPPAR IDLIST CLPAR { FunctionSemantics::VerifyValidArgs($1,$3); $$ = SemanticOperations::OperationFuncCall($1,$3); }
FUNCCALL:ID OPPAR CLPAR { FunctionSemantics::VerifyValidArgs($1,NULL); $$ = SemanticOperations::OperationFuncCall($1,NULL); }

IDLIST: ID  { FunctionSemantics::VerifySymbolExists($1);$$ = NewElement(NULL,(intptr_t)$1);}| 
		ID COMMA IDLIST { FunctionSemantics::VerifySymbolExists($1);$$ = NewElement($3,(intptr_t)$1);}
			 
COND: IF STM THEN STM ELSE STM { $$ = SemanticOperations::OperationIF($2,$4,$6);} |
	  IFQ STM THEN STM ELSE STM { $$ = SemanticOperations::OperationIFQ($2,$4,$6);}

;

%%

int main() {
	yyin = fopen("input.txt","r");
	yyparse();  
	return yylex(); 
} 

void yyerror(const char *s){ printf("\n SYN ERROR - %s on line %d\n",s,yylineno); }
int yywrap(){ return 1; }
