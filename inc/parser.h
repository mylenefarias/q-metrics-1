#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

#include "loader.h"

using namespace std;

enum LoaderMode
{
    MODE_VIDEO,
    MODE_TRAIN,
    MODE_PREDICT,
    MODE_UNKNOWN
};


struct CmdLineOpts
{

};




LoaderMode parseCmdLine(int argc, char *argv[], Loader lFile, CmdLineOpts opts);
void tokenize(const string & str,vector<string> & tokens,const string & delimiters = ";");

#endif // PARSER_H
