// AllocStatements.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;



fir::Type* AllocStmt::doTypecheck(TCInstance* ti)
{
	return 0;
}

fir::Type* DeallocStmt::doTypecheck(TCInstance* ti)
{
	return 0;
}



void AllocStmt::generateDependencies(TCInstance* ti)
{
}

void DeallocStmt::generateDependencies(TCInstance* ti)
{
}
