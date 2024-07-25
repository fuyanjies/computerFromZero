module;

#include <string_view>
#include <ranges>

export module parse;

export enum command_type_t : unsigned char {
    C_ARITHETIC, C_PUSH, C_POP, C_LABEL,
    C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL,
};

export struct parse_t {
private :
    
}