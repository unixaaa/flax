// Classes.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;


fir::Type* ClassDef::doTypecheck(TCInstance* ti)
{
	if(didGenerateType) return this->generatedType;

	return 0;
}


fir::Type* ClassPropertyDef::doTypecheck(TCInstance* ti)
{
	return 0;
}







