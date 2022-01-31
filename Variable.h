#ifndef VARIABLE
#define VARIABLE

class Variable
{
	public:
		Variable(std::string n, int s):name(n), size(s), value(NULL) {	}
		std::string name;
		std::vector<std::string> aliases;
		int size;
		QBit* value;
		void printAllNames()
		{
			printf("Base name %s | ",name.c_str());
			for(auto a : aliases) printf("%s ",a.c_str());
			printf("\n");
		}
		int GetStackIdx(std::stack<QBit*>* qStack)
		{
			std::stack<QBit*> auxStack;
			int idx = -1, i = 0;
			while(!qStack->empty())
			{
				QBit* top = qStack->top();
				if(top== this->value)
				{
					idx = i;
					break;
				}
				auxStack.push(top);
				qStack->pop();
				i++;
			}
			//Restores stack
			while(!auxStack.empty()) {	qStack->push(auxStack.top()); auxStack.pop();	}
			if(idx == -1)
			{
				printf("STACK VAR %s NOT FOUND\n",name.c_str());
			}
			return idx;
		}
		int GetVecIdx(std::vector<QBit*> vec)
		{
			int i = 0;
			for(auto qbit : vec)
			{
				if(qbit == this->value) return i;
				
				i++;
			}
			printf("VEC VAR %s NOT FOUND\n",name.c_str());
			return -1;
		}
		//Checks if two variables share a name
		bool operator == (Variable& v)
		{
			if(v.name == name) return true; //Test Name and Name
			for(auto myalias : this->aliases) if(myalias == v.name) return true; //Test Aliases and Name
			
			for(auto alias : v.aliases)
			{
				if(this->name == alias) return true; // Test Name and Aliases
				for(auto myalias : this->aliases) if(myalias == alias) return true; //Test Aliases and Aliases
			}
			return false;
		}
		bool equals(std::string name)
		{
			if(name == this->name)				return true;
			for(auto a : this->aliases) 		if(name == a) return true;
			
			return false;
		}
};
#endif
