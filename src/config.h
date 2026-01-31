#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR_LEN 256

typedef struct {
	int n_rects;
	double color_range;
	char dx_expr[MAX_EXPR_LEN];
} Config;

bool load_config(const char *path, Config *cfg);
