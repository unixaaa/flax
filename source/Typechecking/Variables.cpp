// Variables.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "typechecking.h"

#include "ir/namespace.h"

using namespace Ast;


fir::Type* VarRef::doTypecheck(TCInstance* ti)
{
	auto ident = ti->getIdentifier(this->name);

	if(ident.second == 0)
		error(this, "Using undeclared variable %s", this->name.c_str());

	return ident.second;
}


fir::Type* VarDecl::doTypecheck(TCInstance* ti)
{
	if(this->type == "##__Inferred__##")
		error(this, "Type inferrence not supported yet.");

	// check if its a builtin first
	fir::Type* actualType = fir::Type::fromBuiltin(this->type);
	if(actualType)
	{
		ti->addIdentifier(this->name, actualType);
	}
	else
	{
		actualType = ti->currentNamespace->getNamedType(this->type);
		if(!actualType)
			error(this, "Using undeclared type %s to instantiate variable", this->type.c_str());
	}

	return actualType;
}



































