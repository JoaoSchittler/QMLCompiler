%option noyywrap
%option yylineno
%{ 
#include <stdlib.h> 
#include <string>
#include "y.tab.h" 
void yyerror(char *) { printf("Lex Error on Line %d\n",yylineno); }
Complex ParseComplex(bool plus)
{
	std::string str = std::string(yytext);
	int impos = str.find(plus?"+":"-",1); /*Finds second minus position*/ 
	float real = std::stof(str.substr(0,impos));
	float imag = std::stof(str.substr(impos,str.length()-impos-1));
	printf("Read Complex (%f,%f)\n",real,imag);
	return Complex(real,imag);
}
%}
%option yylineno
DIGIT [0-9]
LETTER [a-z]|[A-Z]
%%
 /* KEYWORDS */
"main" {return MAIN;}
"if" {return IF;}
"ifo" {return IFQ;}
"else" {return ELSE;}
"then" {return THEN;}
"qinl" { return INL; }
"qinr" { return INR; }
"let" {return LET;}
"in" {return IN;}

 /* TYPES AND OPERATIONS */ 
"Q1"	{return Q1;}
"=" {return EQU;}
"++" {return PLUS;}
"**" {return TENP;}  
 /* Other */
"-o" {return RARROW;}
":" {return COL;}
"(" {return OPPAR;}
")" {return CLPAR;}
"{" {return OPBRK;}
"}" {return CLBRK;}
"," {return COMMA;}
"|" {return VBAR;}
 /* Numbers */
("-"|"+")?{DIGIT}+("."{DIGIT}+)?"-"{DIGIT}+("."{DIGIT}+)?"i" { yylval.complexValue = new Complex(ParseComplex(false)); return NUM;} /* Real Number + Negative Imaginary Number */
("-"|"+")?{DIGIT}+("."{DIGIT}+)?"+"{DIGIT}+("."{DIGIT}+)?"i" { yylval.complexValue = new Complex(ParseComplex(true));  return NUM;} /* Real Number + Positive Imaginary Number */
("-"|"+")?{DIGIT}+("."{DIGIT}+)? {yylval.complexValue = new Complex(std::stof(yytext),0); return NUM; }/*Real Number*/

{LETTER}({LETTER}|{DIGIT})* {yylval.strName = new std::string(yytext);return ID;} 

[\t\n]+        {;}       /* ignore whitespace */ 
 
%%
