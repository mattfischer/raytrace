#include "Parse/Parser.hpp"

extern "C"
{
	AST *parseScene(const char *filename);
}

namespace Parse {

AST *Parser::parse(const char *filename)
{
	return parseScene(filename);
}

}