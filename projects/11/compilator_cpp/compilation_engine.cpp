#include <string>
#include <string_view>
#include <fstream>
#include <map>

#include "jack_tokenizer.cpp"
#include "symbol_table.cpp"

namespace compilator {
struct compilator_t {
private :
    int while_count, if_count;
    symbol_table::symbol_t sym;
    tokenizer::tokenizer_t tok;
    std::ofstream output;
    std::string_view class_name;

public :
    compilator_t() = delete;
    compilator_t(tokenizer::tokenizer_t &&_tok, const std::string &file)
    {
        this->tok = std::move(_tok);
        this->output.open(file);
    }
    ~compilator_t()
    {
        if (this->output.is_open()) {
            this->output.close();
        }
    }

    void compile_class_var_dec()
    {
        symbol_table::kind_t kind;
        if (this->tok.peek() == "field") {
            kind = symbol_table::FIELD;
        } else {
            kind = symbol_table::STATIC;
        }

        std::string_view type = this->tok.next();

        while (true) {
            std::string_view name = this->tok.next();
            this->sym.define(name, type, kind);
            if (this->tok.next() == ";") {
                break;
            }
        }
        this->tok.next();
    }

    void compile_var_dec()
    {
        std::string_view type = this->tok.next();

        while (true) {
            std::string_view name = this->tok.next();
            this->sym.define(name, type, symbol_table::VAR);
            if (this->tok.next() == ";") {
                break;
            }
        }
        this->tok.next();
    }

    void compile_term()
    {
        if (this->tok.peek() == "-") {
            this->tok.next();
            this->compile_term();
            this->output << "neg\n";
            return ;
        } else if (this->tok.peek() == "~") {
            this->tok.next();
            this->compile_term();
            this->output << "not\n";
            return ;
        } else if (this->tok.peek() == "(") {
            this->tok.next();
            this->compile_expression();
            this->tok.next();
            return ;
        }

        if (tokenizer::INT_CONST == this->tok.token_type) {
            this->output << "push constant " << this->tok.peek() << "\n";
            this->tok.next();
            return ;
        } else if (tokenizer::STRING_CONST == this->tok.token_type) {
            this->output << "push constant " << this->tok.string_val().size() << "\n";
            this->output << "call String.new 1\n";
            for (char c : this->tok.string_val()) {
                this->output << "push constant " << static_cast<int>(c) << "\n";
                this->output << "call String.appendChar 2\n";
            }
            this->tok.next();
            return ;
        } else if (tokenizer::KEYWORD == this->tok.token_type) {
            if (this->tok.peek() == "null" || this->tok.peek() == "false") {
                this->output << "push constant 0\n";
            } else if (this->tok.peek() == "true") {
                this->output << "push constant 1\n";
                this->output << "neg\n";
            } else {
                throw std::logic_error("cannot understand keyword");
            }

            this->tok.next();
            return;
        } else if (tokenizer::IDENTIFIER == this->tok.token_type) {
            std::string_view cur = this->tok.peek();
            std::string_view next = this->tok.next();

            if (next == ".") {
                try {
                    std::string_view type = this->sym.type_of(cur);
                    std::string_view na = this->tok.next();

                    switch (this->sym.kind_of(cur)) {
                    case symbol_table::ARG :
                        this->output << "push argument " << this->sym.index_of(cur) << "\n";
                        break;
                    case symbol_table::VAR :
                        this->output << "push local " << this->sym.index_of(cur) << "\n";
                        break;
                    case symbol_table::FIELD :
                        this->output << "push this " << this->sym.index_of(cur) << "\n";
                        break;
                    case symbol_table::STATIC :
                        this->output << "push static " << this->sym.index_of(cur) << "\n";
                        break;
                    }
                    this->tok.next();
                    this->tok.next();
                    int s = this->compile_expression_list();

                    this->output << "call " << type << "." << na << " " << s+1 << "\n";
                    this->tok.next();
                    return ;
                } catch (...) {
                    std::string_view na = this->tok.next();

                    this->tok.next();
                    this->tok.next();
                    int s = this->compile_expression_list();
                    this->output << "call " << cur << "." << na << " " << s << "\n";
                    this->tok.next();
                    return ;
                }
            } else if (next == "[") {
                switch (this->sym.kind_of(cur)) {
                case symbol_table::ARG :
                    this->output << "push argument " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::VAR :
                    this->output << "push local " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::FIELD :
                    this->output << "push this " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::STATIC :
                    this->output << "push static " << this->sym.index_of(cur) << "\n";
                    break;
                }
                this->tok.next();
                this->compile_expression();
                this->output << "add\n";
                this->output << "pop pointer 1\n";
                this->output << "push that 0\n";
                this->tok.next();

                return ;
            } else if (next == "(") {
                this->output << "push pointer 0\n";
                this->tok.next();
                int s = this->compile_expression_list();
                this->output << "call " << this->class_name << "." << cur << " " << s+1 << "\n";
                this->tok.next();

                return ;
            } else {
                switch (this->sym.kind_of(cur)) {
                case symbol_table::ARG :
                    this->output << "push argument " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::VAR :
                    this->output << "push local " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::FIELD :
                    this->output << "push this " << this->sym.index_of(cur) << "\n";
                    break;
                case symbol_table::STATIC :
                    this->output << "push static " << this->sym.index_of(cur) << "\n";
                    break;
                }

                return ;
            }
        }
    }

    void compile_let()
    {
        std::string_view cur = this->tok.next();
        std::string_view next = this->tok.next();

        if (next == "[") {
            switch (this->sym.kind_of(cur)) {
            case symbol_table::ARG :
                this->output << "push argument " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::VAR :
                this->output << "push local " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::FIELD :
                this->output << "push this " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::STATIC :
                this->output << "push static " << this->sym.index_of(cur) << "\n";
                break;
            }
            this->tok.next();
            this->compile_expression();
            this->tok.next();
            this->tok.next();
            this->output << "add\n";
            this->output << "pop temp 1\n";
            this->compile_expression();
            this->tok.next();
            this->output << "push temp 1\n";
            this->output << "pop pointer 1\n";
            this->output << "pop that 0\n";
        } else {
            this->tok.next();
            this->compile_expression();
            this->tok.next();
            switch (this->sym.kind_of(cur)) {
            case symbol_table::ARG :
                this->output << "pop argument " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::VAR :
                this->output << "pop local " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::FIELD :
                this->output << "pop this " << this->sym.index_of(cur) << "\n";
                break;
            case symbol_table::STATIC :
                this->output << "pop static " << this->sym.index_of(cur) << "\n";
                break;
            }
        }
    }

    void compile_expression()
    {
        // ,;)]
        this->compile_term();

        while (true) {
            std::string_view sy = this->tok.peek();
            if (sy == "," || sy == ";" || sy == "]" || sy == ")") {
                break;
            }

            this->tok.next();
            this->compile_term();
            if (sy == "<") {
                this->output << "lt\n";
            } else if (sy == ">") {
                this->output << "gt\n";
            } else if (sy == "=") {
                this->output << "eq\n";
            } else if (sy == "+") {
                this->output << "add\n";
            } else if (sy == "-") {
                this->output << "sub\n";
            } else if (sy == "|") {
                this->output << "or\n";
            } else if (sy == "&") {
                this->output << "and\n";
            } else if (sy == "*") {
                this->output << "call Math.multiply 2\n";
            } else if (sy == "/") {
                this->output << "call Math.divide 2\n";
            }
        }
    }

    int compile_expression_list()
    {
        int count = 0;

        while (this->tok.peek() != ")") {
            this->compile_expression();
            ++count;

            if (this->tok.peek() == ")") {
                break;
            }
            this->tok.next();
        }

        return count;
    }

    void compile_while()
    {
        int while_count = this->while_count++;
        this->tok.next();
        this->tok.next();
        this->output << "label WHILE_EXP" << while_count << "\n";
        this->compile_expression();
        this->output << "not\n";
        this->output << "if-goto WHILE_END" << while_count << "\n";
        this->tok.next();
        this->tok.next();
        this->compile_statements();
        this->tok.next();
        this->output << "goto WHILE_EXP" << while_count << "\n";
        this->output << "label WHILE_END" << while_count << "\n";
    }

    void compile_return()
    {
        std::string_view cur = this->tok.next();

        if (cur == ";") {
            this->output << "push constant 0\n";
            this->output << "return\n";
            this->tok.next();
            return ;
        }

        this->compile_expression();
        this->output << "return\n";
        this->tok.next();
    }

    void compile_if()
    {
        int if_count = this->if_count++;
        this->tok.next();
        this->tok.next();
        this->compile_expression();
        this->tok.next();
        this->tok.next();
        this->output << "if-goto if_body." << if_count << "\n";
        this->output << "goto if_end." << if_count << "\n";
        this->output << "label if_body." << if_count << "\n";
        this->compile_statements();
        this->output << "goto all_if_end." << if_count << "\n";
        this->output << "label if_end." << if_count << "\n";
        if (this->tok.next() != "else") {
            this->output << "label all_if_end." << if_count << "\n";
            return ;
        }
        this->tok.next();
        this->tok.next();
        this->compile_statements();
        this->tok.next();
        this->output << "label all_if_end." << if_count << "\n";
    }

    void compile_do()
    {
        this->tok.next();
        this->compile_term();
        this->output << "pop temp 0\n";
        this->tok.next();
    }

    void compile_parameter_list()
    {
        while (this->tok.peek() != ")") {
            std::string_view type = this->tok.peek();
            std::string_view cur = this->tok.next();
            this->sym.define(cur, type, symbol_table::ARG);
            if (this->tok.next() == ")") {
                break;
            }
            this->tok.next();
        }
    }

    void compile_statements()
    {
        while (this->tok.peek() != "}") {
            if (this->tok.peek() == "do") {
                this->compile_do();
            } else if (this->tok.peek() == "let") {
                this->compile_let();
            } else if (this->tok.peek() == "if") {
                this->compile_if();
            } else if (this->tok.peek() == "while") {
                this->compile_while();
            } else if (this->tok.peek() == "return") {
                this->compile_return();
            }
        }
    }

    void compile_subroutine()
    {
        std::string_view key = this->tok.peek();

        this->sym.start_subroutine();
        this->while_count = 0;
        this->if_count = 0;

        if (key == "method") {
            this->sym.define("this", this->class_name, symbol_table::ARG);
        } else if (key == "constructor") {
            this->sym.define("this", this->class_name, symbol_table::VAR);
        }

        std::string_view rtype = this->tok.next();
        std::string_view fun = this->tok.next();
        this->tok.next();
        this->tok.next();
        this->compile_parameter_list();
        this->tok.next();
        this->tok.next();

        while (this->tok.peek() == "var") {
            this->compile_var_dec();
        }

        this->output << "function " << this->class_name << "." << fun << " " << this->sym.subroutine_scope_var_count << "\n";

        if (key == "method") {
            this->output << "push argument 0\n";
            this->output << "pop pointer 0\n";
        } else if (key == "constructor") {
            this->output << "push constant " << this->sym.class_scope_field_count << "\n";
            this->output << "call Memory.alloc 1\n";
            this->output << "pop local 0\n";
            this->output << "push local 0\n";
            this->output << "pop pointer 0\n";
        }
        this->compile_statements();
        this->tok.next();
    }

    void compile_class()
    {
        this->class_name = this->tok.next();
        this->tok.next();
        this->tok.next();

        while (this->tok.peek() != "}") {
            if (const std::string_view &cur = this->tok.peek(); cur == "static" || cur == "field") {
                this->compile_class_var_dec();
            } else {
                this->compile_subroutine();
            }
        }
    }
};
}