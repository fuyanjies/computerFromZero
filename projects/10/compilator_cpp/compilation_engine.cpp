#ifndef __COMP_H_
#define __COMP_H_

#include <string>
#include <string_view>
#include <fstream>
#include <map>

#include "jack_tokenizer.cpp"

namespace compilator {
struct compilator_t {
private :
    tokenizer::tokenizer_t tok;
    std::ofstream output;

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
        this->output << "<classVarDec>\n";
        this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
        this->tok.next();

        switch (this->tok.token_type) {
        case tokenizer::KEYWORD :
            this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
            break;
        case tokenizer::IDENTIFIER :
            this->output << "<identifier> " << this->tok.peek() << " </identifier>\n";
            break;
        }

        while (true) {
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            if (this->tok.next() == ";") {
                this->output << "<symbol> ; </symbol>\n";
                break;
            }
            this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        }

        this->tok.next();
        this->output << "</classVarDec>\n";
    }

    void compile_var_dec()
    {
        this->output << "<varDec>\n";
        this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
        this->tok.next();

        switch (this->tok.token_type) {
        case tokenizer::KEYWORD :
            this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
            break;
        case tokenizer::IDENTIFIER :
            this->output << "<identifier> " << this->tok.peek() << " </identifier>\n";
            break;
        }

        while (true) {
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            if (this->tok.next() == ";") {
                this->output << "<symbol> ; </symbol>\n";
                break;
            }
            this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        }

        this->tok.next();
        this->output << "</varDec>\n";
    }

    void compile_term()
    {
        this->output << "<term>\n";

        if (const std::string_view &cur = this->tok.peek(); cur == "-" || cur == "~") {
            this->output << "<symbol> " << cur << " </symbol>\n";
            this->tok.next();
            this->compile_term();
            goto end;
        }

        if (this->tok.peek() == "(") {
            this->output << "<symbol> ( </symbol>\n";
            this->tok.next();
            this->compile_expression();
            this->output << "<symbol> ) </symbol>\n";
            this->tok.next();
            goto end;
        }

        switch (this->tok.token_type) {
        case tokenizer::INT_CONST :
            this->output << "<integerConstant> " << this->tok.peek() << " </integerConstant>\n";
            break;
        case tokenizer::STRING_CONST :
            this->output << "<stringConstant> " << this->tok.string_val() << " </stringConstant>\n";
            break;
        case tokenizer::IDENTIFIER :
            this->output << "<identifier> " << this->tok.peek() << " </identifier>\n";
            break;
        case tokenizer::KEYWORD :
            this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
        }
        this->tok.next();

        if (this->tok.peek() == ".") {
            this->output << "<symbol> . </symbol>\n";
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            this->tok.next();
        }
        if (this->tok.peek() == "[") {
            this->output << "<symbol> [ </symbol>\n";
            this->tok.next();
            this->compile_expression();
            this->output << "<symbol> ] </symbol>\n";
            this->tok.next();
        }
        if (this->tok.peek() == "(") {
            this->output << "<symbol> ( </symbol>\n";
            this->tok.next();
            this->compile_expression_list();
            this->output << "<symbol> ) </symbol>\n";
            this->tok.next();
        }
    
    end :
        this->output << "</term>\n";
    }

    void compile_let()
    {
        this->output << "<letStatement>\n";

        this->output << "<keyword> let </keyword>\n";
        this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
        this->tok.next();
        if (this->tok.peek() == ".") {
            this->output << "<symbol> . </symbol>\n";
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            this->tok.next();
        }
        if (this->tok.peek() == "[") {
            this->output << "<symbol> [ </symbol>\n";
            this->tok.next();
            this->compile_expression();
            this->output << "<symbol> ] </symbol>\n";
            this->tok.next();
        }

        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->tok.next();

        this->compile_expression();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->tok.next();

        this->output << "</letStatement>\n";
    }

    void compile_expression()
    {
        this->output << "<expression>\n";
        
        while (true) {
            this->compile_term();
            if (const std::string_view &cur = this->tok.peek(); cur == "," || cur == ";" || cur == ")" || cur == "]") {
                break;
            }
            if (this->tok.peek() == "<") {
                this->output << "<symbol> &lt; </symbol>\n";
            } else if (this->tok.peek() == ">") {
                this->output << "<symbol> &gt; </symbol>\n";
            } else if (this->tok.peek() == "&") {
                this->output << "<symbol> &amp; </symbol>\n";
            } else {
                this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
            }
            this->tok.next();
        }

        this->output << "</expression>\n";
    }

    void compile_expression_list()
    {
        this->output << "<expressionList>\n";

        while (this->tok.peek() != ")") {
            this->compile_expression();
            if (this->tok.peek() == ",") {
                this->output << "<symbol> , </symbol>\n";
                this->tok.next();
            }
        }

        this->output << "</expressionList>\n";
    }

    void compile_while()
    {
        this->output << "<whileStatement>\n";

        this->output << "<keyword> while </keyword>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        this->compile_expression();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        this->compile_statements();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->tok.next();

        this->output << "</whileStatement>\n";
    }

    void compile_return()
    {
        this->output << "<returnStatement>\n";

        this->output << "<keyword> return </keyword>\n";
        if (this->tok.next() != ";") {
            this->compile_expression();
        }
        this->output << "<symbol> ; </symbol>\n";
        this->tok.next();

        this->output << "</returnStatement>\n";
    }

    void compile_if()
    {
        this->output << "<ifStatement>\n";

        this->output << "<keyword> if </keyword>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        this->compile_expression();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        this->compile_statements();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        if (this->tok.next() == "else") {
            this->output << "<keyword> else </keyword>\n";
            this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
            this->tok.next();
            this->compile_statements();
            this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
            this->tok.next();
        }

        this->output << "</ifStatement>\n";
    }

    void compile_do()
    {
        this->output << "<doStatement>\n";
        
        this->output << "<keyword> do </keyword>\n";
        this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
        
        this->tok.next();
        if (this->tok.peek() == ".") {
            this->output << "<symbol> . </symbol>\n";
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            this->tok.next();
        }
        if (this->tok.peek() == "[") {
            this->output << "<symbol> [ </symbol>\n";
            this->tok.next();
            this->compile_expression();
            this->output << "<symbol> ] </symbol>\n";
            this->tok.next();
        }
        if (this->tok.peek() == "(") {
            this->output << "<symbol> ( </symbol>\n";
            this->tok.next();
            this->compile_expression_list();
            this->output << "<symbol> ) </symbol>\n";
            this->tok.next();
        }

        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->tok.next();

        this->output << "</doStatement>\n";
    }

    void compile_parameter_list()
    {
        this->output << "<parameterList>\n";

        while (this->tok.peek() != ")") {
            switch (this->tok.token_type) {
            case tokenizer::KEYWORD :
                this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
                break;
            case tokenizer::IDENTIFIER :
                this->output << "<identifier> " << this->tok.peek() << " </identifier>\n";
                break;
            }
            this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
            if (this->tok.next() == ",") {
                this->output << "<symbol> , </symbol>\n";
                this->tok.next();
            }
        }

        this->output << "</parameterList>\n";
    }

    void compile_statements()
    {
        this->output << "<statements>\n";

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

        this->output << "</statements>\n";
    }

    void compile_subroutine()
    {
        this->output << "<subroutineDec>\n";

        this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
        this->tok.next();
        switch (this->tok.token_type) {
        case tokenizer::KEYWORD :
            this->output << "<keyword> " << this->tok.peek() << " </keyword>\n";
            break;
        case tokenizer::IDENTIFIER :
            this->output << "<identifier> " << this->tok.peek() << " </identifier>\n";
            break;
        }
        this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        this->compile_parameter_list();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->output << "<subroutineBody>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        while (this->tok.peek() == "var") {
            this->compile_var_dec();
        }
        this->compile_statements();
        this->output << "<symbol> " << this->tok.peek() << " </symbol>\n";
        this->tok.next();
        
        this->output << "</subroutineBody>\n";
        this->output << "</subroutineDec>\n";
    }

    void compile_class()
    {
        this->output << "<class>\n";

        this->output << "<keyword> class </keyword>\n";
        this->output << "<identifier> " << this->tok.next() << " </identifier>\n";
        this->output << "<symbol> " << this->tok.next() << " </symbol>\n";
        this->tok.next();
        while (this->tok.peek() != "}") {
            if (const std::string_view &cur = this->tok.peek(); cur == "static" || cur == "field") {
                this->compile_class_var_dec();
            } else {
                this->compile_subroutine();
            }
        }
        this->output << "<symbol> } </symbol>\n";
        
        this->output << "</class>\n";
    }
};
}

#endif