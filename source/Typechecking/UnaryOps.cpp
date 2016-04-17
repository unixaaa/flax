// UnaryOps.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;

fir::Type* UnaryOp::doTypecheck(TCInstance* ti)
{
	fir::Type* etype = this->expression->doTypecheck(ti);
	if(this->op == ArithmeticOp::LogicalNot)
	{
		// check if it's logical-not-able
		if(etype != fir::PrimitiveType::getBool())
			error(this, "Logical not can only be used with boolean types.");

		return etype;
	}
	else if(this->op == ArithmeticOp::Plus || this->op == ArithmeticOp::Minus || this->op == ArithmeticOp::BitwiseNot)
	{
		if(!etype->isIntegerType() && !etype->isFloatingPointType())
			error(this, "Unary plus can only be used with numerical types.");

		return etype;
	}
	else if(this->op == ArithmeticOp::Deref)
	{
		if(!etype->isPointerType())
			error(this, "Only pointer types can be dereferenced.");

		else if(etype->getPointerElementType()->isVoidType())
			error(this, "Pointers to void cannot be dereferenced.");

		return etype->getPointerElementType();
	}
	else if(this->op == ArithmeticOp::AddrOf)
	{
		if(etype->isVoidType())
			error(this, "The address of a void value cannot be taken directly.");

		return etype->getPointerTo();
	}

	iceAssert(0 && "Invalid unary op encountered.");
}















