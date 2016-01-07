// TypeCheck.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "compiler.h"
#include "typechecking.h"

#include "ir/module.h"

TCInstance* doTypechecking(ModuleInfo* mi)
{
	TCInstance* ti = new TCInstance(mi);
	ti->currentNamespace = mi->fmodule->getTopLevelNamespace();

	mi->tci = ti;

	iceAssert(mi->rootAst != 0);
	mi->rootAst->generateDependencies(ti);
	mi->rootAst->doTypecheck(ti);

	return ti;
}

std::deque<std::string> deconstructTupleString(std::string tup)
{
	if(tup.size() == 0) return { };
	if(tup[0] != '(') return { tup };

	std::deque<std::string> ret;

	int depth = 0;
	std::string curtype = "";

	for(size_t i = 0; i < tup.size(); i++)
	{
		if(tup[i] == '(')
		{
			depth++;
		}
		else if(tup[i] == ')')
		{
			depth--;
		}
		else if(tup[i] == ',')
		{
			ret.push_back(curtype);
			curtype = "";
		}
		else
		{
			curtype += tup[i];
		}
	}

	if(!curtype.empty())
		ret.push_back(curtype);

	iceAssert(depth == 0 && "somehow tuple mismatch cascaded to typecheck phase");
	return ret;
}






std::deque<std::string> TCInstance::getCurrentNamespaceStack()
{
	std::deque<std::string> ret;

	fir::Namespace* curns = this->currentNamespace;
	while(curns != 0 && !curns->name.empty())
	{
		ret.push_back(curns->name);
		curns = curns->parent;
	}

	return ret;
}

fir::Namespace* TCInstance::pushNamespace(std::string name)
{
	this->currentNamespace = this->currentNamespace->getOrAddNamespace(name);
	return this->currentNamespace;
}

void TCInstance::popNamespace()
{
	iceAssert(this->currentNamespace->parent && "cannot pop empty namespace stack");
	this->currentNamespace = this->currentNamespace->parent;
}



























