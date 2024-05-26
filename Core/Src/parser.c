#include "parser.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>

ParsingResult parse(const char* msg){
	ParsingResult ret;
	ret.action = PARSER_ERROR;
	if(msg[0] == 'f'){
		if(msg[1] == '='){
			int f = atoi(msg+2);
			if (f < 1 || f > 100)
			{
				return ret;
			}
			ret.value = f;
			ret.action = FREQUENCY_SET;
			return ret;
		}
		else if(msg[1] == '?')
		{
			ret.action = FREQUENCY_READ;
			return ret;
		}
	}
	return ret;
}
