#include <map>
#include <string>
#include <string_view>
#include <stdexcept>

namespace symbol_table {
enum kind_t : unsigned char {
    STATIC, FIELD, ARG, VAR,
};

struct scope_t {
    kind_t kind;
    int sharp;
    std::string_view type;
};

struct symbol_t {
    std::map<std::string_view, scope_t> class_scope{};
    std::map<std::string_view, scope_t> subroutine_scope{};
    int class_scope_field_count{0};
    int class_scope_static_count{0};
    int subroutine_scope_var_count{0};
    int subroutine_scope_arg_count{0};

    void start_subroutine()
    {
        this->subroutine_scope.clear();
        this->subroutine_scope_arg_count = 0;
        this->subroutine_scope_var_count = 0;
    }

    void define(std::string_view name, std::string_view type, kind_t kind)
    {
        switch (kind) {
        case STATIC :
            if (this->class_scope.find(name) != this->class_scope.end()) {
                throw std::logic_error("redeclare static");
            }
            this->class_scope.insert({name, {kind, this->class_scope_static_count++, type}});
            break;
        case FIELD :
            if (this->class_scope.find(name) != this->class_scope.end()) {
                throw std::logic_error("redeclare field");
            }
            this->class_scope.insert({name, {kind, this->class_scope_field_count++, type}});
            break;
        case ARG :
            if (this->subroutine_scope.find(name) != this->subroutine_scope.end()) {
                throw std::logic_error("redeclare arg");
            }
            this->subroutine_scope.insert({name, {kind, this->subroutine_scope_arg_count++, type}});
            break;
        case VAR :
            if (this->subroutine_scope.find(name) != this->subroutine_scope.end()) {
                throw std::logic_error("redeclare var");
            }
            this->subroutine_scope.insert({name, {kind, this->subroutine_scope_var_count++, type}});
            break;
        }
    }

    int count(kind_t kind)
    {
        switch (kind) {
        case FIELD :
            return this->class_scope_field_count;
        case STATIC :
            return this->class_scope_static_count;
        case ARG :
            return this->subroutine_scope_arg_count;
        case VAR :
            return this->subroutine_scope_var_count;
        }
    }

    kind_t kind_of(std::string_view name)
    {
        std::map<std::string_view, scope_t>::const_iterator find = 
            this->subroutine_scope.find(name);
        
        if (find != this->subroutine_scope.end()) {
            return find->second.kind;
        }

        find = this->class_scope.find(name);

        if (find != this->class_scope.end()) {
            return find->second.kind;
        }

        throw std::logic_error("have not already been declared");
    }

    std::string_view type_of(std::string_view name)
    {
        std::map<std::string_view, scope_t>::const_iterator find = 
            this->subroutine_scope.find(name);
        
        if (find != this->subroutine_scope.end()) {
            return find->second.type;
        }

        find = this->class_scope.find(name);

        if (find != this->class_scope.end()) {
            return find->second.type;
        }

        throw std::logic_error("have not already been declared");
    }

    int index_of(std::string_view name)
    {
        std::map<std::string_view, scope_t>::const_iterator find = 
            this->subroutine_scope.find(name);
        
        if (find != this->subroutine_scope.end()) {
            return find->second.sharp;
        }

        find = this->class_scope.find(name);

        if (find != this->class_scope.end()) {
            return find->second.sharp;
        }

        throw std::logic_error("have not already been declared");
    }
};
}