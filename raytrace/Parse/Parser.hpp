#ifndef PARSE_PARSER_HPP
#define PARSE_PARSER_HPP

#include "Parse/AST.h"

namespace Parse {

class Parser
{
public:
	static AST *parse(const char *filename);
};
}
#endif