// parser.h
// Copyright (c) 2014 - 2015, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include "ast.h"
#include <string>
#include <sstream>
#include <deque>
#include <algorithm>

namespace Codegen
{
	struct CodegenInstance;
}

namespace Ast
{
	struct RootAst;
}

enum class ArithmeticOp;
namespace Parser
{
	enum class TType
	{
		// keywords
		Invalid,
		Func,
		Class,
		Import,
		Var,
		Val,
		Ptr,
		Deref,
		Addr,
		If,
		Else,
		Return,
		As,
		Is,
		Switch,
		Case,
		Match,
		To,
		Enum,
		ForeignFunc,
		Struct,
		Static,
		True,
		False,

		For,
		While,
		Do,
		Loop,
		Defer,

		Break,
		Continue,

		Get,
		Set,

		Private,
		Public,
		Internal,

		Extension,
		TypeAlias,

		Typeof,
		Alloc,
		Dealloc,

		Module,
		Namespace,
		Override,

		// symbols
		LBrace,
		RBrace,
		LParen,
		RParen,
		LSquare,
		RSquare,
		LAngle,
		RAngle,
		Plus,
		Minus,
		Asterisk,
		Divide,
		SQuote,
		DQuote,
		Period,
		Comma,
		Colon,
		Equal,
		Question,
		Exclamation,
		Semicolon,
		Ampersand,
		Percent,
		Pipe,
		LogicalOr,
		LogicalAnd,
		At,
		Pound,
		Tilde,


		// compound symbols
		Arrow,
		EqualsTo,
		NotEquals,
		GreaterEquals,
		LessThanEquals,
		ShiftLeft,
		ShiftRight,
		DoublePlus,			// doubleplusgood
		DoubleMinus,
		PlusEq,
		MinusEq,
		MultiplyEq,
		DivideEq,
		ModEq,
		ShiftLeftEq,
		ShiftRightEq,
		Elipsis,
		DoubleColon,

		// other stuff.
		Identifier,
		Integer,
		Decimal,
		StringLiteral,

		NewLine,
		Comment,
	};


	struct Token
	{
		Token() { }

		Pin pin;
		std::string text;
		TType type = TType::Invalid;
	};




	void parserError(const char* msg, ...) __attribute__((noreturn));
	void parserWarn(const char* msg, ...);


	struct ParserState;
	void parserError(ParserState& ps, Token tok, const char* msg, ...) __attribute__((noreturn));
	void parserWarn(ParserState& ps, Token tok, const char* msg, ...);

	void parserError(ParserState& ps, const char* msg, ...) __attribute__((noreturn));
	void parserWarn(ParserState& ps, const char* msg, ...);

	void parserError(Token tok, const char* msg, ...) __attribute__((noreturn));
	void parserWarn(Token tok, const char* msg, ...);

	void parserError(Pin po, const char* msg, ...) __attribute__((noreturn));
	void parserWarn(Pin po, const char* msg, ...);


	typedef std::deque<Token> TokenList;
	struct ParserState
	{
		explicit ParserState(Codegen::CodegenInstance* c) : cgi(c) { }

		TokenList tokens;

		std::map<std::string, bool> visited;

		Token curtok;
		Pin currentPos;
		uint64_t curAttrib = 0;
		Ast::RootAst* rootNode = 0;

		Codegen::CodegenInstance* cgi = 0;

		bool isParsingStruct = false;
		bool didHaveLeftParen = false;
		int currentOpPrec = 0;


		Token front()
		{
			return this->tokens.front();
		}

		void pop_front()
		{
			this->tokens.pop_front();
		}

		Token eat()
		{
			// returns the current front, then pops front.
			if(this->tokens.size() == 0)
				parserError(*this, "Unexpected end of input");

			this->skipNewline();
			Token t = this->front();
			this->pop_front();

			this->skipNewline();

			this->curtok = t;
			return t;
		}

		void skipNewline()
		{
			// eat newlines AND comments
			while(this->tokens.size() > 0 && (this->tokens.front().type == TType::NewLine
				|| this->tokens.front().type == TType::Comment || this->tokens.front().type == TType::Semicolon))
			{
				this->tokens.pop_front();
				this->currentPos.line++;
			}
		}
	};








	void parseAll(ParserState& tokens);
	void parseAttribute(ParserState& ps);
	Ast::Expr* parsePrimary(ParserState& tokens);


	std::string parseType(ParserState& ps);



	// expressions
	Ast::Expr*				parseExpr(ParserState& ps);
	Ast::Typeof*			parseTypeofExpr(ParserState& ps);
	Ast::Expr*				parseIdentifierExpr(ParserState& ps);
	Ast::Expr*				parseParenthesisedExpr(ParserState& ps);
	Ast::FuncCall*			parseFuncCall(ParserState& ps, std::string id);
	Ast::Expr*				parseBinOpRhs(ParserState& ps, Ast::Expr* expr, int prio);

	// literals
	Ast::ArrayLiteral*		parseArrayLiteral(ParserState& ps);
	Ast::StringLiteral*		parseStringLiteral(ParserState& ps);
	Ast::NumberLiteral*		parseNumberLiteral(ParserState& ps);
	Ast::BooleanLiteral*	parseBooleanLiteral(ParserState& ps);
	Ast::TupleLiteral*		parseTupleLiteral(ParserState& ps, Ast::Expr* lhs);



	// statements
	Ast::AllocStmt*			parseAlloc(ParserState& ps);
	Ast::BreakStmt*			parseBreak(ParserState& ps);
	Ast::IfStmt*			parseIfStmt(ParserState& ps);
	Ast::DeallocStmt*		parseDealloc(ParserState& ps);
	Ast::DeferredStmt*		parseDeferred(ParserState& ps);
	Ast::WhileLoop*			parseWhileLoop(ParserState& ps);
	Ast::ImportStmt*		parseImportStmt(ParserState& ps);
	Ast::ReturnStmt*		parseReturnStmt(ParserState& ps);
	Ast::BracedBlock*		parseBracedBlock(ParserState& ps);
	Ast::ContinueStmt*		parseContinueStmt(ParserState& ps);



	// declarations
	Ast::VarDecl*			parseVarDecl(ParserState& ps);
	Ast::FuncDecl*			parseFuncDecl(ParserState& ps);
	Ast::Expr*				parseStaticDecl(ParserState& ps);
	Ast::FuncDecl*			parseForeignFuncDecl(ParserState& ps);


	// definitions
	Ast::NamespaceDef*		parseNamespace(ParserState& ps);

	// types
	Ast::ClassDef*			parseClass(ParserState& ps);
	Ast::StructDef*			parseStruct(ParserState& ps);
	Ast::ExtensionDef*		parseExtension(ParserState& ps);
	Ast::EnumerationDef*	parseEnumeration(ParserState& ps);
	Ast::TypeAliasDef*		parseTypeAliasDef(ParserState& ps);

	// funcs
	Ast::FunctionDef*		parseFunction(ParserState& ps);
	Ast::OpOverloadDef*		parseOpOverloadDef(ParserState& ps);
	Ast::FunctionDef*		parseInitialiserFuncDef(ParserState& ps);









	// Ast::Expr*				parseType(ParserState& ps);
	// Ast::CompoundType*		parseEnum(ParserState& ps);
	// Ast::Func*				parseFunc(ParserState& ps);
	// Ast::Expr*				parseExpr(ParserState& ps);
	// Ast::Expr*				parseUnary(ParserState& ps);
	// Ast::Expr*				parseIdExpr(ParserState& ps);
	// Ast::Typeof*			parseTypeof(ParserState& ps);
	// Ast::Number*			parseNumber(ParserState& ps);
	// Ast::Dealloc*			parseDealloc(ParserState& ps);
	// Ast::Expr*				parseInitFunc(ParserState& ps);
	// void					parseAttribute(ParserState& ps);
	// Ast::TypeAlias*			parseTypeAlias(ParserState& ps);
	// Ast::Extension*			parseExtension(ParserState& ps);
	// Ast::NamespaceDecl*		parseNamespace(ParserState& ps);
	// Ast::Expr*				parseStaticDecl(ParserState& ps);
	// Ast::OpOverload*		parseOpOverload(ParserState& ps);
	// Ast::BracedBlock*		parseBracedBlock(ParserState& ps);
	// Ast::ForeignFuncDecl*	parseForeignFunc(ParserState& ps);
	// Ast::Func*				parseTopLevelExpr(ParserState& ps);
	// Ast::ArrayLiteral*		parseArrayLiteral(ParserState& ps);
	// Ast::StringLiteral*		parseStringLiteral(ParserState& ps);
	// Ast::Tuple*				parseTuple(ParserState& ps, Ast::Expr* lhs);
	// Ast::Expr*				parseRhs(ParserState& ps, Ast::Expr* expr, int prio);



	Ast::RootAst* Parse(ParserState& pstate, std::string filename);
	void parseAllCustomOperators(ParserState& pstate, std::string filename, std::string curpath);




	std::string getModuleName(std::string filename);
	Token getNextToken(std::string& stream, Pin& pos);

	std::string arithmeticOpToString(Codegen::CodegenInstance*, ArithmeticOp op);
	ArithmeticOp mangledStringToOperator(Codegen::CodegenInstance*, std::string op);
	std::string operatorToMangledString(Codegen::CodegenInstance*, ArithmeticOp op);
}

















