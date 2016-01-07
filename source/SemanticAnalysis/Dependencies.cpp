// Dependencies.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "codegen.h"
#include "compiler.h"
#include "dependency.h"

#include <fstream>
#include <sstream>

using namespace Codegen;
using namespace Ast;

namespace Codegen
{
	static void stronglyConnect(DependencyGraph* graph, int& index, std::deque<std::deque<DepNode*>>& connected, DepNode* node);

	static std::pair<DepNode*, DepNode*> getNodes(DependencyGraph* g, std::string from, std::string to, Expr* e)
	{
		// find existing node
		DepNode* src = 0;
		DepNode* dst = 0;
		for(auto d : g->nodes)
		{
			if(!src && d->name == from)
			{
				src = d;
			}
			else if(!dst && d->name == to && to != "")
			{
				dst = d;
			}
		}

		if(!src)
		{
			src = new DepNode();
			src->name = from;

			g->nodes.push_back(src);
		}

		if(!dst)
		{
			dst = new DepNode();
			dst->name = to;

			g->nodes.push_back(dst);
		}

		dst->users.push_back({ src, e });

		return { src, dst };
	}

	void DependencyGraph::addDependency(std::string from, std::string to, ImportStmt* imp)
	{
		auto p = getNodes(this, from, to, imp);

		Dep* d = new Dep();
		d->type = DepType::Module;
		d->from = p.first;
		d->to = p.second;

		this->edgesFrom[p.first].push_back(d);
		this->edgesTo[p.second].push_back(d);
	}

	void DependencyGraph::addTypeDependency(std::string from, std::deque<std::string> possibleNames, Ast::Expr* user)
	{
		auto p = getNodes(this, from, "", user);

		Dep* d = new Dep();
		d->type = DepType::Type;
		d->from = p.first;
		d->to = p.second;

		p.second->alts = possibleNames;

		this->edgesFrom[p.first].push_back(d);
		this->edgesTo[p.second].push_back(d);
	}



	std::deque<std::deque<DepNode*>> DependencyGraph::findCyclicDependencies()
	{
		int index = 0;
		std::deque<std::deque<DepNode*>> ret;

		for(auto n : this->nodes)
		{
			n->index = -1;
			n->onStack = false;
			n->lowlink = -1;
		}

		for(auto n : this->nodes)
		{
			if(n->index == -1)
				stronglyConnect(this, index, ret, n);
		}

		return ret;
	}


	static void stronglyConnect(DependencyGraph* graph, int& index, std::deque<std::deque<DepNode*>>& connected, DepNode* node)
	{
		node->index = index;
		node->lowlink = index;

		index++;


		graph->stack.push(node);
		node->onStack = true;


		std::deque<Dep*> edges = graph->edgesFrom[node];
		for(auto edge : edges)
		{
			DepNode* w = edge->to;
			if(w->index == -1)
			{
				stronglyConnect(graph, index, connected, w);
				node->lowlink = (node->lowlink < w->lowlink ? node->lowlink : w->lowlink);
			}
			else if(w->onStack)
			{
				node->lowlink = (node->lowlink < w->index ? node->lowlink : w->index);
			}
		}



		if(node->lowlink == node->index)
		{
			std::deque<DepNode*> set;

			while(true)
			{
				DepNode* w = graph->stack.top();
				graph->stack.pop();

				w->onStack = false;

				set.push_back(w);

				if(w == node)
					break;
			}

			connected.push_back(set);
		}
	}


	std::deque<DepNode*> DependencyGraph::findDependenciesOf(Expr* expr)
	{
		std::deque<DepNode*> ret;
		for(auto e : this->edgesFrom)
		{
			if(e.first->expr == expr)
			{
				for(auto d : e.second)
					ret.push_back(d->to);
			}
		}

		return ret;
	}

	std::deque<DepNode*> DependencyGraph::findDependentsOf(std::string str)
	{
		// todo: this is definitely suboptimal
		std::deque<DepNode*> ret;
		for(auto e : this->edgesTo)
		{
			if(e.first->name != "" && e.first->name == str)
			{
				for(auto d : e.second)
				{
					iceAssert(d->to == e.first);
					ret.push_back(d->from);
				}
			}
			else
			{
				for(auto n : e.first->alts)
				{
					if(n == str)
					{
						for(auto d : e.second)
						{
							iceAssert(d->to == e.first);
							ret.push_back(d->from);
						}

						goto finish;
					}
				}

				finish:
				; // feel dirty
			}
		}

		return ret;
	}
}






































