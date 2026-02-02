#include "expression.h"

#ifndef M_PI
	#define M_PI 3.1415926535897932384626
#endif

#ifndef M_E
    #define M_E 2.71828182845904523536028747135266250
#endif

static Variable variables[MAX_VARIABLES];
static int variable_count = 0;

void set_variable(const char *name, double value)
{
	for (int i = 0; i < variable_count; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			variables[i].value = value;
			return;
		}
	}
	if (variable_count < MAX_VARIABLES) {
		strncpy(variables[variable_count].name, name, MAX_VAR_NAME - 1);
		variables[variable_count].name[MAX_VAR_NAME - 1] = '\0';
		variables[variable_count].value = value;
		variable_count++;
	}
}

void clear_variables(void)
{
	variable_count = 0;
}

static double *lookup_variable(const char *name)
{
	for (int i = 0; i < variable_count; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			return &variables[i].value;
		}
	}
	return NULL;
}

void skip_whitespace(char **s)
{
	while (**s == ' ') {
		(*s)++;
	}
}

double parse_number(char **s, CalcError *out)
{
	double number = 0;
	bool decimal = false;
	int decimal_level = 10;
	int added_numbers = 0;

	while (is_num(**s) || **s == '.') {
		if (**s == '.') {
			if (decimal) {
				*out = ERR_SYNTAX; // ERROR: duplicate decimal points
				return 0;
			}
			decimal = true;
			(*s)++;
			continue;
		}

		if (!decimal) {
			number = 10 * number + (**s - '0');
			added_numbers++;
		} else {
			number += (double)(**s - '0') / decimal_level;
			decimal_level *= 10;
			added_numbers++;
		}
		(*s)++;
	}
	
	if (added_numbers == 0) {
		*out = ERR_SYNTAX; // ERROR: Incorrect decimal form
		return 0;
	}

	return number;
}

double parse_primary(char **s, CalcError *out)
{
	double number = 0;

	skip_whitespace(s);
	
	if (**s == '(') {
		(*s)++;

		number = parse_expression(s, out);
		if (*out != SUCCESS) {
			return 0;
		}

		if (**s != ')') {
			*out = ERR_PAREN; // ERROR: Missing right parenthesis
			return 0;
		} else {
			(*s)++;
		}
	} else if ((**s >= '0' && **s <= '9') || **s == '.') {
		number = parse_number(s, out);

		if (*out != SUCCESS) {
			return 0;
		}
	} else if ((**s >= 'a' && **s <= 'z') || (**s >= 'A' && **s <= 'Z')) {
		char str[64];
		size_t i = 0;
		while ((**s >= 'a' && **s <= 'z') || (**s >= 'A' && **s <= 'Z')) {
			str[i] = tolower(**s);
			i++;
			(*s)++;
		}

		str[i] = '\0';

		/****/ if (strcmp(str, "pi") == 0) {
			number = M_PI;
		} else if (strcmp(str, "e") == 0) {
			number = M_E;
		} else if (strcmp(str, "tau") == 0) {
			number = M_PI * 2;
		} else if (strcmp(str, "sin") == 0) {
			number = sin(process_function_body(s, out));
		} else if (strcmp(str, "cos") == 0) {
			number = cos(process_function_body(s, out));
		} else if (strcmp(str, "tan") == 0) {
			number = tan(process_function_body(s, out));
		} else if (strcmp(str, "asin") == 0 || strcmp(str, "arcsin") == 0) {
			number = asin(process_function_body(s, out));
		} else if (strcmp(str, "acos") == 0 || strcmp(str, "arccos") == 0) {
			number = acos(process_function_body(s, out));
		} else if (strcmp(str, "atan") == 0 || strcmp(str, "arctan") == 0) {
			number = atan(process_function_body(s, out));
		} else if (strcmp(str, "floor") == 0) {
			number = floor(process_function_body(s, out));
		} else if (strcmp(str, "ceil") == 0) {
			number = ceil(process_function_body(s, out));
		} else if (strcmp(str, "sqrt") == 0) {
			number = sqrt(process_function_body(s, out));
		} else {
			double *var = lookup_variable(str);
			if (var != NULL) {
				number = *var;
			} else {
				*out = ERR_UNKNOWN; // ERROR: Function/Variable not found
				return 0;
			}
		}

		if (*out != SUCCESS) {
			return 0;
		}

	} else {
		*out = ERR_UNKNOWN; // ERROR: Ecountered unknown symbol
		return 0;
	}

	return number;
}

double process_function_body(char **s, CalcError *out)
{
	skip_whitespace(s);

	if (**s != '(') {
		*out = ERR_SYNTAX; // ERROR: Missing left function parenthesis
		return 0;
	}

	(*s)++;

	double function_body = parse_expression(s, out);

	if (*out != SUCCESS) {
		return 0;
	}

	if (**s != ')') {
		*out = ERR_PAREN; // ERROR: Missing right function parenthesis
		return 0;
	}

	(*s)++;

	return function_body;
}

double parse_power(char **s, CalcError *out)
{
	int sign = 1;

	if (**s == '-') {
		sign = -1;
		(*s)++;
	} else if (**s == '+') {
		(*s)++;
	}

	double base = parse_primary(s, out);

	if (*out != SUCCESS) {
		return 0;
	}

	double exponent = 0;

	skip_whitespace(s);

	if (**s == '^') {
		(*s)++;
		skip_whitespace(s);
		exponent = parse_power(s, out);

		if (*out != SUCCESS) {
			return 0;
		}
	} else {
		return sign * base;
	}

	if (*out != 0) return 0;

	return sign * pow(base, exponent);
}

bool is_implicit_separator(char s) {
	return s == '('
	    || ((s >= 'a' && s <= 'z') || (s <= 'A' && s >= 'Z'))
            || (s >= '0' && s <= '9');
}

bool is_term_separator(char s) {
	return s == '*' 
	    || s == '/'
		|| s == '%'
	    || is_implicit_separator(s);
}

bool is_num(char s) {
	return s >= '0' && s <= '9';
}

void handle_term_separation(char **s, CalcError *out, double *number, bool *i_checker)
{	
	if (**s == '*') {
		(*s)++;

		skip_whitespace(s);

		*i_checker = is_num(**s);

		*number *= parse_power(s, out);
	}
	else if (**s == '/') {
		(*s)++;

		skip_whitespace(s);

		*i_checker = is_num(**s);

		double rhs = parse_power(s, out);

		if (*out != SUCCESS) {
			return;
		}

		if (rhs == 0) {
			*out = ERR_DIV_ZERO; // ERROR: Divide by zero
			return;
		}

		*number /= rhs;
	}
	else if (**s == '%') {
		(*s)++;

		skip_whitespace(s);

		*i_checker = is_num(**s);

		double rhs = parse_power(s, out);

		if (*out != SUCCESS) {
			return;
		}

		if (rhs == 0) {
			*out = ERR_DIV_ZERO; // ERROR: Modulus by zero
			return;
		}

		*number = fmod(*number, rhs);
	}
	else if (is_implicit_separator(**s)) {
		// If current and previous are both numbers, Ex: "5 73"
		if (is_num(**s) && *i_checker) { // ERROR: Improper implicit multiplication
			*out = ERR_SYNTAX;
			return;
		}

		*i_checker = is_num(**s);

		*number *= parse_power(s, out);
	}

	skip_whitespace(s);
}

double parse_term(char **s, CalcError *out)
{
	bool i_checker = is_num(**s); // Checks for implicit multiplication syntax

	double number = parse_power(s, out);
	if (*out != SUCCESS) {
		return 0;
	}

	skip_whitespace(s);

	while (is_term_separator(**s))
	{
		handle_term_separation(s, out, &number, &i_checker);
		if (*out != SUCCESS) {
			return 0;
		}
	}

	return number;
}

double parse_expression(char **s, CalcError *out)
{
	double number = parse_term(s, out);

	if (*out != SUCCESS) return 0;

	skip_whitespace(s);

	while (**s == '+' || **s == '-') {
		if (**s == '+') {
			(*s)++;
			skip_whitespace(s);
			number += parse_term(s, out);
			if (*out != SUCCESS) return 0;
		}
		else {
			(*s)++;
			skip_whitespace(s);
			number -= parse_term(s, out);
			if (*out != SUCCESS) return 0;
		}

		skip_whitespace(s);
	}

	return number;
}
