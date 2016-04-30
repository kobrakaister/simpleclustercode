#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int main(int argc, char *argv[])
{
printf("Clustering code\n");
if (strcmp(argv[1],"--head")==0)
{
	head();
}

if (strcmp(argv[1],"--node")==0)
{
	node();
}

}	
