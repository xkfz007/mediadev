#include "errorHandle.h"
#include <cstdlib>
void no_mem_exit(char *where)
{
	//snprintf(errortext, ET_SIZE, "Could not allocate memory: %s",where);
	error (where, 100);
}
void error(char *text, int code)
{
	fprintf(stderr, "%s\n", text);
	exit(code);
}