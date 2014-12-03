#ifndef PARAMPARSER_H
#define PARAMPARSER_H

class ParamParser
{
    int argc;
    char** argv;
public:
    ParamParser(int *argc, char ***argv);
    bool getParam(const char name[], char **value);
};

#endif // PARAMPARSER_H
