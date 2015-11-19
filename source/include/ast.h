// ast.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "defs.h"
#include "ir/type.h"

namespace Ast
{
	struct Expr
	{
		virtual ~Expr() { }
	};

	struct Dummy : Expr
	{
		virtual ~Dummy() { }
	};





	// literals
	struct StringLiteral : Expr
	{
		virtual ~StringLiteral() override { }
	};

	struct NumberLiteral : Expr
	{
		virtual ~NumberLiteral() override { }
	};

	struct BooleanLiteral : Expr
	{
		virtual ~BooleanLiteral() override { }
	};

	struct ArrayLiteral : Expr
	{
		virtual ~ArrayLiteral() override { }
	};

	struct TupleLiteral : Expr
	{
		virtual ~TupleLiteral() override { }
	};




	struct VarRef : Expr
	{
		virtual ~VarRef() override { }
	};

	struct UnaryOp : Expr
	{
		virtual ~UnaryOp() override { }
	};

	struct BinaryOp : Expr
	{
		virtual ~BinaryOp() override { }
	};

	struct FuncCall : Expr
	{
		virtual ~FuncCall() override { }
	};

	struct Typeof : Expr
	{
		virtual ~Typeof() override { }
	};











	// statements etc.
	struct ImportStmt : Expr
	{
		virtual ~ImportStmt() override { }
	};

	struct IfStmt : Expr
	{
		virtual ~IfStmt() override { }
	};

	struct WhileLoop : Expr
	{
		virtual ~WhileLoop() override { }
	};

	struct BreakStmt : Expr
	{
		virtual ~BreakStmt() override { }
	};

	struct ContinueStmt : Expr
	{
		virtual ~ContinueStmt() override { }
	};

	struct ReturnStmt : Expr
	{
		virtual ~ReturnStmt() override { }
	};

	struct AllocStmt : Expr
	{
		virtual ~AllocStmt() override { }
	};

	struct DeallocStmt : Expr
	{
		virtual ~DeallocStmt() override { }
	};

	struct DeferredStmt : Expr
	{
		virtual ~DeferredStmt() override { }
	};

	struct BracedBlock : Expr
	{
		virtual ~BracedBlock() override { }
	};







	// declarations
	struct VarDecl : Expr
	{
		virtual ~VarDecl() override { }



		bool isStatic = false;
	};

	struct FuncDecl : Expr
	{
		virtual ~FuncDecl() override { }



		bool isStatic = false;
	};






	// large things
	struct CompoundType : Expr
	{
		virtual ~CompoundType() override { }
	};

	struct StructDef : CompoundType
	{
		virtual ~StructDef() override { }
	};

	struct EnumerationDef : CompoundType
	{
		virtual ~EnumerationDef() override { }
	};

	struct ClassDef : CompoundType
	{
		virtual ~ClassDef() override { }
	};

	struct ExtensionDef : CompoundType
	{
		virtual ~ExtensionDef() override { }
	};

	struct ClassPropertyDef : Expr
	{
		virtual ~ClassPropertyDef() override { }
	};










	struct FunctionDef : Expr
	{
		virtual ~FunctionDef() override { }



		FuncDecl* decl = 0;
	};

	struct OpOverloadDef : Expr
	{
		virtual ~OpOverloadDef() override { }
	};

	struct TypeAliasDef : Expr
	{
		virtual ~TypeAliasDef() override { }
	};

	struct NamespaceDef : Expr
	{
		virtual ~NamespaceDef() override { }
	};








	struct RootAst : Expr
	{



		std::deque<Expr*> topLevelExpressions;
	};
}


















