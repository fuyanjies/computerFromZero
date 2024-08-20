#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include <set>
#include <iostream>

namespace tokenizer {
enum token_type_t : unsigned char {
    KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST,
};

enum keyword_type_t : unsigned char {
    CLASS, METHOD, INT, FUNCTION, BOOLEAN, CONSTRUCTOR,
    CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN,
    TRUE, FALSE, NIL,// THIS, // NUL instead NULL bcz NULL has defined
};
}

namespace tokenizer {
struct tokenizer_t {
private :
    int token_idx;
    char *data;
    std::string_view content;
    std::vector<std::string_view> tokens;

    static const std::set<char> symbol;
    static const std::set<std::string_view> keyword;
    static const std::map<std::string_view, tokenizer::keyword_type_t> keyword_to_enum;

    void tokenize()
    {
        std::string_view::iterator left_iter = nullptr;
        std::string_view::iterator right_iter = this->content.begin();

        while (right_iter != this->content.end()) {
            for (; this->content.end() != right_iter && std::isspace(*right_iter); ++right_iter);
            if (this->content.end() == right_iter) break;
            if ('/' == *right_iter) {
                if (this->content.end() == (right_iter + 1)) {
                    throw std::logic_error("file end with / cannot understand");
                }
                if ('*' == *(right_iter + 1)) {
                    while (true) {
                        for (right_iter += 2; this->content.end() != right_iter && '*' != *right_iter; ++right_iter);
                        if (this->content.end() == (right_iter + 1)) {
                            throw std::logic_error("has readed /*(*) but not find */");
                        }
                        if ('/' == *(right_iter+1)) {
                            right_iter += 2;
                            break;
                        }
                    }
                    continue;
                } else if ('/' == *(right_iter + 1)) {
                    for (; this->content.end() != right_iter && '\n' != *right_iter; ++right_iter);
                    ++right_iter;
                    continue;
                }
            }

            if ('"' == *right_iter) {
                left_iter = right_iter++;
                for (; this->content.end() != right_iter && '"' != *right_iter; ++right_iter);
                if (this->content.end() == right_iter) {
                    throw std::logic_error("cannot find pair \"");
                }
                this->tokens.push_back({left_iter, ++right_iter});
                continue;
            }
            if (symbol.contains(*right_iter)) {
                this->tokens.push_back({right_iter, ++right_iter});
                continue;
            }

            left_iter = right_iter++;
            for (; this->content.end() != right_iter && !std::isspace(*right_iter) && !symbol.contains(*right_iter); ++right_iter);
            this->tokens.push_back({left_iter, right_iter});
        }

        this->token_idx = -1;
    }
public :
    token_type_t token_type;
    keyword_type_t keyword_type;

    tokenizer_t() = default;
    tokenizer_t(std::string input)
    {
        std::size_t content_size{};
        std::ifstream file{};

        file.open(input);

        file.seekg(0, std::ios::end);
        content_size = file.tellg();
        file.seekg(0, std::ios::beg);

        this->data = new char[content_size + 1];
        file.read(this->data, content_size);
        this->data[content_size] = 0;

        this->content = std::string_view(this->data, content_size);

        tokenize();
    }
    tokenizer_t(std::ifstream input)
    {
        std::size_t content_size{};
    
        input.seekg(0, std::ios::end);
        content_size = input.tellg();
        input.seekg(0, std::ios::beg);

        this->data = new char[content_size + 1];
        input.read(this->data, content_size);
        this->data[content_size] = 0;

        this->content = std::string_view(this->data, content_size);

        tokenize();
    }

    tokenizer_t(tokenizer_t &&other)
    {
        this->data = other.data;
        this->content = std::move(other.content);
        this->tokens = std::move(other.tokens);
        this->token_idx = other.token_idx;

        other.data = nullptr;
        other.token_idx = 0;
    }

    tokenizer_t &operator =(tokenizer_t &&other)
    {
        this->data = other.data;
        this->content = std::move(other.content);
        this->tokens = std::move(other.tokens);
        this->token_idx = other.token_idx;

        other.data = nullptr;
        other.token_idx = 0;

        return *this;
    }

    ~tokenizer_t()
    {
        if (nullptr != this->data) {
            delete [] this->data;
        }
    }

    std::string_view &peek()
    {
        return this->tokens[this->token_idx];
    }

    std::string_view &next()
    {
        std::string_view &cur = this->tokens[++this->token_idx];

        if ('"' == *cur.begin() && '"' == *(cur.end() - 1)) {
            this->token_type = token_type_t::STRING_CONST;
        } else if (std::isdigit(*cur.begin())) {
            for (char c : cur) {
                if (!std::isdigit(c)) throw std::logic_error("a identifier begin with digit");
            }
            this->token_type = token_type_t::INT_CONST;
        } else if (symbol.contains(*cur.begin())) {
            this->token_type = token_type_t::SYMBOL;
        } else if (keyword.contains(cur)) {
            this->token_type = token_type_t::KEYWORD;
            this->keyword_type = keyword_to_enum.find(cur)->second;
        } else {
            this->token_type = token_type_t::IDENTIFIER;
        }

        return cur;
    }

    void previous()
    {
        if (0 == this->token_idx) return;
        --this->token_idx;
    }

    bool has_more_token()
    {
        return this->token_idx != this->tokens.size() - 1;
    }

    std::string_view string_val()
    {
        if (token_type_t::STRING_CONST != this->token_type) {
            throw std::logic_error("this isn't string constant");
        }

        std::string_view &cur = this->peek();

        return {cur.begin()+1, cur.end()-1};
    }
};

const std::set<char> tokenizer_t::symbol{
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
    '-', '*', '/', '&', '|', '<', '>', '=', '~',
};

const std::set<std::string_view> tokenizer_t::keyword{
    "class", "constructor", "function", "method",
    "field", "static", "var", "int", "char", "boolean",
    "void", "true", "false", "null", "let",
    "do", "if", "else", "while", "return", // "this",
};

const std::map<std::string_view, tokenizer::keyword_type_t> tokenizer_t::keyword_to_enum{
{"class", tokenizer::CLASS}, {"method", tokenizer::METHOD},
{"int", tokenizer::INT}, {"function", tokenizer::FUNCTION},
{"boolean", tokenizer::BOOLEAN}, {"constructor", tokenizer::CONSTRUCTOR},
{"char", tokenizer::CHAR}, {"void", tokenizer::VOID},
{"static", tokenizer::STATIC}, {"field", tokenizer::FIELD},
{"let", tokenizer::LET}, {"do", tokenizer::DO},
{"if", tokenizer::IF}, {"else", tokenizer::ELSE},
{"while", tokenizer::WHILE}, {"return", tokenizer::RETURN},
{"true", tokenizer::TRUE}, {"false", tokenizer::FALSE},
{"null", tokenizer::NIL}, // {"this", tokenizer::THIS},
};
}