#include "paramparser.h"
#include <string.h>
#include <stdio.h>

ParamParser::ParamParser(int *argc, char ***argv):
    argc(*argc),
    argv(*argv)
{
}

bool ParamParser::getParam(const char name[], char **value)
{
    for(int i = 0; i < argc; ++i) {
        if (strcmp(name, argv[i]) == 0) {
            *value = argv[i + 1];
            return true;
        }
    }
    value = NULL;
    return false;
}
