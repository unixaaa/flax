// dependency.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <map>
#include <deque>
#include <stack>
#include <string>


namespace Ast
{
	struct Expr;
	struct Root;
	struct ImportStmt;
}

namespace Codegen
{
	enum class DepType
	{
		Invalid,
		Function,
		Module,
		Type,
	};

	struct DepNode
	{
		Ast::Expr* expr = 0;
		std::string name;

		std::deque<std::string> alts;

		// mainly to aid error reporting
		std::deque<std::pair<DepNode*, Ast::Expr*>> users;

		int index = -1;
		int lowlink = -1;
		bool onStack = false;
	};

	struct Dep
	{
		DepNode* from = 0;
		DepNode* to = 0;

		DepType type;
	};

	struct DependencyGraph
	{
		std::deque<DepNode*> nodes;
		std::map<DepNode*, std::deque<Dep*>> edgesFrom;
		std::map<DepNode*, std::deque<Dep*>> edgesTo;

		std::stack<DepNode*> stack;


		void addDependency(std::string from, std::string to, Ast::ImportStmt* imp);
		void addTypeDependency(std::string from, std::deque<std::string> possibleNames, Ast::Expr* user);

		std::deque<std::deque<DepNode*>> findCyclicDependencies();

		std::deque<DepNode*> findDependenciesOf(Ast::Expr* expr);
		std::deque<DepNode*> findDependentsOf(std::string str);
	};
}

namespace SemAnalysis
{
	// Codegen::DependencyGraph* resolveDependencyGraph(Codegen::CodegenInstance* cgi, Ast::RootAst* root);
}




























