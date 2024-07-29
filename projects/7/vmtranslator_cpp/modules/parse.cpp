module;

#include <string>
#include <string_view>
#include <fstream>
#include <ranges>

export module parse;

export namespace parse {
enum command_type_t : unsigned char {
    C_BLANK, 
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
};
}