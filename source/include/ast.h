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

		virtual fir::Type* doTypecheck(TCInstance* ti) = 0;
		Parser::Pin posinfo;
	};

	struct Dummy : Expr
	{
		Dummy(Parser::Pin pos) : Expr(pos) { }
		virtual ~Dummy() { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override { return 0; }
		std::string type;
	};





	// literals
	struct StringLiteral : Expr
	{
		StringLiteral(Parser::Pin pos, std::string str) : Expr(pos),
			text(str) { }

		virtual ~StringLiteral() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string text;

		bool isRawString = false;
	};

	struct NumberLiteral : Expr
	{
		NumberLiteral(Parser::Pin pos, bool floating) : Expr(pos),
			isFloating(floating) { }

		virtual ~NumberLiteral() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		bool isFloating = false;

		double floatingValue = 0;
		int64_t integerValue = 0;
	};

	struct BooleanLiteral : Expr
	{
		BooleanLiteral(Parser::Pin pos, bool val) : Expr(pos),
			value(val) { }

		virtual ~BooleanLiteral() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		bool value;
	};

	struct ArrayLiteral : Expr
	{
		ArrayLiteral(Parser::Pin pos, std::deque<Expr*> vals) : Expr(pos),
			values(vals) { }

		virtual ~ArrayLiteral() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::deque<Expr*> values;
	};

	struct TupleLiteral : Expr
	{
		TupleLiteral(Parser::Pin pos, std::deque<Expr*> vals) : Expr(pos),
			values(vals) { }

		virtual ~TupleLiteral() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::deque<Expr*> values;
	};




	struct VarRef : Expr
	{
		VarRef(Parser::Pin pos, std::string n) : Expr(pos),
			name(n) { }

		virtual ~VarRef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string name;
	};

	struct UnaryOp : Expr
	{
		UnaryOp(Parser::Pin pos, ArithmeticOp o, Expr* e) : Expr(pos),
			op(o), expression(e) { }

		virtual ~UnaryOp() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		ArithmeticOp op;
		Expr* expression = 0;
	};

	struct BinaryOp : Expr
	{
		BinaryOp(Parser::Pin pos, ArithmeticOp o, Expr* lhs, Expr* rhs) : Expr(pos),
			op(o), leftExpr(lhs), rightExpr(rhs) { }

		virtual ~BinaryOp() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		ArithmeticOp op;
		Expr* leftExpr = 0;
		Expr* rightExpr = 0;
	};

	struct SubscriptOp : Expr
	{
		SubscriptOp(Parser::Pin pos, Expr* e, std::deque<Expr*> i) : Expr(pos),
			expr(e), indices(i) { }

		virtual ~SubscriptOp() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* expr = 0;
		std::deque<Expr*> indices;
	};

	struct DotOp : Expr
	{
		DotOp(Parser::Pin pos, Expr* lhs, Expr* rhs) : Expr(pos),
			leftExpr(lhs), rightExpr(rhs) { }

		virtual ~DotOp() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* leftExpr = 0;
		Expr* rightExpr = 0;
	};

	struct FuncCall : Expr
	{
		FuncCall(Parser::Pin pos, std::string n, std::deque<Expr*> args) : Expr(pos),
			name(n), arguments(args) { }

		virtual ~FuncCall() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string name;
		std::deque<Expr*> arguments;
	};

	struct Typeof : Expr
	{
		Typeof(Parser::Pin pos, Expr* e) : Expr(pos),
			expr(e) { }

		virtual ~Typeof() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* expr = 0;
	};











	// statements etc.
	struct ImportStmt : Expr
	{
		ImportStmt(Parser::Pin pos, std::string imp) : Expr(pos),
			moduleIdentifier(imp) { }

		virtual ~ImportStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string moduleIdentifier;
	};

	struct BracedBlock;
	struct IfStmt : Expr
	{
		typedef std::pair<Expr*, BracedBlock*> CondBlockPair;

		IfStmt(Parser::Pin pos, std::deque<CondBlockPair> conds, BracedBlock* ec) : Expr(pos),
			cases(conds), elseCase(ec) { }

		virtual ~IfStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;

		std::deque<CondBlockPair> cases;
		BracedBlock* elseCase = 0;
	};

	struct WhileLoop : Expr
	{
		WhileLoop(Parser::Pin pos, Expr* cond, BracedBlock* body, bool kind) : Expr(pos),
			loopCondition(cond), loopBody(body), isDoWhile(kind) { }

		virtual ~WhileLoop() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* loopCondition = 0;
		BracedBlock* loopBody = 0;

		bool isDoWhile = false;
	};

	struct BreakStmt : Expr
	{
		BreakStmt(Parser::Pin pos) : Expr(pos) { }
		virtual ~BreakStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;	};

	struct ContinueStmt : Expr
	{
		ContinueStmt(Parser::Pin pos) : Expr(pos) { }
		virtual ~ContinueStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;	};

	struct ReturnStmt : Expr
	{
		ReturnStmt(Parser::Pin pos, Expr* e) : Expr(pos),
			returnValue(e) { }

		virtual ~ReturnStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* returnValue = 0;
	};

	struct AllocStmt : Expr
	{
		AllocStmt(Parser::Pin pos) : Expr(pos) { }
		virtual ~AllocStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string type;
		std::deque<Expr*> counts;
		std::deque<Expr*> initArguments;
	};

	struct DeallocStmt : Expr
	{
		DeallocStmt(Parser::Pin pos, Expr* e) : Expr(pos),
			expr(e) { }

		virtual ~DeallocStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* expr = 0;
	};

	struct DeferredStmt : Expr
	{
		DeferredStmt(Parser::Pin pos, Expr* e) : Expr(pos),
			deferred(e) { }

		virtual ~DeferredStmt() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		Expr* deferred = 0;
	};

	struct BracedBlock : Expr
	{
		BracedBlock(Parser::Pin pos) : Expr(pos) { }
		virtual ~BracedBlock() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::deque<Expr*> statements;
		std::deque<DeferredStmt*> deferredStatements;
	};







	// declarations
	struct VarDecl : Expr
	{
		VarDecl(Parser::Pin pos, std::string n, bool immut) : Expr(pos),
			name(n), isImmutable(immut) { }

		virtual ~VarDecl() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
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
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
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





	// definitions
	struct FunctionDef : Expr
	{
		FunctionDef(Parser::Pin pos, FuncDecl* decl, BracedBlock* block) : Expr(pos),
			funcDecl(decl), funcBody(block) { }

		virtual ~FunctionDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		FuncDecl* funcDecl = 0;
		BracedBlock* funcBody = 0;
	};

	struct OpOverloadDef : Expr
	{
		OpOverloadDef(Parser::Pin pos, ArithmeticOp o) : Expr(pos),
			op(o) { }

		virtual ~OpOverloadDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		ArithmeticOp op;
		FunctionDef* function = 0;

		bool isBinaryOp = false;
		bool belongsToType = false;
		bool isCommutativeOp = false;

		uint64_t attributes = 0;
	};

	struct TypeAliasDef : Expr
	{
		TypeAliasDef(Parser::Pin pos) : Expr(pos) { }
		virtual ~TypeAliasDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string actualType;
		std::string newTypeName;

		bool isStrongAlias = false;
	};

	struct NamespaceDef : Expr
	{
		NamespaceDef(Parser::Pin pos, std::string n, BracedBlock* b) : Expr(pos),
			name(n), body(b) { }

		virtual ~NamespaceDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::string name;
		BracedBlock* body = 0;
	};









	// large things
	struct OpOverloadDef;
	struct CompoundType : Expr
	{
		CompoundType(Parser::Pin pos, std::string n) : Expr(pos),
			name(n) { }

		virtual ~CompoundType() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override = 0;

		bool didGenerateType = false;
		fir::Type* generatedType = 0;

		std::string name;
		std::deque<std::string> typeParameters;

		uint64_t attributes = 0;

		std::deque<VarDecl*> members;
		std::deque<OpOverloadDef*> operatorOverloads;
	};

	struct StructDef : CompoundType
	{
		StructDef(Parser::Pin pos, std::string n) : CompoundType(pos, n) { }
		virtual ~StructDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		bool isPackedStruct = false;
	};

	struct EnumerationDef : CompoundType
	{
		typedef std::pair<std::string, Expr*> EnumCase;

		EnumerationDef(Parser::Pin pos, std::string n) : CompoundType(pos, n) { }
		virtual ~EnumerationDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		bool isStrongEnumeration = false;

		std::deque<EnumCase> enumCases;
	};

	struct ClassPropertyDef;
	struct ClassDef : CompoundType
	{
		ClassDef(Parser::Pin pos, std::string n) : CompoundType(pos, n) { }
		virtual ~ClassDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		std::deque<FunctionDef*> functions;
		std::deque<ClassPropertyDef*> properties;

		std::deque<ClassDef*> nestedTypes;

		std::deque<std::string> conformingProtocols;
	};

	struct ExtensionDef : ClassDef
	{
		ExtensionDef(Parser::Pin pos, std::string n) : ClassDef(pos, n) { }
		virtual ~ExtensionDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
	};

	struct ClassPropertyDef : VarDecl
	{
		ClassPropertyDef(Parser::Pin pos, std::string n) : VarDecl(pos, n, true) { }
		virtual ~ClassPropertyDef() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;
		BracedBlock* getterDef = 0;
		BracedBlock* setterDef = 0;
		std::string setterArgName;
	};















	struct RootAst : Expr
	{
		RootAst(Parser::Pin pos);
		virtual ~RootAst() override { }
		virtual fir::Type* doTypecheck(TCInstance* ti) override;

		std::deque<Expr*> topLevelExpressions;
	};
}


















