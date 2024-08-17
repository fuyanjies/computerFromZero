#include <iostream>
#include <string_view>
#include <string>
#include <set>
#include <filesystem>
#include <vector>
#include <fstream>
#include "compilation_engine.cpp"

int main(int argc, char *argv[])
{
    std::string s(argv[1]);
    tokenizer::tokenizer_t tok(argv[1]);

    tok.next();

    s.pop_back();
    int size = s.size()-1;
    s[size-2] = 'x';
    s[size-1] = 'm';
    s[size] = 'l';

    compilator::compilator_t(std::move(tok), s).compile_class();

    return 0;
}