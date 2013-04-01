#ifndef PARSER_HPP
#define PARSER_HPP

#include "Parse/AST.h"

namespace Parse {

class Parser
{
public:
	static AST *parse(const char *filename);
};
}
#endif