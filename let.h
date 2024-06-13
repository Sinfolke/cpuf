#pragma once

#ifndef CPUF_LET
#define CPUF_LET

#include <string>
#include <cstring>
#include <type_traits>
#include <iostream>
#include <cmath>
#ifdef CPUF_LET_PUBLICNULL
#define PRIVATE 
#define ENDPRIV 
#else
#define PRIVATE namespace {
#define ENDPRIV }
#endif
// hello 12345
template<typename A, typename B>
std::enable_if_t<std::is_floating_point_v<A> && std::is_floating_point_v<B> || std::is_integral_v<B>, A>
operator%(const A& lhs, const B& rhs) {
    return fmod(lhs, rhs);
}
template<typename A, typename B>
std::enable_if_t<std::is_floating_point_v<A> && std::is_floating_point_v<B> || std::is_integral_v<B>, A&>
operator%=(A& lhs, const B& rhs) {
    lhs = lhs % rhs;
    return lhs;
}

namespace std {
    template <typename T>
    constexpr const bool is_string_v() {
        return is_string<T>::value;
    }
}
/**
 * @brief Checks whether the given type is a string type
 * 
 * This structure is used to check whether the given type is a string type (char*, std::string, char [N]).
 *
 * @tparam T The type to be checked
 * @return A boolean value that is true if the type is a string type, and false otherwise.
 */
template <typename T> struct is_string {
    static const bool value = false;
};

// Specialization for char*
template <> struct is_string<char*> {
    static const bool value = true;
};

// Specialization for std::string
template <> struct is_string<std::string> {
    static const bool value = true;
};

// Specialization for char[]
template <size_t N> struct is_string<char[N]> {
    static const bool value = true;
};


class unexpected_type_exception : std::exception {
    public:
    unexpected_type_exception(const std::string op, const std::string unexpected) {
#if defined(DEBUG) || defined(CPUF_LET_DEBUG)
        std::cout << __FILE__ ": Unexpected type \"" << unexpected << "\" on [" << op << ']' << std::endl;
#endif
    }
    virtual const char* what() const throw() {
        return __FILE__ ": Unexpected type";
    }
};

PRIVATE 
    struct __Null {};
ENDPRIV
__Null Null;
class Type {
public:
    enum type {
        Int, Bool, Double, String, Null
    };

    Type(type t = type::Null) : _t(t) {}

    Type(const class let& data);

    operator type() const {
        return _t;
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    operator T() const {
        return _t;
    }
    char* str() const noexcept {
        switch (_t) {
            case type::String:
                return "String";
            case type::Int:
                return "Int";
            case type::Bool:
                return "Bool";
            case type::Double:
                return "Double";
            default:
                return "Null";            
        }
    }

private:
    type _t;
};
class let {
private:
    union DATA {
        std::string str;
        long long num;
        long double fl;

        DATA() : num(0) {} // Default constructor
        ~DATA() {} // Destructor

        /**
         * @brief Call the destructor of the string type if the union is explicitly string
         * 
         */
        void clear() {
            if constexpr (std::is_same_v<decltype(str), std::string>) {
                str.~basic_string();
            }
        }
    } data;

    /**
     * @brief called when you want to deallocate the stirng if it is allocated
     * 
     */
    void unstring() {
        if (type == Type::String) {
            str_destructor();
        }
    }
    /**
     * @brief just directly call the string destructor
     * 
     */
    void str_destructor() {
        data.str.~basic_string();
    }

    void ensureNotNull(const std::string op) const {
        if (type == Type::Null) {
            throw unexpected_type_exception(op, "Null");
        }
    }

    std::string toString() const {
        switch (type) {
            case Type::Bool: return data.num ? "true" : "false";
            case Type::Int: return std::to_string(data.num);
            case Type::Double: return std::to_string(data.fl);
            case Type::String: return data.str;
            default: return "Null";
        }
    }

public:
    Type type;

    // Default constructor
    let() : type(Type::Null) {}

    // Integral constructor
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_string_v<T>>>
    let(const T& input) {
        if constexpr(std::is_integral_v<T>()) {
            if (std::is_same_v<T, bool>())
                type = Type::Bool;
            else
                type = Type::Int;
            data.num = input;
        } else if constexpr (std::is_floating_point_v<T>()) {
            type = Type::Double;
            data.fl = static_cast<T>(input);
        } else {
            type = Type::String;
            new (&data.str) std::string(input); // Placement new to initialize union member
        }
    }
    let(const let& input) {
        type = input.type;
        if (type == Type::String) {
            new (&data.str) std::string(input.data.str);
        } else {
            data.num = input.data.num;
        }
    }
    let(const __Null) {}
    ~let() {
        if (type == Type::String) {
            data.str.~basic_string();
        }
    }

    // Assignment operators
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator=(const T& num) {
        unstring();
        if (std::is_same_v<T, bool>)
            type = Type::Bool;
        else
            type = Type::Int;
        data.num = num;
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator=(const T& input) {
        unstring();
        data.fl = static_cast<T>(input);
        type = Type::Double;
        return *this;
    }
    template<typename T>
    std::enable_if_t<std::is_string_v<T>, let&>
    operator=(const T& str) {
        if (type != Type::String) {
            new (&data.str) std::string(str);
            type = Type::String;
        } else  data.str = str;
        return *this;
    }

    // Copy assignment operator
    let& operator=(const let& other) {
        if (this != &other) {
            if (other.type == Type::String) {
                if (type != Type::String) {
                    new (&data.str) std::string(other.data.str);
                    type = Type::String;
                } else
                    data.str = other.data.str;
            } else {
                type = other.type;
                if (type < Type::Double)
                    data.num = other.data.num;
                else if (type == Type::Double)
                    data.fl = other.data.fl;
                // if none executed the type is null
            }

        }
        return *this;
    }

        // Compound assignment operators
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator+=(const T& other) {
        if (type == Type::String) {
            data.str += other;
        } else if (type < Type::Double) {
            data.num += other;
            if (type == Type::Bool)
                type = Type::Int;
        } else {
            data.fl += other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator+=(const T& other) {
        if (type == Type::String) {
            data.str += other;
        } else if (type < Type::Double) {
            double d = other + data;
            data.fl = d;
            type = Type::Double;
        } else {
            data.fl += other;
        }
        return *this;
    }
    template<typename T>
    std::enable_if_t<std::is_string_v<T>, let&>
        operator+=(const let& other) {
        if (other.type == Type::String) {
            if (type == Type::String) {
                data.str += other;
            } else {
                std::string thisstr = toString();
                thisstr += other;
                new (&data.str) std::string(thisstr);
                type = Type::String;
            }
        }
        return *this;
    }
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator-=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator-=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            data.num -= other;
            if (type == Type::Bool)
                type = Type::Int;
        } else {
            data.fl -= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator-=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator-=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            double d = other - data.num;
            data.fl = d;
            type = Type::Double;
        } else {
            data.fl -= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator*=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator*=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            data.num *= other;
            if (type == Type::Bool)
                type = Type::Int;
        } else {
            data.fl *= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator*=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator*=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            double d = other * data.num;
            data.fl = d;
            type = Type::Double;
        } else {
            data.fl *= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator/=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator/=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            data.num /= other;
            if (type == Type::Bool)
                type = Type::Int;
        } else {
            data.fl /= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator/=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator/=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            double d = other / data.num;
            data.fl = d;
            type = Type::Double;
        } else {
            data.fl /= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, let&>
    operator%=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator-=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            data.num %= other;
            if (type == Type::Bool)
                type = Type::Int;
        } else {
            data.fl %= other;
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, let&>
    operator%=(const T& other) {
        if (type >= Type::String) {
            throw unexpected_type_exception("operator-=", "[INT | BOOL, DOUBLE]]", type.str());
        }
        if (type < Type::Double) {
            double d = other * data.num;
            data.fl = d;
            type = Type::Double;
        } else {
            data.fl *= other;
        }
        return *this;
    }

    let& operator>>=(const let& other) {
        ensureNotNull("operator>>=");
        if (type == Type::String || other.type == Type::String) {
            throw std::runtime_error("Cannot use >>= with string types");
        } else if (type == Type::Int && other.type == Type::Int) {
            data.num >>= other.data.num;
        } else {
            throw std::runtime_error("Incompatible types for >>=");
        }
        return *this;
    }

    let& operator<<=(const let& other) {
        ensureNotNull("operator<<=");
        if (type == Type::String || other.type == Type::String) {
            throw std::runtime_error("Cannot use <<= with string types");
        } else if (type == Type::Int && other.type == Type::Int) {
            data.num <<= other.data.num;
        } else {
            throw std::runtime_error("Incompatible types for <<=");
        }
        return *this;
    }

    let& operator&=(const let& other) {
        ensureNotNull("operator&=");
        if (type == Type::String || other.type == Type::String) {
            throw std::runtime_error("Cannot use &= with string types");
        } else if (type == Type::Int && other.type == Type::Int) {
            data.num &= other.data.num;
        } else if (type == Type::Bool && other.type == Type::Bool) {
            data.num = data.num && other.data.num;
        } else {
            throw std::runtime_error("Incompatible types for &=");
        }
        return *this;
    }

    let& operator|=(const let& other) {
        ensureNotNull("operator|=");
        if (type == Type::String || other.type == Type::String) {
            throw std::runtime_error("Cannot use |= with string types");
        } else if (type == Type::Int && other.type == Type::Int) {
            data.num |= other.data.num;
        } else if (type == Type::Bool && other.type == Type::Bool) {
            data.num = data.num || other.data.num;
        } else {
            throw std::runtime_error("Incompatible types for |=");
        }
        return *this;
    }

    let& operator^=(const let& other) {
        ensureNotNull("operator^=");
        if (type == Type::String || other.type == Type::String) {
            throw std::runtime_error("Cannot use ^= with string types");
        } else if (type == Type::Int && other.type == Type::Int) {
            data.num ^= other.data.num;
        } else if (type == Type::Bool && other.type == Type::Bool) {
            data.num = data.num != other.data.num;
        } else {
            throw std::runtime_error("Incompatible types for ^=");
        }
        return *this;
    }

    template <typename Stream>
    friend Stream& operator<<(Stream& os, const let& obj) {
        switch (obj.type) {
            case Type::Bool:
                os << std::boolalpha;
            case Type::Int:
                os << obj.data.num;
                break;
            case Type::Double:
                os << obj.data.fl;
                break;
            case Type::String:
                os << obj.data.str;
                break;
            case Type::Null:
                os << "Null";
                break;
        }
        return os;
    }
    friend std::istream& operator>>(std::istream& is, let& obj) {
        std::string s;
        is >> s;
        if (s == "true") {
            obj = true;
        } else if (s == "false") {
            obj = false;
        } else {
            try {
                obj = std::stoi(s);
            } catch (const std::invalid_argument& e) {
                try {
                    obj = std::stod(s);
                } catch (const std::invalid_argument& e) {
                    obj = s;
                }
            }
        }
        return is;
    }
};
namespace std {
    std::istream& getline(std::istream& is, let& obj) {
        std::string s;
        getline(is, s);
        if (s == "true") {
            obj = true;
        } else if (s == "false") {
            obj = false;
        } else {
            try {
                obj = std::stoi(s);
            } catch (const std::invalid_argument& e) {
                try {
                    obj = std::stod(s);
                } catch (const std::invalid_argument& e) {
                    obj = s;
                }
            }
        }
        return is;
    };
}
using std::getline;
Type::Type(const let& data) : _t(data.type) {}

#endif // CPUF_LET