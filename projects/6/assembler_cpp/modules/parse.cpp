module;

#include <map>
#include <string>
#include <string_view>

export module parse;

export enum command_type_t : char {
    BLANK, A_COMMAND, C_COMMAND, L_COMMAND,
};

export struct parse_t {
private :
    std::string content;
    std::string_view command;
    std::map<std::string_view, unsigned int> symbol_table;
    command_type_t command_type;
};