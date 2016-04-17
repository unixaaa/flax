// tcn.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <map>
#include <string>
#include <deque>

#include "typeinfo.h"
#include "defs.h"

namespace fir
{
	struct PHINode;
	struct StructType;
}

// type-checked node
namespace Tcn
{
	struct Expr;

	enum class ResultType { Normal, BreakCodegen };
	struct Result_t
	{
		Result_t(fir::Value* v, fir::Value* p, ResultType rt) : val(v), ptr(p), type(rt) { }
		Result_t(fir::Value* v, fir::Value* p) : val(v), ptr(p), type(ResultType::Normal) { }

		fir::Value* val = 0;
		fir::Value* ptr = 0;

		ResultType type;
	};

	struct Expr
	{
		Expr(Parser::Pin pos) : pin(pos) { }
		virtual ~Expr() { }
		// virtual Result_t codegen(Codegen::CodegenInstance* cgi, fir::Value* lhsPtr, fir::Value* rhs) = 0;

		Parser::Pin pin;
		fir::Type* type = 0;
	};

	struct Number : Expr
	{
	};

	struct BoolVal : Expr
	{
	};

	struct VarRef : Expr
	{
	};

	struct VarDecl : Expr
	{
	};

	struct ComputedProperty : VarDecl
	{
	};

	struct BinOp : Expr
	{
	};

	struct FuncDecl : Expr
	{
	};

	struct BracedBlock : Expr
	{
	};

	struct Func : Expr
	{
	};

	struct FuncCall : Expr
	{
	};

	struct Return : Expr
	{
	};

	struct Import : Expr
	{
	};

	struct ForeignFuncDecl : Expr
	{
	};

	struct DeferredExpr : Expr
	{
	};

	struct BreakableBracedBlock : Expr
	{
	};

	struct IfStmt : Expr
	{
	};

	struct WhileLoop : BreakableBracedBlock
	{
	};

	struct ForLoop : BreakableBracedBlock
	{
	};

	struct ForeachLoop : BreakableBracedBlock
	{

	};

	struct Break : Expr
	{
	};

	struct Continue : Expr
	{
	};

	struct UnaryOp : Expr
	{
	};

	struct OpOverload : Expr
	{
	};

	struct StructBase : Expr
	{
	};

	struct Class : StructBase
	{
	};

	struct Extension : Class
	{
	};

	struct Struct : StructBase
	{
	};

	struct Enumeration : Class
	{
	};

	struct Tuple : StructBase
	{
	};

	enum class MAType
	{
		Invalid,
		LeftNamespace,
		LeftVariable,
		LeftFunctionCall,
		LeftTypename
	};

	struct MemberAccess : Expr
	{
		MAType matype = MAType::Invalid;
	};

	struct NamespaceDecl : Expr
	{
	};

	struct ArrayIndex : Expr
	{
	};

	struct StringLiteral : Expr
	{
	};

	struct ArrayLiteral : Expr
	{
	};

	struct TypeAlias : StructBase
	{
	};

	struct Alloc : Expr
	{
	};

	struct Dealloc : Expr
	{
	};

	struct Typeof : Expr
	{
	};

	struct PostfixUnaryOp : Expr
	{
		enum class Kind
		{
			Invalid,
			ArrayIndex,
			Increment,
			Decrement
		};

		Kind kind;
	};

	struct Root : Expr
	{
	};
}









