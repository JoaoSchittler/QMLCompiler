#ifndef UNITARY
#define UNITARY

#include "Complex.h"
#include <vector>
#include <stack>
#include <stdio.h>
#include <cmath>
#include "Variable.h"

class Permutation
{
	public:
		std::vector<Variable*> vars;
		Permutation() {	}
		Permutation(std::vector<Variable*> v): vars(v) {}
		void printVars()
		{
			for(auto var : vars) printf("%s ",var->name.c_str());
			printf("\n");
		}
		
};
class Copy
{
	public:
		Variable* var,* var2;
		Copy(Variable* v,Variable* v2):var(v),var2(v2) { }
		void printVars()
		{
			printf("%s and %s\n",var->name.c_str(),var2->name.c_str());
		}
		
};
class Swap
{
	public:
	int offset, length, totalsize;
	std::string altName = "";
	Swap( int o, int l, int n) : offset(o), length(l), totalsize(n) {	}
	Swap( std::string name ): altName(name), offset(0), length(0), totalsize(0) { 	}
	void printSwap(){ printf(" %s(%d,%d,%d)\n",altName.c_str(),offset,length,totalsize);	}
	
};
class Rotation 
{
	public:
	Complex rotation[2][2];
	enum class RotType { None, Identity, Not	};
	RotType type = RotType::None;
	void InitIdentity()
	{
		type = RotType::Identity;
		rotation[0][0] = Complex(1,0);
		rotation[0][1] = Complex(0,0);
		rotation[1][0] = Complex(0,0);
		rotation[1][1] = Complex(1,0);
	}
	void InitNot()
	{
		type = RotType::Not;
		rotation[0][0] = Complex(0,0);
		rotation[0][1] = Complex(1,0);
		rotation[1][0] = Complex(1,0);
		rotation[1][1] = Complex(0,0);
	}
	void InitConst(Complex a,Complex b)
	{
		type = RotType::None;
		rotation[0][0] = a;
		rotation[0][1] = b;
		rotation[1][0] = b;
		rotation[1][1] = a;
	}
	bool operator ==(const Rotation& u)
    {
        if(this->rotation[0][0] == u.rotation[0][0] &&
		   this->rotation[0][1] == u.rotation[0][1] &&
		   this->rotation[1][0] == u.rotation[1][0] &&
		   this->rotation[1][1] == u.rotation[1][1])
		{
			return true;
		}
		else
			return false;
    }
	void printRotation()
	{
		if(type == RotType::Identity)
		{
			printf("Identity Matrix\n");
			return;
		}
		if(type == RotType::Identity)
		{
			printf("Not Matrix\n");
			return;
		}
		printf("(%f + %fi, %f + %fi)(%f + %fi, %f + %fi)\n",
		rotation[0][0].r,rotation[0][0].i,rotation[0][1].r,rotation[0][1].i,
		rotation[1][0].r,rotation[1][0].i,rotation[1][1].r,rotation[1][1].i);
	}
	void MatrixToAngles(double* theta,double* lambda,double* phi)
	{
		if(type == RotType::Identity)
		{
			*theta = 0;
			*lambda = 0;
			*phi = 0;
			return;
		}
		if(type == RotType::Not)
		{
			*theta = 3.141593;
			*lambda = 3.141593;
			*phi = 0;
			return;
		}
		//printRotation();
		*theta = 2*acos(rotation[0][0].r);
		if(*theta == 0)
		{
			if(rotation[1][1].r == -1)
			{
				*lambda = 3.141593/2;
				*phi = 3.141593/2;
			}
			else
			{
				*lambda = 0;
				*phi = 0;
			}
		}
		else
		{
			Complex div = Complex(sin(*theta/2),0);	
			Complex lambdaMul = Complex(-1,0)*rotation[0][1]/div;
			*lambda = lambdaMul.r > 0 ? log(lambdaMul.Magnitude()) : acos( lambdaMul.r );
			
		
			Complex phiMul =  Complex(rotation[1][0]/div);
			*phi = phiMul.r > 0 ?  log(phiMul.Magnitude()) :acos( phiMul.r );
		}
	}
};


class Unitary
{
	public:
	enum class UType { Rot, Swap, Copy, Perm, Measure, Null };
	UType type;
	union { Rotation* rot; Swap* swap; Copy* copy; Permutation* perm; Variable* measureVar; };
	Unitary* nextP = NULL;
	Unitary* nextT = NULL;
	Variable* condition = NULL;
	Unitary() {	type = UType::Null; }
		
	void PlusUnitary(Unitary* ut)   
	{ 
		if(nextP == NULL)
			nextP = ut;
		else
			nextP->PlusUnitary(ut);		
	}
	void TensorUnitary(Unitary* ut) 
	{ 
		if(nextT == NULL)
			nextT = ut;
		else
			nextT->TensorUnitary(ut); 
	}
	void PrintUnitary(int tab = 0)
	{
		for(int i = 0; i < tab;i++) printf("\t");
		printf("Unitary Operation of type ");
		if(type == UType::Rot) { printf("Rotation: ",rot);rot->printRotation();}
		if(type == UType::Swap) { printf("SwapN:");	swap->printSwap();	}
		if(type == UType::Perm) { printf("Permutation of :"); perm->printVars();		}
		if(type == UType::Copy) { printf("Copy operation of:"); copy->printVars();		}
		if(type == UType::Measure) { printf("Measure\n"); }
		if(type == UType::Null) { printf("Null operation\n");		}
		if(nextT != NULL)
		{
			nextT->PrintUnitary(tab+1);
		}
		if(nextP != NULL)
		{
			nextP->PrintUnitary(tab);
		}
		
	}
	void SetConditionVar(Variable* c)
	{
		if(condition != NULL) return;
		
		condition = c;
		if(nextP != NULL) nextP->SetConditionVar(c);
		if(nextT != NULL) nextT->SetConditionVar(c);
		
	}
	std::string Compile(int index, std::stack<QBit*>* stk)
	{
		std::string compiledUnitary = "";
		switch(type)
		{
			case UType::Rot:	 compiledUnitary.append(this->CompileRotation(index,stk));
							  	 break;
			case UType::Swap:	 compiledUnitary.append(this->CompileSwap(stk));
							  	 break;
			case UType::Perm:    compiledUnitary.append(this->CompilePermutation(stk));
								 break;
			case UType::Copy:    compiledUnitary.append(this->CompileCopy(stk));
								 break;
			case UType::Measure: compiledUnitary.append(this->CompileMeasure(stk));
								 break;
			default:			 compiledUnitary.append("");
		}
		
		if(nextT != NULL)	
		{
			std::string tensorStr = nextT->Compile(index+1,stk);
			compiledUnitary.append(tensorStr);
		}
		if(nextP != NULL)
		{
			std::string sumStr = nextP->Compile(index,stk);
			compiledUnitary.append(sumStr);
		}
		return compiledUnitary;
	}
protected:
	std::string CompileRotation(int index,std::stack<QBit*>* stack)
	{
		double t,p,l;
		rot->MatrixToAngles(&t,&l,&p);
		if(t == 0 && p == 0 && l == 0) return std::string("");
		if(rot->type == Rotation::RotType::Not)
		{
			char buffer[25]; 
			if(condition == NULL)
			{
				sprintf(buffer,"NOT %d\n",index);
			}
			else
			{
				sprintf(buffer,"CNOT %d %d\n",condition->GetStackIdx(stack),index);
			}
			return std::string(buffer);
		}
		char buffer[50]; 
		if(condition == NULL)
		{
			sprintf(buffer,"U3 %d %f %f %f\n",index,t,p,l);
		}
		else
		{
			sprintf(buffer,"CU3 %d %d %f %f %f\n",condition->GetStackIdx(stack),index,t,p,l);
		}
		return std::string(buffer);
	}
	std::string CompileSwap(std::stack<QBit*>* stack)
	{
		//Generate Swap strings
		std::string swapStr;
		int s = stack->size();
		for(int i = 0; i < swap->length; i++)
		{
			char buffer[25]; 
			if(condition == NULL)
			{
				//Avoid swap with itself
				if(swap->offset+i != i)	 sprintf(buffer,"SWAP %d %d \n",swap->offset+i,i);
				else 
				{
					sprintf(buffer,"");
				}
			}
			else
			{
				if(swap->offset+i != i && condition->GetStackIdx(stack) != swap->offset+i && condition->GetStackIdx(stack) != i)
					sprintf(buffer,"CSWAP %d %d %d \n",condition->GetStackIdx(stack),swap->offset+i,i);
				else 
				{
					sprintf(buffer,"");
				} 
			}
			swapStr.append(buffer);
		}
		//Execute those swaps in the qubit stack
		std::stack<QBit*> offsetStack;
		std::stack<QBit*> lenStack;
		//Destack offset
		for(int i = 0; i < swap->offset; i++)
		{ offsetStack.push(stack->top()); stack->pop(); }
		//Destack length
		for(int i = 0; i < swap->length; i++)
		 {lenStack.push(stack->top()); stack->pop(); }
		//Stack offset back 
		for(int i = 0; i < swap->offset; i++)
		{ stack->push(offsetStack.top()); offsetStack.pop(); }
		//Stack length back
		for(int i = 0; i < swap->length; i++)
		{ stack->push(lenStack.top()); lenStack.pop(); }

		
		return swapStr;
	}
	std::string CompilePermutation(std::stack<QBit*>* stack)
	{
		int i = 0;
		std::vector<QBit*> auxVector; 
		//Builds vector as reverse stack
		while(!stack->empty()) { auxVector.push_back(stack->top()); stack->pop();}
		int last = auxVector.size() -1;
		
		std::string permstr = std::string("");
		
		for(auto var : perm->vars)
		{	
			int vecIdx = var->GetVecIdx(auxVector);
			for(int s = 0; s < var->size; s++ )
			{
				char buffer[25]; 
				if(i != vecIdx)	
				{
					if(condition == NULL)
					{
						//Avoid swap with itself
						if(s+i != s+vecIdx) sprintf(buffer,"SWAP %d %d\n",s+i,s+vecIdx);
						else sprintf(buffer,"");
					}
					else
					{
						if(s+i != s+vecIdx && condition->GetVecIdx(auxVector) != s + i && condition->GetVecIdx(auxVector)  != s+vecIdx)
							sprintf(buffer,"CSWAP %d %d %d\n",condition->GetVecIdx(auxVector),s+i,s+vecIdx);
						else 
							sprintf(buffer,"");
					}
					permstr.append(buffer);
					QBit* aux = auxVector.at(s+i);
					auxVector.at(s+i) = auxVector.at(s+vecIdx);
					auxVector.at(s+vecIdx) = aux;
				}
				else sprintf(buffer,"");
				i++;
			}
		}
		//Restore stack (reverse iterator)
		for(auto qbit = auxVector.rbegin(); qbit < auxVector.rend(); qbit++)
		{
			stack->push(*qbit);
		}
		
		return permstr;
	}
	std::string CompileCopy(std::stack<QBit*>* stack)
	{
		std::string copystr = std::string("");
		for(int i = 0; i < copy->var->size; i++)
		{
			char buffer[25]; 
			if(condition == NULL)
			{
				sprintf(buffer,"CNOT %d %d\n",copy->var2->GetStackIdx(stack)+i,copy->var->GetStackIdx(stack)+i);
			}
			else
			{
				sprintf(buffer,"CCNOT %d %d %d\n",condition->GetStackIdx(stack),copy->var2->GetStackIdx(stack)+i,copy->var->GetStackIdx(stack)+i);
			}
			copystr.append(buffer);
		}
		return copystr;
	}
	std::string CompileMeasure(std::stack<QBit*>* stack)
	{
		char buffer[25]; 
		//if(condition == NULL)
		{
			sprintf(buffer,"Measure %d\n",measureVar->GetStackIdx(stack));
		}
		return std::string(buffer);
	}
};
class IdentityRot : public Unitary
{
	public:
	IdentityRot(int len)
	{
		if(len == 0)
		{
			return;
		}
		type = UType::Rot;
		while(len != 0)
		{
			rot = new Rotation();
			rot->InitIdentity();
			if(len != 1)
			{
				Unitary* ideRot = new Unitary();
				ideRot->type = UType::Rot;
				ideRot->rot = new Rotation();
				ideRot->rot->InitIdentity();
				
				if(nextT == NULL)
				{
					nextT = ideRot;
				}
				else
				{
					nextT->TensorUnitary(ideRot);
				}
			}
			len--;
		}
	}
};
class ConstRot: public Unitary
{
	public:
	ConstRot(Complex a,Complex b)
	{
		type = UType::Rot;
		rot = new Rotation();
		rot->InitConst(a,b);	
	}	
};
class NotRot: public Unitary
{
	public:
	NotRot(int len)
	{
		type = UType::Rot;
		if(len == 0) return;
		
		rot = new Rotation();
		rot->InitNot();
		
		if(len != 1)	nextT = new NotRot(len-1);
	}	
};
class SwapOp : public Unitary
{
	public:
	SwapOp(int o,int l,int n)
	{
		type = UType::Swap;
		swap =	new Swap(o,l,n);	
	}
};
class CopyOp: public Unitary
{
	public:
	CopyOp(Variable* v,Variable* v2)
	{
		type = UType::Copy;
		copy =	new Copy(v,v2);	
	}
};
class PermOp: public Unitary
{
	public:
	PermOp(std::vector<Variable*> v)
	{
		type = UType::Perm;
		perm = new Permutation(v);
	}
};
class MeasureOp : public Unitary
{
	public:
	MeasureOp(Variable* v)
	{
		type = UType::Measure;
		measureVar = v;
	}
};
#endif
