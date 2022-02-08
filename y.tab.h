
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 1676 of yacc.c  */
#line 1 "sintatical.y"

   	#include "Function.h"
	#include "SemanticActions.h"



/* Line 1676 of yacc.c  */
#line 47 "y.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MAIN = 258,
     IF = 259,
     IFQ = 260,
     ELSE = 261,
     THEN = 262,
     INL = 263,
     INR = 264,
     LET = 265,
     IN = 266,
     EQU = 267,
     INC = 268,
     DEC = 269,
     OPPAR = 270,
     CLPAR = 271,
     OPBRK = 272,
     CLBRK = 273,
     COMMA = 274,
     VBAR = 275,
     COL = 276,
     RARROW = 277,
     PLUS = 278,
     TENP = 279,
     TRUE = 280,
     FALSE = 281,
     Q1 = 282,
     ID = 283,
     NUM = 284
   };
#endif
/* Tokens.  */
#define MAIN 258
#define IF 259
#define IFQ 260
#define ELSE 261
#define THEN 262
#define INL 263
#define INR 264
#define LET 265
#define IN 266
#define EQU 267
#define INC 268
#define DEC 269
#define OPPAR 270
#define CLPAR 271
#define OPBRK 272
#define CLBRK 273
#define COMMA 274
#define VBAR 275
#define COL 276
#define RARROW 277
#define PLUS 278
#define TENP 279
#define TRUE 280
#define FALSE 281
#define Q1 282
#define ID 283
#define NUM 284




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 106 "sintatical.y"
 GenericList* genList; std::string* strName; int varSize;FQC* fqc; Complex* complexValue; QBit* qbitValue; 


/* Line 1676 of yacc.c  */
#line 126 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

