#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include <ucl.h>

#define USAGE "usage: treeload directory file\n"

void
errexit(int code, char *fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(code);
}

typedef struct Walkstate Walkstate;
struct Walkstate {
	const ucl_object_t *obj;
	int index;
	const char *key;
	ucl_type_t type;
	char *path;
} walkstatezero = {
	.obj = NULL,
	.index = 0,
	.key = NULL,
	.type = UCL_NULL,
	.path = ""
};

void
mkdir(char *path)
{
	printf("mkdir -p %s\n", path);
}

void
mkfile(char *path, const char *data, size_t dlen)
{
	(void)dlen;
	printf("echo '%s' > %s\n", data, path);
}

void
leaf(const ucl_object_t *obj, Walkstate *parent)
{
	char *p;

	if(parent->type == UCL_ARRAY)
		asprintf(&p, "%s/%d", parent->path, parent->index);
	else
		asprintf(&p, "%s/%s", parent->path, ucl_object_key(obj));

	mkfile(p, ucl_object_tostring_forced(obj), -1);
	free(p);
}

void
inner(const ucl_object_t *obj, Walkstate *parent)
{
	Walkstate this = walkstatezero;
	const ucl_object_t *cur = NULL;
	ucl_object_iter_t it = NULL;

	this.key = ucl_object_key(obj);
	this.type = ucl_object_type(obj);
	if(this.key != NULL)
		asprintf(&this.path, "%s/%s", parent->path, this.key);
	else
		asprintf(&this.path, "%s", parent->path);
	if(parent->type == UCL_ARRAY){
		free(this.path);
		asprintf(&this.path, "%s/%d", parent->path, parent->index);
	}
	mkdir(this.path);

	it = ucl_object_iterate_new(obj);
	while((cur = ucl_object_iterate_safe(it, true)) != NULL){
		switch(ucl_object_type(cur)){
		case UCL_OBJECT:
		case UCL_ARRAY:
			inner(cur, &this); break;
		default:
			leaf(cur, &this); break;
		}
		this.index += 1;
	}
	ucl_object_iterate_free(it);
	free(this.path);
}

int
main(int argc, char *argv[])
{
	char *dn = NULL, *fn = NULL;
	struct ucl_parser *parser = NULL;
	ucl_object_t *obj = NULL;
	
	if(argc != 3) errexit(111, USAGE);
	dn = argv[1];
	fn = argv[2];

	if((parser = ucl_parser_new(0)) == NULL)
		errexit(100, "can't allocate a parser\n");
	if(ucl_parser_add_file(parser, fn) == false)
		errexit(100, "%s\n", ucl_parser_get_error(parser));
	if((obj = ucl_parser_get_object(parser)) == NULL)
		errexit(100, "%s\n", ucl_parser_get_error(parser));

	{
		Walkstate root = walkstatezero;

		root.path = dn;
		mkdir(root.path);
		inner(obj, &root);
	}
	exit(0);
}
