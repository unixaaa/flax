// Variables.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;


fir::Type* VarRef::doTypecheck(TCInstance* ti)
{
	return 0;
}


fir::Type* VarDecl::doTypecheck(TCInstance* ti)
{
	return 0;
}




void VarRef::generateDependencies(TCInstance* ti)
{
}

void VarDecl::generateDependencies(TCInstance* ti)
{
}
