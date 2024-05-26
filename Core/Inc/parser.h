/*
 * parser.h
 *
 *  Created on: May 19, 2024
 *      Author: Jakub Kaszowski
 */

#ifndef PARSER_H
#define PARSER_H


typedef enum{
	PARSER_ERROR,
	FREQUENCY_SET,
	FREQUENCY_READ
} Action;

typedef struct{
	Action action;
	int value;
}ParsingResult;


ParsingResult parse(const char* msg);


#endif
