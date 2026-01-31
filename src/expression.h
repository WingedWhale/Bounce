#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
	SUCCESS = 0,
	ERR_DIV_ZERO = 1,
	ERR_SYNTAX = 2,
	ERR_PAREN = 3,
	ERR_UNKNOWN = 4
} CalcError;

#define MAX_VARIABLES 16
#define MAX_VAR_NAME 32

typedef struct {
	char name[MAX_VAR_NAME];
	double value;
} Variable;

void set_variable(const char *name, double value);
void clear_variables(void);

double get_constant(char *constant, CalcError *out);

double parse_number(char **s, CalcError *out);
double parse_primary(char**s, CalcError *out);
double process_function_body(char **s, CalcError *out);
double parse_power(char**s, CalcError *out);
bool is_num(char s);
bool is_implicit_separator(char s);
bool is_term_separator(char s);
void handle_term_separation(char **s, CalcError *out, double *number, bool *is_num);
double parse_term(char**s, CalcError *out);
double parse_expression(char**s, CalcError *out);