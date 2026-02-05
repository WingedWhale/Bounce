#include "config.h"

static void trim(char *s)
{
	char *end = s + strlen(s) - 1;
	while (end > s && isspace((unsigned char)*end)) {
		*end = '\0';
		end--;
	}
	char *start = s;
	while (isspace((unsigned char)*start)) {
		start++;
	}
	if (start != s) {
		memmove(s, start, strlen(start) + 1);
	}
}

bool load_config(const char *path, Config *cfg)
{
	cfg->n_rects = 100;
	cfg->color_range = 0.6;
	strncpy(cfg->dx_expr, "10 * sin((n - i) / n * pi)", MAX_EXPR_LEN - 1);

	FILE *file = fopen(path, "r");
	if (!file) {
		return false;
	}

	char line[512];
	while (fgets(line, sizeof(line), file)) {
		
		// Skip comments and empty lines
		char *trimmed = line;
		while (isspace(*trimmed)) trimmed++;
		if (*trimmed == '#' || *trimmed == '\0' || *trimmed == '\n') {
			continue;
		}

		char *eq = strchr(line, '=');
		if (!eq) continue;

		*eq = '\0';
		char *key = line;
		char *value = eq + 1;

		trim(key);
		trim(value);

		if (strcmp(key, "n_rects") == 0) {
			int i_value = atoi(value);
			if (i_value > 0 && i_value <= 500) {
				cfg->n_rects = i_value;
			}
		} else if (strcmp(key, "color_range") == 0) {
			if (strcmp(value, "rainbow") == 0) {
				cfg->color_range = -1.0F;
			}
			else {
				double d_value = atof(value);
				if (d_value >= 0.0F && d_value <= 1.0F) {
					cfg->color_range = d_value;
				}
			}
		} else if (strcmp(key, "dx") == 0) {
			strncpy(cfg->dx_expr, value, MAX_EXPR_LEN - 1);
			cfg->dx_expr[MAX_EXPR_LEN - 1] = '\0';
		}
	}

	fclose(file);
	return true;
}
