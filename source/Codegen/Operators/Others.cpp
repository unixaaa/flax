// Others.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "codegen.h"
#include "operators.h"

using namespace Ast;
using namespace Codegen;

namespace Operators
{
	Result_t operatorCustom(CodegenInstance* cgi, ArithmeticOp op, Expr* user, std::deque<Expr*> args)
	{
		auto leftres = args[0]->codegen(cgi);
		auto rightres = args[1]->codegen(cgi);

		auto data = cgi->getBinaryOperatorOverload(user, op, leftres.value->getType(), rightres.value->getType());
		if(data.found)
		{
			return cgi->callBinaryOperatorOverload(data, leftres.value, leftres.pointer, rightres.value, rightres.pointer, op);
		}
		else
		{
			error(user, "No such operator '%s' for expression %s %s %s", Parser::arithmeticOpToString(cgi, op).c_str(),
				leftres.value->getType()->str().c_str(), Parser::arithmeticOpToString(cgi, op).c_str(),
				rightres.value->getType()->str().c_str());
		}
	}



	Result_t operatorCast(CodegenInstance* cgi, ArithmeticOp op, Expr* user, std::deque<Expr*> args)
	{
		if(args.size() != 2)
			error(user, "Expected 2 arguments for operator %s", Parser::arithmeticOpToString(cgi, op).c_str());


		auto leftr = args[0]->codegen(cgi);
		fir::Value* lhs = leftr.value;
		fir::Value* lhsPtr = leftr.pointer;

		fir::Type* rtype = args[1]->getType(cgi);
		if(!rtype)
		{
			GenError::unknownSymbol(cgi, user, args[1]->ptype->str(), SymbolType::Type);
		}


		iceAssert(rtype);
		if(lhs->getType() == rtype)
		{
			warn(user, "Redundant cast");
			return Result_t(lhs, 0);
		}



		if(lhs->getType()->isIntegerType() && rtype->isIntegerType())
		{
			return Result_t(cgi->irb.CreateIntSizeCast(lhs, rtype), 0);
		}
		else if(lhs->getType()->isIntegerType() && rtype->isFloatingPointType())
		{
			return Result_t(cgi->irb.CreateIntToFloatCast(lhs, rtype), 0);
		}
		else if(lhs->getType()->isFloatingPointType() && rtype->isFloatingPointType())
		{
			// printf("float to float: %d -> %d\n", lhs->getType()->getPrimitiveSizeInBits(), rtype->getPrimitiveSizeInBits());

			if(lhs->getType()->toPrimitiveType()->getFloatingPointBitWidth() > rtype->toPrimitiveType()->getFloatingPointBitWidth())
				return Result_t(cgi->irb.CreateFTruncate(lhs, rtype), 0);

			else
				return Result_t(cgi->irb.CreateFExtend(lhs, rtype), 0);
		}
		else if(lhs->getType()->isPointerType() && rtype->isPointerType())
		{
			return Result_t(cgi->irb.CreatePointerTypeCast(lhs, rtype), 0);
		}
		else if(lhs->getType()->isPointerType() && rtype->isIntegerType())
		{
			return Result_t(cgi->irb.CreatePointerToIntCast(lhs, rtype), 0);
		}
		else if(lhs->getType()->isIntegerType() && rtype->isPointerType())
		{
			return Result_t(cgi->irb.CreateIntToPointerCast(lhs, rtype), 0);
		}
		else if(cgi->isEnum(rtype))
		{
			// dealing with enum
			fir::Type* insideType = rtype->toStructType()->getElementN(0);
			if(lhs->getType() == insideType)
			{
				fir::Value* tmp = cgi->getStackAlloc(rtype, "tmp_enum");

				fir::Value* gep = cgi->irb.CreateStructGEP(tmp, 0);
				cgi->irb.CreateStore(lhs, gep);

				return Result_t(cgi->irb.CreateLoad(tmp), tmp);
			}
			else
			{
				error(user, "Enum '%s' does not have type '%s', invalid cast", rtype->toStructType()->getStructName().str().c_str(),
					lhs->getType()->str().c_str());
			}
		}
		else if(cgi->isEnum(lhs->getType()) && lhs->getType()->toStructType()->getElementN(0) == rtype)
		{
			iceAssert(lhsPtr);
			fir::Value* gep = cgi->irb.CreateStructGEP(lhsPtr, 0);
			fir::Value* val = cgi->irb.CreateLoad(gep);

			return Result_t(val, gep);
		}
		else if(cgi->isAnyType(lhs->getType()))
		{
			iceAssert(lhsPtr);
			return cgi->extractValueFromAny(rtype, lhsPtr);
		}
		else if(lhs->getType()->isClassType() && lhs->getType()->toClassType()->getClassName().str() == "String"
			&& rtype == fir::Type::getInt8Ptr(cgi->getContext()))
		{
			// string to int8*.
			// just access the data pointer.

			iceAssert(lhsPtr);
			fir::Value* stringPtr = cgi->irb.CreateStructGEP(lhsPtr, 0);

			return Result_t(cgi->irb.CreateLoad(stringPtr), stringPtr);
		}
		else if(lhs->getType() == fir::Type::getInt8Ptr(cgi->getContext())
			&& rtype->isClassType() && rtype->toClassType()->getClassName().str() == "String")
		{
			// support this shit.
			// error(cgi, this, "Automatic char* -> String casting not yet supported");

			// create a bogus func call.
			TypePair_t* tp = cgi->getTypeByString("String");
			iceAssert(tp);

			std::vector<fir::Value*> args { lhs };
			return cgi->callTypeInitialiser(tp, user, args);
		}
		else if(lhs->getType()->isArrayType() && rtype->isPointerType()
			&& lhs->getType()->toArrayType()->getElementType() == rtype->getPointerElementType())
		{
			// array to pointer cast.
			if(!dynamic_cast<fir::ConstantArray*>(lhs))
				warn(user, "Casting from non-constant array to pointer is potentially unsafe");

			iceAssert(lhsPtr);

			fir::Value* lhsRawPtr = cgi->irb.CreateConstGEP2(lhsPtr, 0, 0);
			return Result_t(lhsRawPtr, 0);
		}
		else if(lhs->getType()->isStringType() && rtype->isCharType())
		{
			if(StringLiteral* sl = dynamic_cast<StringLiteral*>(args[0]))
			{
				if(sl->str.size() != 1)
					error(user, "Only single-character string literals can be cast into chars");

				char c = sl->str[0];
				fir::Value* cc = fir::ConstantChar::get(c);

				return Result_t(cc, 0);
			}
			else
			{
				error(user, "Non-literal strings cannot be cast into chars; did you mean to subscript?");
			}
		}
		else if(lhs->getType()->isCharType() && rtype == fir::Type::getInt8())
		{
			return Result_t(cgi->irb.CreateBitcast(lhs, rtype), 0);
		}
		else if(op != ArithmeticOp::ForcedCast)
		{
			error(user, "Invalid cast from type '%s' to '%s'", lhs->getType()->str().c_str(), rtype->str().c_str());
		}

		return Result_t(cgi->irb.CreateBitcast(lhs, rtype), 0);
	}
}






