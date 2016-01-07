// MiscStatements.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "ast.h"
using namespace Ast;


fir::Type* Typeof::doTypecheck(TCInstance* ti)
{
	return 0;
}

fir::Type* ImportStmt::doTypecheck(TCInstance* ti)
{
	return 0;
}




void Typeof::generateDependencies(TCInstance* ti)
{
}

void ImportStmt::generateDependencies(TCInstance* ti)
{
}
