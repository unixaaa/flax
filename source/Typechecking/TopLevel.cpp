// TopLevel.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "compiler.h"
#include "typechecking.h"

#include "ir/module.h"

using namespace Ast;


RootAst::RootAst(Parser::Pin pos) : Expr(pos)
{
}

ModuleInfo::ModuleInfo(RootAst* rast)
{
	this->rootAst = rast;

	std::string fname = rast->posinfo.file;
	std::string modname;
	for(size_t i = 0; i < fname.size(); i++)
	{
		if(fname[i] == '/' || fname[i] == '\\')
			modname += "::";

		else if(fname[i] == '.' && fname.size() > i + 1 && fname.substr(i + 1).find("flx") == 0)
			break;

		else
			modname += fname[i];
	}

	this->fmodule = new fir::Module(modname);
}




















void RootAst::generateDependencies(TCInstance* ti)
{
	for(auto e : this->topLevelExpressions)
		e->generateDependencies(ti);
}

void BracedBlock::generateDependencies(TCInstance* ti)
{
	for(auto e : this->statements)
		e->generateDependencies(ti);

	for(auto e : this->deferredStatements)
		e->generateDependencies(ti);
}

void NamespaceDef::generateDependencies(TCInstance* ti)
{
	ti->pushNamespace(this->name);
	this->body->generateDependencies(ti);
	ti->popNamespace();
}






fir::Type* RootAst::doTypecheck(TCInstance* ti)
{
	for(auto e : this->topLevelExpressions)
		e->doTypecheck(ti);

	return 0;
}

fir::Type* BracedBlock::doTypecheck(TCInstance* ti)
{
	fir::Type* last = 0;
	for(auto e : this->statements)
		last = e->doTypecheck(ti);

	for(auto e : this->deferredStatements)
		e->doTypecheck(ti);

	// should return the value of the last expression.
	// regardless of its validity
	return last;
}


fir::Type* NamespaceDef::doTypecheck(TCInstance* ti)
{
	ti->pushNamespace(this->name);
	this->body->doTypecheck(ti);
	ti->popNamespace();


	return 0;
}



























