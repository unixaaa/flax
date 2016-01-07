// CompilerDriver.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <iostream>
#include <fstream>
#include <cassert>
#include <fstream>
#include <cstdlib>
#include <cinttypes>

#include <sys/stat.h>
#include "parser.h"
#include "codegen.h"
#include "compiler.h"
#include "dependency.h"
#include "typechecking.h"


#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Bitcode/ReaderWriter.h"

using namespace Ast;

namespace Compiler
{
	std::string resolveImport(ImportStmt* imp, std::string fullPath)
	{
		std::string curpath = getPathFromFile(fullPath);

		if(imp->moduleIdentifier.find("*") != (size_t) -1)
		{
			Parser::parserError("Wildcard imports are currently not supported (trying to import %s)", imp->moduleIdentifier.c_str());
		}

		// first check the current directory.
		std::string modname = imp->moduleIdentifier;
		for(size_t i = 0; i < modname.length(); i++)
		{
			if(modname[i] == '.')
				modname[i] = '/';
		}

		std::string name = curpath + "/" + modname + ".flx";
		char* fname = realpath(name.c_str(), 0);

		// a file here
		if(fname != NULL)
		{
			auto ret = std::string(fname);
			free(fname);
			return getFullPathOfFile(ret);
		}
		else
		{
			free(fname);
			std::string builtinlib = getSysroot() + getPrefix() + modname + ".flx";

			struct stat buffer;
			if(stat(builtinlib.c_str(), &buffer) == 0)
			{
				return getFullPathOfFile(builtinlib);
			}
			else
			{
				std::string msg = "No module or library with the name '" + modname + "' could be found (no such builtin library either)";

				va_list ap;
				__error_gen(imp->posinfo.line + 1 /* idk why */, imp->posinfo.col, imp->posinfo.len,
					getFilenameFromPath(fullPath).c_str(), msg.c_str(), "Error", true, ap);

				abort();
			}
		}
	}




	#if 0

	static void cloneCGIInnards(Codegen::CodegenInstance* from, Codegen::CodegenInstance* to)
	{
		to->typeMap					= from->typeMap;
		to->customOperatorMap		= from->customOperatorMap;
		to->customOperatorMapRev	= from->customOperatorMapRev;
		to->globalConstructors		= from->globalConstructors;
	}

	static void copyRootInnards(Codegen::CodegenInstance* cgi, RootAst* from, RootAst* to, bool doClone)
	{
		using namespace Codegen;

		for(auto v : from->typeList)
		{
			bool skip = false;
			for(auto k : to->typeList)
			{
				if(std::get<0>(k) == std::get<0>(v))
				{
					skip = true;
					break;
				}
			}

			if(skip)
				continue;

			to->typeList.push_back(v);
		}


		if(doClone)
		{
			cgi->cloneFunctionTree(from->rootFuncStack, to->rootFuncStack, false);
			cgi->cloneFunctionTree(from->publicFuncTree, to->publicFuncTree, false);
		}
	}
	#endif













	ModuleInfo* doCompilation(std::string fullpath, ModuleInfo* mi)
	{
		TCInstance* ti = doTypechecking(mi);
		iceAssert(ti != 0);

		CGInstance* ci = doCodegen(mi, ti);
		iceAssert(ci != 0);

		return mi;
	}






	static void copyModuleInfo(ModuleInfo* from, ModuleInfo* to)
	{
		to->customOperatorMap		= from->customOperatorMap;
		to->customOperatorMapRev	= from->customOperatorMapRev;
	}

	static std::pair<ModuleInfo*, std::string> _compileFile(std::string fpath, ModuleInfo* rmi)
	{
		using namespace Codegen;
		using namespace Parser;

		std::string curpath = Compiler::getPathFromFile(fpath);


		// parse
		ParserState pstate;
		RootAst* root = Parser::Parse(pstate, fpath);
		(void) root;


		ModuleInfo* mi = new ModuleInfo(root);
		copyModuleInfo(rmi, mi);	// copy from the root to the new one

		iceAssert(doCompilation(fpath, mi) == mi);

		copyModuleInfo(mi, rmi);	// copy back new info


		size_t lastdot = fpath.find_last_of(".");
		std::string oname = (lastdot == std::string::npos ? fpath : fpath.substr(0, lastdot));
		oname += ".bc";

		return { mi, oname };
	}




















	static void _resolveImportGraph(Codegen::DependencyGraph* g, std::unordered_map<std::string, bool>& visited, std::string currentMod,
		std::string curpath)
	{
		using namespace Parser;

		// NOTE: make sure resolveImport **DOES NOT** use codegeninstance, cuz it's 0.
		ParserState fakeps;

		fakeps.currentPos.file = currentMod;
		fakeps.currentPos.line = 1;
		fakeps.currentPos.col = 1;
		fakeps.currentPos.len = 1;

		fakeps.tokens = Compiler::getFileTokens(currentMod);

		while(fakeps.tokens.size() > 0)
		{
			Token t = fakeps.front();
			fakeps.pop_front();

			if(t.type == TType::Import)
			{
				// hack: parseImport expects front token to be "import"
				fakeps.tokens.push_front(t);

				ImportStmt* imp = parseImportStmt(fakeps);

				std::string file = Compiler::getFullPathOfFile(Compiler::resolveImport(imp, Compiler::getFullPathOfFile(currentMod)));

				g->addDependency(currentMod, file, imp);

				if(!visited[file])
				{
					visited[file] = true;
					_resolveImportGraph(g, visited, file, curpath);
				}
			}
		}
	}

	static Codegen::DependencyGraph* resolveImportGraph(std::string baseFullPath, std::string curpath)
	{
		using namespace Codegen;
		DependencyGraph* g = new DependencyGraph();

		std::unordered_map<std::string, bool> visited;
		_resolveImportGraph(g, visited, baseFullPath, curpath);

		return g;
	}











	std::tuple<RootAst*, std::vector<std::string>, std::unordered_map<std::string, RootAst*>, std::unordered_map<std::string, fir::Module*>>
	compileFile(std::string filename, std::map<ArithmeticOp, std::pair<std::string, int>> foundOps, std::map<std::string, ArithmeticOp> foundOpsRev)
	{
		using namespace Codegen;

		filename = getFullPathOfFile(filename);
		std::string curpath = getPathFromFile(filename);

		DependencyGraph* g = resolveImportGraph(filename, curpath);

		std::deque<std::deque<DepNode*>> groups = g->findCyclicDependencies();

		for(auto gr : groups)
		{
			if(gr.size() > 1)
			{
				std::string modlist;
				std::deque<Expr*> imps;

				for(auto m : gr)
				{
					std::string fn = getFilenameFromPath(m->name);
					fn = fn.substr(0, fn.find_last_of('.'));

					modlist += "\t" + fn + "\n";
				}

				info("Cyclic import dependencies between these modules:\n%s", modlist.c_str());
				info("Offending import statements:");

				for(auto m : gr)
				{
					for(auto u : m->users)
					{
						va_list ap;

						__error_gen(u.second->posinfo.line + 1 /* idk why */, u.second->posinfo.col, u.second->posinfo.len,
							getFilenameFromPath(u.first->name).c_str(), "", "Note", false, ap);
					}
				}

				error("Cyclic dependencies found, cannot continue");
			}
		}

		std::vector<std::string> outlist;
		std::unordered_map<std::string, RootAst*> rootmap;
		std::unordered_map<std::string, fir::Module*> modulemap;


		RootAst* dummyRoot = new RootAst(Parser::Pin());
		ModuleInfo* mi = new ModuleInfo(dummyRoot);

		// if groups.size == 0, then we didn't import anything.
		if(groups.size() == 0)
		{
			std::deque<DepNode*> fakes;
			DepNode* dn = new DepNode();
			dn->name = filename;

			fakes.push_back(dn);
			groups.push_back(fakes);
		}


		for(auto gr : groups)
		{
			iceAssert(gr.size() == 1);
			std::string name = Compiler::getFullPathOfFile(gr.front()->name);

			auto pair = _compileFile(name, mi);

			outlist.push_back(pair.second);
			modulemap[name] = /* cgi->module */ 0;
			rootmap[name] = /* cgi->rootNode */ 0;
		}

		return std::make_tuple(rootmap[Compiler::getFullPathOfFile(filename)], outlist, rootmap, modulemap);
	}















































	void writeBitcode(std::string oname, llvm::Module* module)
	{
		std::error_code e;
		llvm::sys::fs::OpenFlags of = (llvm::sys::fs::OpenFlags) 0;
		llvm::raw_fd_ostream rso(oname.c_str(), e, of);

		llvm::WriteBitcodeToFile(module, rso);
		rso.close();
	}


	void compileProgram(llvm::Module* module, std::vector<std::string> filelist, std::string foldername, std::string outname)
	{
		std::string tgt;
		if(!getTarget().empty())
			tgt = "-target " + getTarget();


		if(!Compiler::getIsCompileOnly() && !module->getFunction("main"))
		{
			error(0, "No main() function, a program cannot be compiled.");
		}



		std::string oname = outname.empty() ? (foldername + "/" + module->getModuleIdentifier()).c_str() : outname.c_str();
		// compile it by invoking clang on the bitcode
		char* inv = new char[1024];
		// snprintf(inv, 1024, "llvm-link -o '%s.bc'", oname.c_str());
		// std::string llvmlink = inv;
		// for(auto s : filelist)
		// 	llvmlink += " '" + s + "'";

		// system(llvmlink.c_str());

		llvm::verifyModule(*module, &llvm::errs());
		Compiler::writeBitcode(oname + ".bc", module);

		memset(inv, 0, 1024);
		{
			int opt = Compiler::getOptimisationLevel();
			const char* optLevel	= (Compiler::getOptimisationLevel() >= 0 ? ("-O" + std::to_string(opt)) : "").c_str();
			const char* mcmodel		= (getMcModel().empty() ? "" : ("-mcmodel=" + getMcModel())).c_str();
			const char* isPic		= (getIsPositionIndependent() ? "-fPIC" : "");
			const char* target		= (tgt).c_str();
			const char* outputMode	= (Compiler::getIsCompileOnly() ? "-c" : "");

			snprintf(inv, 1024, "clang++ -Wno-override-module %s %s %s %s %s -o '%s' '%s.bc'", optLevel, mcmodel, target,
				isPic, outputMode, oname.c_str(), oname.c_str());
		}

		std::string final = inv;

		// todo: clang bug, http://clang.llvm.org/doxygen/CodeGenAction_8cpp_source.html:714
		// that warning is not affected by any flags I can pass
		// besides, LLVM itself should have caught everything.

		// edit: fixed now with -Wno-override-module
		// clang shouldn't output anything when it shouldn't,
		// but we should still get linking errors etc.

		// if(!Compiler::getPrintClangOutput())
			// final += " &>/dev/null";

		system(final.c_str());
		delete[] inv;
	}




	std::string getPathFromFile(std::string path)
	{
		std::string ret;

		size_t sep = path.find_last_of("\\/");
		if(sep != std::string::npos)
			ret = path.substr(0, sep);

		return ret;
	}

	std::string getFilenameFromPath(std::string path)
	{
		std::string ret;

		size_t sep = path.find_last_of("\\/");
		if(sep != std::string::npos)
			ret = path.substr(sep + 1);

		return ret;
	}

	std::string getFullPathOfFile(std::string partial)
	{
		const char* fullpath = realpath(partial.c_str(), 0);
		iceAssert(fullpath);

		std::string ret = fullpath;
		free((void*) fullpath);

		return ret;
	}
}


































namespace GenError
{
	void printContext(std::string file, uint64_t line, uint64_t col, uint64_t len)
	{
		std::vector<std::string> lines = Compiler::getFileLines(file);
		if(lines.size() > line - 1)
		{
			std::string orig = lines[line - 1];
			std::string ln;

			for(auto c : orig)
			{
				if(c == '\t')
				{
					for(size_t i = 0; i < TAB_WIDTH; i++)
						ln += " ";
				}
				else
				{
					ln += c;
				}
			}


			fprintf(stderr, "%s\n", ln.c_str());

			for(uint64_t i = 1; i < col - 1; i++)
			{
				if(ln[i - 1] == '\t')
				{
					for(size_t i = 0; i < TAB_WIDTH; i++)
						fprintf(stderr, " ");
				}
				else
				{
					fprintf(stderr, " ");
				}
			}

			std::string tildes;
			// for(size_t i = 0; i < len; i++)
			// 	tildes += "~";

			fprintf(stderr, "%s^%s%s", COLOUR_GREEN_BOLD, tildes.c_str(), COLOUR_RESET);
		}
		else
		{
			fprintf(stderr, "(no context)");
		}
	}

	void printContext(Expr* e)
	{
		if(e->posinfo.line > 0)
			printContext(e->posinfo.file, e->posinfo.line, e->posinfo.col, e->posinfo.len);
	}
}


void __error_gen(uint64_t line, uint64_t col, uint64_t len, std::string file, const char* msg,
	const char* type, bool doExit, va_list ap)
{
	if(strcmp(type, "Warning") == 0 && Compiler::getFlag(Compiler::Flag::NoWarnings))
		return;


	char* alloc = nullptr;
	vasprintf(&alloc, msg, ap);

	auto colour = COLOUR_RED_BOLD;
	if(strcmp(type, "Warning") == 0)
		colour = COLOUR_MAGENTA_BOLD;

	else if(strcmp(type, "Note") == 0)
		colour = COLOUR_GREY_BOLD;

	// todo: do we want to truncate the file path?
	// we're doing it now, might want to change (or use a flag)

	std::string filename = Compiler::getFilenameFromPath(file);

	if(line > 0 && col > 0)
		fprintf(stderr, "%s(%s:%" PRIu64 ":%" PRIu64 ") ", COLOUR_BLACK_BOLD, filename.c_str(), line, col);

	fprintf(stderr, "%s%s%s: %s\n", colour, type, COLOUR_RESET, alloc);

	if(line > 0 && col > 0)
	{
		std::vector<std::string> lines;
		if(!file.empty())
		{
			GenError::printContext(file, line, col, len);
		}
	}

	fprintf(stderr, "\n");

	va_end(ap);
	free(alloc);

	if(doExit)
	{
		fprintf(stderr, "There were errors, compilation cannot continue\n");
		abort();
	}
	else if(strcmp(type, "Warning") == 0 && Compiler::getFlag(Compiler::Flag::WarningsAsErrors))
	{
		error("Treating warning as error because -Werror was passed");
	}
}

void error(Expr* relevantast, const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	std::string file	= relevantast ? relevantast->posinfo.file : "";
	uint64_t line		= relevantast ? relevantast->posinfo.line : 0;
	uint64_t col		= relevantast ? relevantast->posinfo.col : 0;
	uint64_t len		= relevantast ? relevantast->posinfo.len : 0;

	__error_gen(line, col, len, file, msg, "Error", true, ap);
	va_end(ap);
	abort();
}

void error(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	__error_gen(0, 0, 0, "", msg, "Error", true, ap);
	va_end(ap);
	abort();
}







void warn(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	__error_gen(0, 0, 0, "", msg, "Warning", false, ap);
	va_end(ap);
}

void warn(Expr* relevantast, const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	std::string file	= relevantast ? relevantast->posinfo.file : "";
	uint64_t line		= relevantast ? relevantast->posinfo.line : 0;
	uint64_t col		= relevantast ? relevantast->posinfo.col : 0;
	uint64_t len		= relevantast ? relevantast->posinfo.len : 0;

	__error_gen(line, col, len, file, msg, "Warning", false, ap);
	va_end(ap);
}



void info(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	__error_gen(0, 0, 0, "", msg, "Note", false, ap);
	va_end(ap);
}

void info(Expr* relevantast, const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	std::string file	= relevantast ? relevantast->posinfo.file : "";
	uint64_t line		= relevantast ? relevantast->posinfo.line : 0;
	uint64_t col		= relevantast ? relevantast->posinfo.col : 0;
	uint64_t len		= relevantast ? relevantast->posinfo.len : 0;

	__error_gen(line, col, len, file, msg, "Note", false, ap);
	va_end(ap);
}




































