// Namespace.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ir/type.h"
#include "ir/value.h"
#include "ir/function.h"
#include "ir/namespace.h"

#include <list>

namespace fir
{
	Namespace::Namespace(Module* mp)
	{
		this->mparent = mp;
		this->parent = 0;
	}

	Namespace::Namespace(Namespace* np)
	{
		this->mparent = np->mparent;
		this->parent = np;
	}

	std::deque<Function*> Namespace::getFunctionsWithNameAndType(std::string name, FunctionType* ft)
	{
		std::deque<Function*> ret;

		// move up the... list.
		if(this->parent != 0)
		{
			auto p = this->parent->getFunctionsWithNameAndType(name, ft);
			for(auto f : p) ret.push_back(f);
		}

		for(auto f : this->functions)
		{
			if(f->getName() == name && (ft == 0 || f->getType()->isTypeEqual(ft)))
				ret.push_back(f);
		}

		return ret;
	}

	std::deque<Function*> Namespace::getFunctionsWithName(std::string name)
	{
		return this->getFunctionsWithNameAndType(name, 0);
	}

	std::deque<Function*> Namespace::getFunctionsWithFQName(std::deque<std::string> qualifiedName)
	{
		return this->getFunctionsWithFQNameAndType(qualifiedName, 0);
	}








	static Namespace* resolveQualifiedName(Namespace* cur, std::deque<std::string> qualifiedName)
	{
		if(qualifiedName.size() == 0) return 0;

		// get root.
		Namespace* rootns = cur;
		while(rootns->parent != 0)
			rootns = rootns->parent;

		auto qn = qualifiedName;
		Namespace* curns = rootns;

		for(size_t i = 0; i < qn.size(); i++)
		{
			bool found = false;
			for(auto sub : curns->subNamespaces)
			{
				if(sub->name == qn[i])
				{
					found = true;
					curns = sub;
					break;
				}
			}

			if(!found)
				break;
		}

		return curns;
	}

	std::deque<Function*> Namespace::getFunctionsWithFQNameAndType(std::deque<std::string> qualifiedName, FunctionType* ft)
	{
		std::deque<Function*> ret;
		Namespace* curns = resolveQualifiedName(this, qualifiedName);

		if(qualifiedName.size() > 1 && curns->name == qualifiedName[qualifiedName.size() - 2])
		{
			for(auto f : curns->functions)
			{
				if(f->getName() == name && (ft == 0 || f->getType() == ft))
					ret.push_back(f);
			}
		}

		return ret;
	}

	StructType* Namespace::getNamedType(std::string name)
	{
		// search upwards
		if(this->parent != 0)
		{
			auto p = this->parent->getNamedType(name);
			if(p) return p;
		}

		for(auto t : this->namedTypes)
		{
			if(t->getStructName() == name)
				return t;
		}

		return 0;
	}

	StructType* Namespace::getFQNamedType(std::deque<std::string> qualifiedName)
	{
		Namespace* curns = resolveQualifiedName(this, qualifiedName);
		if(qualifiedName.size() > 1 && curns->name == qualifiedName[qualifiedName.size() - 2])
		{
			for(auto t : curns->namedTypes)
			{
				if(t->getStructName() == name)
					return t;
			}
		}

		return 0;
	}
}






















