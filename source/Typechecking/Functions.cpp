// Functions.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "typechecking.h"
using namespace Ast;






fir::Type* FuncDecl::doTypecheck(TCInstance* ti)
{
	return 0;
}

fir::Type* FunctionDef::doTypecheck(TCInstance* ti)
{
	return 0;
}





void FuncDecl::generateDependencies(TCInstance* ti)
{
	// first, the return type
	std::deque<std::string> list = deconstructTupleString(this->returnType);

	// this is a list of the types as provided by the user, verbatim.
	// hence, we need to create a list of "possible" types that it could refer to
	// eg. user gives "Bar::Foo". we are in namespace "Qux::Baz".
	// therefore, "Bar::Foo" could refer to any of these types:

	// 1. Qux::Baz::Bar::Foo
	// 2. Qux::Bar::Foo
	// 3. Bar::Foo

	// of course, if later on we discover that there is more than one match, we raise an error.


	std::string fullyqual = "";
	for(auto n : ti->getCurrentNamespaceStack())
		fullyqual += (n + "::");

	fullyqual += this->name;



	// now for parameters.
	// same thing.
	// actually we just add it to the same list.
	for(auto p : this->arguments)
		list.push_back(p->type);


	for(auto type : list)
	{
		std::deque<std::string> possibilities;

		for(size_t i = 0; i < ti->getCurrentNamespaceStack().size() + 1; i++)
		{
			std::string ns;
			for(size_t j = 0; j < i; j++)
				ns += ti->getCurrentNamespaceStack()[j] + "::";

			possibilities.push_back(ns + type);
		}

		printf("adding dependency from %s to %s\n", fullyqual.c_str(), possibilities[0].c_str());
		ti->graph.addTypeDependency(fullyqual, possibilities, this);
	}
}

void FunctionDef::generateDependencies(TCInstance* ti)
{
	// definitions need several things:
	// 1. declaration needs to have dependencies generated.
	// 2. statements beed to have dependencies generated as well.

	this->funcDecl->generateDependencies(ti);
	this->funcBody->generateDependencies(ti);
}
































