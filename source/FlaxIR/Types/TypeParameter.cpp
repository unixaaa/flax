// TypeParameter.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ir/type.h"

namespace fir
{
	TypeParameter::TypeParameter(std::string name, FunctionType* funcScope) : Type(FTypeKind::Generic)
	{
		this->parameterName = name;
		this->scope = funcScope;

		iceAssert(this->scope && "invalid scope");
	}

	TypeParameter::TypeParameter(std::string name, StructType* structScope) : Type(FTypeKind::Generic)
	{
		this->parameterName = name;
		this->scope = structScope;

		iceAssert(this->scope && "invalid scope");
	}

	std::string TypeParameter::getName()
	{
		return this->parameterName;
	}

	std::string TypeParameter::str()
	{
		return "<" + this->parameterName + ">";
	}

	bool TypeParameter::isTypeEqual(Type* other)
	{
		TypeParameter* ot = dynamic_cast<TypeParameter*>(other);
		if(!ot) return false;
		if(!this->scope->isTypeEqual(ot->scope)) return false;
		if(this->parameterName != ot->parameterName) return false;

		return true;
	}




	static TypeParameter* findExistingInList(std::string name, std::deque<Type*> list)
	{
		for(auto elm : list)
		{
			if(TypeParameter* gen = dynamic_cast<TypeParameter*>(elm))
			{
				if(gen->getName() == name)
				{
					return gen;
				}
			}
		}

		return 0;
	}



	TypeParameter* TypeParameter::getForFunction(std::string name, FunctionType* scope)
	{
		TypeParameter* found = findExistingInList(name, scope->getArgumentTypes());

		if(!found)
		{
			TypeParameter* ret = new TypeParameter(name, scope);
			return ret;
		}
		else
		{
			return found;
		}
	}

	TypeParameter* TypeParameter::getForStruct(std::string name, StructType* scope)
	{
		TypeParameter* found = findExistingInList(name, scope->getElements());

		if(!found)
		{
			TypeParameter* ret = new TypeParameter(name, scope);
			return ret;
		}
		else
		{
			return found;
		}
	}
}


















