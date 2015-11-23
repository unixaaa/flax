// Literals.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;

fir::Type* StringLiteral::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* NumberLiteral::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* BooleanLiteral::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* ArrayLiteral::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* TupleLiteral::doTypecheck(TCInstance *ti)
{
	return 0;
}
