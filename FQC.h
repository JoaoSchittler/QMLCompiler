#ifndef FQCCLASS
#define FQCCLASS
#include "Unitary.h"
class FQC
{
	public:
		int a,h,b,g;
		Unitary* phi;
		std::vector<Variable*> inputVars;
		std::vector<Variable*> auxVars;
		FQC(int aa,int hh,int bb,int gg):a(aa), h(hh), b(bb), g(gg) {	}
		FQC(int aa,int hh,int bb):a(aa), h(hh), b(bb), g( aa+hh-bb) {	}
		FQC(int aa,int hh,int bb,int gg,Unitary* p):a(aa), h(hh), b(bb), g(gg), phi(p) {}
		
		void CreateCondition(FQC* t, FQC* f, bool measure)
		{
			
			//Execute condition , add measure, execute t, not on first qubit, execute f, not on first qubit
			if(measure)
			{
				MeasureOp* measureCon = inputVars.size() == 1 ? new MeasureOp(inputVars.at(0)) : new MeasureOp(auxVars.at(0)) ;
				this->phi->PlusUnitary(measureCon);
				measureCon->TensorUnitary(t->phi);
			}
			else
			{
				this->phi->TensorUnitary(t->phi);
			}
			NotRot* not0 = new NotRot(1);
			for(int i = 0; i < t->a+t->h;i++)
				not0->TensorUnitary(new Unitary()); // Add Padding the size of T so that F is on the correct qubits
			not0->TensorUnitary(f->phi);
			
			this->phi->PlusUnitary(not0);
			
			this->phi->PlusUnitary(new NotRot(1));
			
			
			AddFQCVars(t);
			AddFQCVars(f);
			if(inputVars.size()== 1)
			{
				t->phi->SetConditionVar(inputVars.at(0));
				f->phi->SetConditionVar(inputVars.at(0)); 
			}
			else if (auxVars.size() >= 1)
			{
				t->phi->SetConditionVar(auxVars.at(0));
				f->phi->SetConditionVar(auxVars.at(0)); 
			}
			
		}
			
		void printFQC()
		{
			printf("----------------------------------------\n");
			printf("FQC = (%d , %d , %d , %d , Phi)\n",a,h,b,g);
			printf("Phi = \n");
			if(phi!= NULL)	phi->PrintUnitary(0);
			printf("This FQC uses vars : ");
			for(auto var : inputVars) printf("%s, ",var->name.c_str());
			printf("and ");
			for(auto var : auxVars) printf("%s, ",var->name.c_str());
			printf("\n----------------------------------------\n");
		}
		Variable* GetVar(std::string name)
		{
			for(auto var : inputVars)
			{
				if(var->equals(name)) return var;
			}
			for(auto var : auxVars)
			{
				if(var->equals(name)) return var;
			}
			return NULL;
		}
		void AddFQCVars(FQC* fqc)
		{
			for(auto var : (fqc->inputVars))
			{
				this->inputVars.push_back(var);
			}
			for(auto var : (fqc->auxVars))
			{
				this->auxVars.push_back(var);
			}
		}
		void RenameVar(std::string oldName,std::string newName)
		{
			for(auto var : inputVars)
			{
				if(var->name == oldName) var->name = newName;
				for(auto alias : var->aliases) if(alias == oldName) alias = newName;
			}
			for(auto var : auxVars)
			{
				if(var->name == oldName) var->name = newName;
				for(auto alias : var->aliases) if(alias == oldName) alias = newName;
			}
		}
		void AddInputVar(Variable* v)
		{
			inputVars.push_back(v);
		}
		void AddAuxVar(Variable* v)
		{
			auxVars.push_back(v);
		}
};
#endif
