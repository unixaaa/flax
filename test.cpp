#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cctype>

enum class State: char {
	LParen = '('
	, RParen = '('
	, Symbol
	, Identifier
	, Number
	, Whitespace
	, Comment
	, None
};

typedef std::tuple<std::string, State, int, int> Token;

Token make_token(std::string token, State state, int line, int column) {
	return std::make_tuple(token, state, line, column);
}

std::vector<Token> lex(std::string expr) {
	std::vector<Token> tokens;
	std::string token = "";
	State state = State::None;
	int line = 1;
	int column = 1;

	for(char next: expr) {
		if(next == '\n' and state == State::Comment) {
			tokens.push_back(make_token(token, state, line, column));
			token = "";
			state = State::None;
		}

		if(state == State::Comment) {
			token += next;
		}

		if(next == '#') {
			if(token != "") {
				tokens.push_back(make_token(token, state, line, column));
				token = "";
			}

			state = State::Comment;
		}
	}

	return tokens;
}

std::vector<Token> evaluate(std::string expr) {
	return lex(expr);
}

std::string input(std::string prompt, std::istream *in, std::ostream *out) {
	(*out) << prompt;
	std::string line;
	std::getline(*in, line);
	return line;
}

int main(int argc, char *argv[]) {
	std::istream *in = &std::cin;
	std::ostream *out = &std::cout;
	std::ostream *err = &std::cerr;

	while(*in) {
		try {
			(*out) << evaluate(input("[~>] ", in, out)) << std::endl;
		}

		catch(...) {
			(*out) << "error" << std::endl;
		}
	}
}