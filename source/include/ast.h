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
		Expr(Parser::Pin pos) : posinfo(pos) { }
		virtual ~Expr() { }


		Parser::Pin posinfo;
	};

	struct Dummy : Expr
	{
		Dummy(Parser::Pin pos) : Expr(pos) { }
		virtual ~Dummy() { }

		std::string type;
	};





	// literals
	struct StringLiteral : Expr
	{
		StringLiteral(Parser::Pin pos) : Expr(pos) { }
		virtual ~StringLiteral() override { }

		std::string text;
	};

	struct NumberLiteral : Expr
	{
		virtual ~NumberLiteral() override { }

		double floatingValue = 0;
		int64_t signedValue = 0;
		uint64_t unsignedValue = 0;
	};

	struct BooleanLiteral : Expr
	{
		BooleanLiteral(Parser::Pin pos, bool val) : Expr(pos), value(val) { }
		virtual ~BooleanLiteral() override { }

		bool value;
	};

	struct ArrayLiteral : Expr
	{
		virtual ~ArrayLiteral() override { }

		std::deque<Expr*> values;
	};

	struct TupleLiteral : Expr
	{
		TupleLiteral(Parser::Pin pos, std::deque<Expr*> vals) : Expr(pos), values(vals) { }
		virtual ~TupleLiteral() override { }

		std::deque<Expr*> values;
	};




	struct VarRef : Expr
	{
		VarRef(Parser::Pin pos, std::string n) : Expr(pos), name(n) { }
		virtual ~VarRef() override { }

		std::string name;
	};

	struct UnaryOp : Expr
	{
		UnaryOp(Parser::Pin pos, ArithmeticOp o, Expr* e) : Expr(pos), op(o), expression(e) { }
		virtual ~UnaryOp() override { }

		ArithmeticOp op;
		Expr* expression = 0;
	};

	struct BinaryOp : Expr
	{
		BinaryOp(Parser::Pin pos, ArithmeticOp o, Expr* lhs, Expr* rhs) : Expr(pos), op(o), leftExpr(lhs), rightExpr(rhs) { }
		virtual ~BinaryOp() override { }

		ArithmeticOp op;
		Expr* leftExpr = 0;
		Expr* rightExpr = 0;
	};

	struct SubscriptOp : Expr
	{
		SubscriptOp(Parser::Pin pos, Expr* e, std::deque<Expr*> i) : Expr(pos), expr(e), indices(i) { }
		virtual ~SubscriptOp() override { }

		Expr* expr = 0;
		std::deque<Expr*> indices;
	};

	struct DotOp : Expr
	{
		DotOp(Parser::Pin pos, Expr* lhs, Expr* rhs) : Expr(pos), leftExpr(lhs), rightExpr(rhs) { }
		virtual ~DotOp() override { }

		Expr* leftExpr = 0;
		Expr* rightExpr = 0;
	};

	struct FuncCall : Expr
	{
		virtual ~FuncCall() override { }

		std::string functionName;
		std::deque<Expr*> arguments;
	};

	struct Typeof : Expr
	{
		virtual ~Typeof() override { }

		Expr* expression = 0;
	};











	// statements etc.
	struct ImportStmt : Expr
	{
		virtual ~ImportStmt() override { }

		std::string moduleIdentifier;
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
		BracedBlock(Parser::Pin pos) : Expr(pos) { }
		virtual ~BracedBlock() override { }

		std::deque<Expr*> statements;
		std::deque<DeferredStmt*> deferredStatements;
	};







	// declarations
	struct VarDecl : Expr
	{
		VarDecl(Parser::Pin pos, std::string n, bool immut) : Expr(pos), name(n), isImmutable(immut) { }
		virtual ~VarDecl() override { }

		std::string type;
		std::string name;

		Expr* initialValue = 0;

		bool isStatic = false;
		bool isImmutable = false;

		uint64_t attributes = 0;
	};

	struct FuncDecl : Expr
	{
		FuncDecl(Parser::Pin pos, std::string n, std::deque<VarDecl*> args, std::string ret) : Expr(pos),
			name(n), returnType(ret), arguments(args) { }

		virtual ~FuncDecl() override { }

		std::string name;
		std::string returnType;
		std::deque<VarDecl*> arguments;
		std::deque<std::string> genericTypeParameters;

		bool isFFI = false;
		bool isVarArg = false;
		bool isStatic = false;

		FFIType ffiType;


		uint64_t attributes = 0;

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

	struct ClassPropertyDef : VarDecl
	{
		ClassPropertyDef(Parser::Pin pos, std::string n) : VarDecl(pos, n, true) { }
		virtual ~ClassPropertyDef() override { }

		BracedBlock* getterDef = 0;
		BracedBlock* setterDef = 0;
		std::string setterArgName;
	};










	struct FunctionDef : Expr
	{
		FunctionDef(Parser::Pin pos, FuncDecl* decl, BracedBlock* block) : Expr(pos), funcDecl(decl), funcBody(block) { }
		virtual ~FunctionDef() override { }

		FuncDecl* funcDecl = 0;
		BracedBlock* funcBody = 0;
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
		RootAst();
		virtual ~RootAst() override { }


		std::deque<Expr*> topLevelExpressions;
	};
}


















