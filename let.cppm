export module cpuf.let;
import cpuf.color;
import std;
import std.compat;

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

template <> struct is_string<char> {
    static const bool value = true;
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
    unexpected_type_exception(std::string op, std::string unexpected) {
#if defined(DEBUG) || defined(CPUF_LET_DEBUG)
        std::cout << __FILE__ ": Unexpected _type \"" << unexpected << "\" on [" << op << ']' << std::endl;
#endif
    }
    unexpected_type_exception(std::string op, std::string first, std::string second, std::string expected, std::string got, bool which) {
#if defined(DEBUG) || defined(CPUF_LET_DEBUG)
        std::cout << __FILE__ ": Incompatible _types <";
        if (which == 0)
            std::cout << color::red << first << color::reset << "> " << op << " <" << second << ">: expected " << expected;
        else
            std::cout << first << "> " << op << " <" << color::red << second << color::reset << ">: expected " << expected;
#endif
    }
    virtual const char* what() const throw() {
        return __FILE__ ": Unexpected _type";
    }
};
template<typename T>
struct _LONG_LONG_OR_T {
    using type = T;
};

// Specialization for int
template<>
struct _LONG_LONG_OR_T<int> {
    using type = long long;
};
template<>
struct _LONG_LONG_OR_T<long> {
    using type = long long;
};
struct __Null {};
export __Null Null;
export class let;
class Type {
public:
    enum type {
        Int, Bool, Double, String, Null
    };

    Type(type t = type::Null) : _t(t) {}

    Type(const let& data);

    operator type() const {
        return _t;
    }

    template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
    operator T() const {
        return _t;
    }
    const char* str() const noexcept {
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
    friend std::ostream& operator<<(std::ostream& out, const Type& type) {
        out << type.str();
        return out;
    }
private:
    type _t;
};
class let {
protected:
    union DATA {
        std::string str;
        long long num;
        long double fl;

        DATA() : num(0) {} // Default constructor
        ~DATA() {} // Destructor
    } data;
    Type _type;
public:
    /**
     * @brief If the data is string the str_destructor is called
     * 
     */
    void unstring() {
        if (_type == Type::String) {
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
    /**
     * @brief Convert the property data to a string
     * @return std::string 
     */
    std::string toString() const noexcept {
        if (_type == Type::Bool)
            return data.num != 0 ? "true" : "false"; // Ensure explicit comparison for boolean logic
        else if (_type == Type::Int)
            return std::to_string(data.num); // Explicitly cast to int
        else if (_type == Type::Double)
            return std::to_string(data.fl); // Explicitly cast to double
        else if (_type == Type::String)
            return data.str; // Explicitly construct std::string if necessary
        else
            return "Null";
    }

    /**
     * @brief Construct the std::string class with your value to data. _type is not changed
     * 
     * @tparam T
     * @param value 
     */
    template<typename T>
    inline void str_constructor(const T& value) {
        new (&data.str) std::string(value);
    }
    inline void str_constructor() {
        new (&data.str) std::string();
    }
    /**
     * @brief Construct the std::string class with your value and change the _type to Type::String
     * 
     * @tparam T
     * @param value 
     */
    template<typename T>
    inline void makeString(const T& value) {
        str_constructor(value);
        _type = Type::String;
    }

    /**
     * @brief Sets the string to desired value. If _type is not string, makeString() is called and _type set to Type::String
     * 
     * @tparam T 
     * @param value 
     */
    template <typename T>
    inline void setString(const T& value) {
        if (_type == Type::String) {
            data.str = value;
        } else {
            makeString(value);
        }
    }
    /**
     * @brief Add string if the data is already a string. Otherwise makeString( toString() + value) is called and _type set to Type::String
     * 
     * @tparam T 
     * @param value 
     */
    template<typename T>
    inline void addString(const T& value) {
        if (_type == Type::String) {
            data.str = value;
        } else {
            makeString(toString() + value);
        }
    }
    // methods

    auto type() const noexcept {
        return _type;
    }

    template<typename TYPE>
    _LONG_LONG_OR_T<TYPE> cast() const {
        if (std::is_same<TYPE, int>::value || std::is_same<TYPE, long long>::value) {
            #ifdef DEBUG
            if (_type == Type::Bool)
                std::cerr << "\nlet::cast<int/long long> throws a warning: the type is bool but you're casting to int/long long" << std::endl;
            else
            #endif
            if (_type != Type::Int) {
                throw std::runtime_error("let::cast<int/long long> throws an error: your type is not Int");
            }
            return data.num;
        } else if (std::is_same<TYPE, bool>::value) {
            #ifdef DEBUG
            if (_type == Type::Int) {
                std::cerr << "\nlet::cast<bool> throws a warning: the type is int but you're casting to bool" << std::endl;
                return data.num;
            } else
            #endif
            if (_type != Type::Bool) {
                throw std::runtime_error("let::cast<bool> throws an error: your type is not Bool");
            }
            return data.num;
        } else if (std::is_same<TYPE, double>::value) {
            if (_type != Type::Double) {
                throw std::runtime_error("let::cast<double> throws an error: your type is not Double");
            }
            return data.fl;
        } else if (std::is_same<TYPE, std::string>::value) {
            if (_type != Type::String) {
                throw std::runtime_error("let::cast<std::string> throws an error: your type is not String");
            }
            return data.str;
        } else if (is_string<TYPE>::value) {
            if (_type != Type::String) {
                throw std::runtime_error("let::cast<const char*> throws an error: your type is not String");
            }
            return data.str.c_str();
        } else {
            throw std::runtime_error("undefined cast type");
        }

    }
    // a simpler way to check for the type
    bool isInt() { return _type == Type::Int; }
    bool isBool() { return _type == Type::Bool; }
    bool isDouble() { return _type == Type::Double; }
    bool isNumeric() { return _type < Type::String; }
    bool isString() { return _type == Type::String; }
    bool isNull() { return _type == Type::Null; }

    /**
     * @brief Returns a reference to the data union. Please be careful.
     * 
     * @return auto& 
     */
    auto& ref() {
        return data;
    }
    /**
     * @brief returns a reference to the type so you could manually adjust it. Please keep in mind if type == string you should call destructor before change
     * 
     * @return auto& 
     */
    auto& reft() {
        return _type;
    }
    /**
     * @brief Assign the string element converting to the corresponding type is possible. If not the value assigned as a string type
     * 
     * @param el 
     * @return Type 
     */
    // Type convertible_assign(const char* el) {
    // }

    let() : _type(Type::Null) {}
    let(const __Null) : _type(Type::Null) {}

    let(long long input) : _type(Type::Int) {
        data.num = input;
    }
    let(unsigned long long input) : _type(Type::Int) {
        data.num = input;
    }
    let(long input) : _type(Type::Int) {
        data.num = input;
    }
    let(unsigned long input) : _type(Type::Int) {
        data.num = input;
    }
    let(int input) : _type(Type::Int) {
        data.num = input;
    }
    let(unsigned int input) : _type(Type::Int) {
        data.num = input;
    }
    let(short input) : _type(Type::Int) {
        data.num = input;
    }
    let(unsigned short input) : _type(Type::Int) {
        data.num = input;
    }
    let(char input) : _type(Type::Int) {
        data.num = input;
    }
    let(unsigned char input) : _type(Type::Int) {
        data.num = input;
    }
    let(long double input) : _type(Type::Double) {
        data.fl = input;
    }
    let(double input) : _type(Type::Double) {
        data.fl = input;
    }
    let(float input) : _type(Type::Double) {
        data.fl = input;
    }
    let(const std::string& input) {
        makeString(input);
    }
    let(const char* input) {
        makeString(input);
    }
    let(const let& input) {
        if (input._type == Type::String) {
            str_constructor(input.data.str);
        } else {
            data.num = input.data.num;
        }
        _type = input._type;
    }
    ~let() {
        unstring();
    }
    // Assignment operators
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator=(const T& num) {
        unstring();
        if (std::is_same<T, bool>::value)
            _type = Type::Bool;
        else
            _type = Type::Int;
        data.num = num;
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator=(const T& input) {
        unstring();
        data.fl = static_cast<T>(input);
        _type = Type::Double;
        return *this;
    }
    template<typename T>
    typename std::enable_if<is_string<T>::value, let&>::type
    operator=(const T& str) {
        setString(str);
        return *this;
    }

    // Copy assignment operator
    let& operator=(const let& other) {
        if (this == &other) return *this;

        if (other._type == Type::String) {
            setString(other.data.str);
        } else {
            _type = other._type;
            data.num = other.data.num;
        }
        return *this;
    }

        // Compound assignment operators
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator+=(const T& other) {
        if (_type == Type::String) {
            data.str += other;
        } else if (_type < Type::Double) {
            data.num += other;
            _type = Type::Int;
        } else {
            data.fl += other;
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator+=(const T& other) {
        if (_type == Type::String) {
            data.str += other;
        } else if (_type < Type::Double) {
            auto fl = data.num + other;
            data.fl = fl;
            _type = Type::Double;
        } else {
            data.fl += other;
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<is_string<T>::value, let&>::type
    operator+=(const let& other) {
        addString(other);
        return *this;
    }
    let& operator+=(const let& other) {
        if (this == &other) return *this;
        if (other._type == Type::String) {
            makeString(toString() + other.data.str);
        } else if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num += other.data.num;
                _type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl + data.num;
                data.fl = fl;
                _type = Type::Double; 
            } else {
                throw unexpected_type_exception("+=", "(INT | BOOL)", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (_type == Type::Double) {
            if (other._type < Type::Double) {
                data.fl += other.data.num;
            } else if (other._type == Type::Double) {
                data.fl += other.data.fl;
            } else {
                throw unexpected_type_exception("+=", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else if (_type == Type::String) {
            data.str += other.toString();
        } else {
            throw unexpected_type_exception("+=", _type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", _type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator-=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("-=", _type.str(), "Int | Bool", "(INT | BOOL | DOUBLE)", _type.str(), 0);
        }
        if (_type < Type::Double) {
            data.num -= other;
            _type = Type::Int;
        } else {
            data.fl -= other;
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator-=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("-=", _type.str(), "Int | Bool", "(INT | BOOL | DOUBLE)", _type.str(), 0);
        }
        if (_type < Type::Double) {
            double d = data.num - other;
            data.fl = d;
            _type = Type::Double;
        } else {
            data.fl -= other;
        }
        return *this;
    }
    let& operator-=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num -= other.data.num;
                _type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl - data.num;
                data.fl = fl;
                _type = Type::Double; 
            } else {
                throw unexpected_type_exception("-=", "(INT | BOOL)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (_type == Type::Double) {
            if (other._type < Type::Double) {
                data.fl -= other.data.num;
            } else if (other._type == Type::Double) {
                data.fl -= other.data.fl;
            } else {
                throw unexpected_type_exception("-=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("-=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator*=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("*=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", "String", 0);
        }
        if (_type < Type::Double) {
            data.num *= other;
            _type = Type::Int;
        } else {
            data.fl *= other;
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator*=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("*=", "String", "Int | Bool", "[INT | BOOL, DOUBLE]]", "String", 0);
        }
        if (_type < Type::Double) {
            double d = other * data.num;
            data.fl = d;
            _type = Type::Double;
        } else {
            data.fl *= other;
        }
        return *this;
    }
    let& operator*=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num *= other.data.num;
                _type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl * data.num;
                data.fl = fl;
                _type = Type::Double; 
            } else {
                throw unexpected_type_exception("*=", "(INT | BOOL)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (_type == Type::Double) {
            if (other._type < Type::Double) {
                data.fl *= other.data.num;
            } else if (other._type == Type::Double) {
                data.fl *= other.data.fl;
            } else {
                throw unexpected_type_exception("*=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("*=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator/=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("/=", "String", "Int | Bool", "[INT | BOOL, DOUBLE]]", "String", 0);
        }
        if (_type < Type::Double) {
            data.num /= other;
            _type = Type::Int;
        } else {
            data.fl /= other;
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator/=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("/=", "String", "Double", "[INT | BOOL, DOUBLE]]", "String", 0);
        }
        if (_type < Type::Double) {
            double d = other / data.num;
            data.fl = d;
            _type = Type::Double;
        } else {
            data.fl /= other;
        }
        return *this;
    }
    let& operator/=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num /= other.data.num;
                _type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl / data.num;
                data.fl = fl;
                _type = Type::Double; 
            } else {
                throw unexpected_type_exception("/=", "(INT | BOOL)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (_type == Type::Double) {
            if (other._type < Type::Double) {
                data.fl /= other.data.num;
            } else if (other._type == Type::Double) {
                data.fl /= other.data.fl;
            } else {
                throw unexpected_type_exception("/=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("/=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator%=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("%=", "String", "Int | Bool", "[INT | BOOL, DOUBLE]]", "String", 0);
        }
        if (_type < Type::Double) {
            data.num %= other;
            _type = Type::Int;
        } else {
            data.fl = std::fmod(data.fl, other);
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let&>::type
    operator%=(const T& other) {
        if (_type >= Type::String) {
            throw unexpected_type_exception("%=", "String", "Int | Bool", "[INT | BOOL, DOUBLE]]", "String", 0);
        }
        if (_type < Type::Double) {
            data.fl = std::fmod(data.num, other);
            _type = Type::Double;
        } else {
            data.fl = std::fmod(data.fl, other);
        }
        return *this;
    }
    let& operator%=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num %= other.data.num;
                _type = Type::Int;
            } else if (other._type == Type::Double) {
                data.fl = std::fmod(data.num, other.data.fl);
                _type = Type::Double; 
            } else {
                throw unexpected_type_exception("%=", "(INT | BOOL)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (_type == Type::Double) {
            if (other._type < Type::Double) {
                data.fl = fmod(data.fl, other.data.num);
            } else if (other._type == Type::Double) {
                data.fl = fmod(data.fl, other.data.fl);
            } else {
                throw unexpected_type_exception("%=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("%=", "(DOUBLE)", "(let)", "(INT | BOOL | DOUBLE)", other._type.str(), 0);
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator>>=(const T& other) {
        if (_type >= Type::Double) {
            throw unexpected_type_exception(">>=", _type.str(), "Int | Bool", "Int | Bool", _type.str(), 0);
        }
        data.num >>= other;
        _type = Type::Int;
        return *this;
    }
    let& operator>>=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num >>= other.data.num;   
            } else {
                throw unexpected_type_exception(">>=", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception(">>=", _type.str(), other._type.str(), "Int | Bool", _type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator<<=(const T& other) {
        if (_type >= Type::Double) {
            throw unexpected_type_exception("<<=", _type.str(), "Int | Bool", "Int | Bool", _type.str(), 0);
        }
        data.num <<= other;
        _type = Type::Int;
        return *this;
    }
    let& operator<<=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num <<= other.data.num;   
            } else {
                throw unexpected_type_exception("<<=", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("<<=", _type.str(), other._type.str(), "Int | Bool", _type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator&=(const T& other) {
        if (_type >= Type::Double) {
            throw unexpected_type_exception("&=", _type.str(), "Int | Bool", "Int | Bool", _type.str(), 0);
        }
        data.num &= other;
        _type = Type::Int;
        return *this;
    }

    let& operator&=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num &= other.data.num;   
            } else {
                throw unexpected_type_exception("&=", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("&=", _type.str(), other._type.str(), "Int | Bool", _type.str(), 0);
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator|=(const T& other) {
        if (_type >= Type::Double) {
            throw unexpected_type_exception("|=", _type.str(), "Int | Bool", "Int | Bool", _type.str(), 0);
        }
        data.num |= other;
        _type = Type::Int;
        return *this;
    }
    let& operator|=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num |= other.data.num;   
            } else {
                throw unexpected_type_exception("|=", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("|=", _type.str(), other._type.str(), "Int | Bool", _type.str(), 0);
        }
        return *this;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let&>::type
    operator^=(const T& other) {
        if (_type >= Type::Double) {
            throw unexpected_type_exception("^=", _type.str(), "Int | Bool", "Int | Bool", _type.str(), 0);
        }
        data.num ^= other;
        _type = Type::Int;
        return *this;
    }
    let& operator^=(const let& other) {
        if (this == &other) return *this;
        if (_type < Type::Double) {
            if (other._type < Type::Double) {
                data.num ^= other.data.num;   
            } else {
                throw unexpected_type_exception("^=", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("^=", _type.str(), other._type.str(), "Int | Bool", _type.str(), 0);
        }
        return *this;
    }
        // Compound assignment operators
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator+(let d, const T& other) {
        if (d._type == Type::String) {
            d.data.str += other;
        } else if (d._type < Type::Double) {
            d.data.num += other;
            d._type = Type::Int;
        } else {
            d.data.fl += other;
        }
        return d;
    }
    
    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let>::type
    friend operator+(let d, const T& other) {
        if (d._type == Type::String) {
            d.data.str += other;
        } else if (d._type < Type::Double) {
            d.data.fl += other;
            d._type = Type::Double;
        } else {
            d.data.fl += other;
        }
        return d;
    }
    template<typename T>
    typename std::enable_if<is_string<T>::value, let>::type
    friend operator+(let d, const T& other) {
        if (d._type == Type::String) {
            d.data.str += other;
        } else {
            d.str_constructor(d.toString());
            d.data.str += other;
            d._type = Type::String;
        }
        return d;
    }
    friend let operator+(let d, const let& other) {
        if (other._type == Type::String) {
            d.makeString(d.toString() + other.data.str);
        } else if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num += other.data.num;
                d._type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl + d.data.num;
                d.data.fl = fl;
                d._type = Type::Double; 
            } else {
                throw unexpected_type_exception("+", "Int | Bool", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (d._type == Type::Double) {
            if (other._type < Type::Double) {
                d.data.fl += other.data.num;
            } else if (other._type == Type::Double) {
                d.data.fl += other.data.fl;
            } else {
                throw unexpected_type_exception("+", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else if (d._type == Type::String) {
            d.data.str += other.toString();
        } else {
            throw unexpected_type_exception("+", "Null", other._type.str(), "(INT | BOOL | DOUBLE)", "Null", 0);
        }
        return d;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator-(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.num -= other;
            d._type = Type::Int;
        } else {
            d.data.fl -= other;
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let>::type
    friend operator-(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.fl = other - d.data.num;
            d._type = Type::Double;
        } else {
            d.data.fl -= other;
        }
        return d;
    }

    friend let operator-(let d, const let& other) {
        if (d._type >= Type::String)
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        else if (other._type >= Type::String)
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 1);
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num -= other.data.num;
                d._type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl - d.data.num;
                d.data.fl = fl;
                d._type = Type::Double; 
            } else {
                throw unexpected_type_exception("-", "Int | Bool", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (d._type == Type::Double) {
            if (other._type < Type::Double) {
                d.data.fl -= other.data.num;
            } else if (other._type == Type::Double) {
                d.data.fl -= other.data.fl;
            } else {
                throw unexpected_type_exception("-", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator*(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("*", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.num *= other;
            d._type = Type::Int;
        } else {
            d.data.fl *= other;
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let>::type
    friend operator*(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("*", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.fl = other * d.data.num;
            d._type = Type::Double;
        } else {
            d.data.fl *= other;
        }
        return d;
    }

    friend let operator*(let d, const let& other) {
        if (d._type >= Type::String)
            throw unexpected_type_exception("*", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        else if (other._type >= Type::String)
            throw unexpected_type_exception("*", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 1);
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num *= other.data.num;
                d._type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl * d.data.num;
                d.data.fl = fl;
                d._type = Type::Double; 
            } else {
                throw unexpected_type_exception("*", "Int | Bool", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (d._type == Type::Double) {
            if (other._type < Type::Double) {
                d.data.fl *= other.data.num;
            } else if (other._type == Type::Double) {
                d.data.fl *= other.data.fl;
            } else {
                throw unexpected_type_exception("-", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("-", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator/(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("/", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.num /= other;
            d._type = Type::Int;
        } else {
            d.data.fl /= other;
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let>::type
    friend operator/(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("/", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.fl = other / d.data.num;
            d._type = Type::Double;
        } else {
            d.data.fl /= other;
        }
        return d;
    }

    friend let operator/(let d, const let& other) {
        if (d._type >= Type::String)
            throw unexpected_type_exception("/", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        else if (other._type >= Type::String)
            throw unexpected_type_exception("/", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 1);
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num /= other.data.num;
                d._type = Type::Int;
            } else if (other._type == Type::Double) {
                auto fl = other.data.fl / d.data.num;
                d.data.fl = fl;
                d._type = Type::Double; 
            } else {
                throw unexpected_type_exception("/", "Int | Bool", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (d._type == Type::Double) {
            if (other._type < Type::Double) {
                d.data.fl /= other.data.num;
            } else if (other._type == Type::Double) {
                d.data.fl /= other.data.fl;
            } else {
                throw unexpected_type_exception("/", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("/", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator%(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("%", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.num %= other;
            d._type = Type::Int;
        } else {
            d.data.fl = std::fmod(d.data.fl, other);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, let>::type
    friend operator%(let d, const T& other) {
        if (d._type >= Type::String) {
            throw unexpected_type_exception("%", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        if (d._type < Type::Double) {
            d.data.fl = std::fmod(d.data.num, other);
            d._type = Type::Double;
        } else {
            d.data.fl = std::fmod(d.data.fl, other);
        }
        return d;
    }

    friend let operator%(let d, const let& other) {
        if (d._type >= Type::String)
            throw unexpected_type_exception("%", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        else if (other._type >= Type::String)
            throw unexpected_type_exception("%", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 1);
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num /= other.data.num;
                d._type = Type::Int;
            } else if (other._type == Type::Double) {
                d.data.fl = std::fmod(d.data.num, other.data.fl);
                d._type = Type::Double; 
            } else {
                throw unexpected_type_exception("%", "Int | Bool", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }

        } else if (d._type == Type::Double) {
            if (other._type < Type::Double) {
                d.data.fl = std::fmod(d.data.fl, other.data.num);
            } else if (other._type == Type::Double) {
                d.data.fl = std::fmod(d.data.fl, other.data.fl);
            } else {
                throw unexpected_type_exception("%", "Double", other._type.str(), "(INT | BOOL | DOUBLE)", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("%", d._type.str(), other._type.str(), "(INT | BOOL | DOUBLE)", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator>>(let d, const T& other) {
        if (d._type >= Type::Double) {
            throw unexpected_type_exception(">>", d._type.str(), "Int | Bool", "Int | Bool", d._type.str(), 0);
        }
        d.data.num >>= other;
        d._type = Type::Int;
        return d;
    }
    friend let operator>>(let d, const let& other) {
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num >>= other.data.num;   
            } else {
                throw unexpected_type_exception(">>", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception(">>", d._type.str(), other._type.str(), "Int | Bool", d._type.str(), 0);
        }
        return d;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator<<(let d, const T& other) {
        if (d._type >= Type::Double) {
            throw unexpected_type_exception("<<", d._type.str(), "Int | Bool", "Int | Bool", d._type.str(), 0);
        }
        d.data.num <<= other;
        d._type = Type::Int;
        return d;
    }
    friend let operator<<(let d, const let& other) {
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num <<= other.data.num;   
            } else {
                throw unexpected_type_exception("<<", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("<<", d._type.str(), other._type.str(), "Int | Bool", d._type.str(), 0);
        }
        return d;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator&(let d, const T& other) {
        if (d._type >= Type::Double) {
            throw unexpected_type_exception("&", d._type.str(), "Int | Bool", "Int | Bool", d._type.str(), 0);
        }
        d.data.num &= other;
        d._type = Type::Int;
        return d;
    }
    friend let operator&(let d, const let& other) {
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num &= other.data.num;   
            } else {
                throw unexpected_type_exception("&", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("&", d._type.str(), other._type.str(), "Int | Bool", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator|(let d, const T& other) {
        if (d._type >= Type::Double) {
            throw unexpected_type_exception("|", d._type.str(), "Int | Bool", "Int | Bool", d._type.str(), 0);
        }
        d.data.num |= other;
        d._type = Type::Int;
        return d;
    }
    friend let operator|(let d, const let& other) {
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num |= other.data.num;   
            } else {
                throw unexpected_type_exception("|", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("|", d._type.str(), other._type.str(), "Int | Bool", d._type.str(), 0);
        }
        return d;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, let>::type
    friend operator^(let d, const T& other) {
        if (d._type >= Type::Double) {
            throw unexpected_type_exception("^", d._type.str(), "Int | Bool", "Int | Bool", d._type.str(), 0);
        }
        d.data.num ^= other;
        d._type = Type::Int;
        return d;
    }
    friend let operator^(let d, const let& other) {
        if (d._type < Type::Double) {
            if (other._type < Type::Double) {
                d.data.num ^= other.data.num;   
            } else {
                throw unexpected_type_exception("^", "Int | Bool", other._type.str(), "Int | Bool", other._type.str(), 1);
            }
        } else {
            throw unexpected_type_exception("^", d._type.str(), other._type.str(), "Int | Bool", d._type.str(), 0);
        }
        return d;
    }
    let& operator++() {
        if (_type < Type::Double) {
            data.num++;
        } else if (_type == Type::Double) {
            data.fl++;
        } else {
            throw std::runtime_error("let::operator++ throws an error: your type is not numeric");
        }
        return *this;
    }
    friend let operator++(let& dt, int) {
        let prev = dt;
        ++dt;
        return prev;
    }


    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator==(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num == num;
            } else {
                return data.num == (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num == num;
            } else {
                return data.num == (long long) num;
            }
        } else return false;
    }
    bool operator==(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num == num.data.num;
            } else {
                return data.fl == num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num == num.data.fl;
            } else {
                return data.fl == num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator==(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data == num.data.num;
        } else if (num._type == Type::Double) {
            return data == num.data.fl;
        } else return false;
    }
    // std::enable_if
    // operator!= as operator<;
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator!=(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num != num;
            } else {
                return data.num != (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num != num;
            } else {
                return data.num != (long long) num;
            }
        } else return false;
    }
    bool operator!=(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num != num.data.num;
            } else {
                return data.fl != num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num != num.data.fl;
            } else {
                return data.fl != num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator!=(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data != num.data.num;
        } else if (num._type == Type::Double) {
            return data != num.data.fl;
        } else return false;
    }
    bool operator!() {
        if (_type < Type::String)
            return !data.num;
        else if (_type == Type::String)
            return data.str == "";
        else return true; // Null always true
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator>(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num > num;
            } else {
                return data.num > (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num > num;
            } else {
                return data.num > (long long) num;
            }
        } else return false;
    }
    bool operator>(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num > num.data.num;
            } else {
                return data.fl > num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num > num.data.fl;
            } else {
                return data.fl > num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator>(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data > num.data.num;
        } else if (num._type == Type::Double) {
            return data > num.data.fl;
        } else return false;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator<(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num < num;
            } else {
                return data.num < (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num < num;
            } else {
                return data.num < (long long) num;
            }
        } else return false;
    }
    bool operator<(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num < num.data.num;
            } else {
                return data.fl < num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num < num.data.fl;
            } else {
                return data.fl < num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator<(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data < num.data.num;
        } else if (num._type == Type::Double) {
            return data < num.data.fl;
        } else return false;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator>=(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num >= num;
            } else {
                return data.num >= (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num >= num;
            } else {
                return data.num >= (long long) num;
            }
        } else return false;
    }
    bool operator>=(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num >= num.data.num;
            } else {
                return data.fl >= num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num >= num.data.fl;
            } else {
                return data.fl >= num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator>=(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data >= num.data.num;
        } else if (num._type == Type::Double) {
            return data >= num.data.fl;
        } else return false;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    operator<=(const T& num) {
        if (_type < Type::Double) {
            // int | bool
            if (std::is_integral<T>::value) {
                return data.num <= num;
            } else {
                return data.num <= (long long) num;
            }
        } else if (_type == Type::Double) {
            if (std::is_integral<T>::value) {
                return data.num <= num;
            } else {
                return data.num <= (long long) num;
            }
        } else return false;
    }
    bool operator<=(const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            if (_type < Type::Double) {
                return data.num <= num.data.num;
            } else {
                return data.fl <= num.data.num;
            }
        } else if (num._type == Type::Double) {
            if (_type < Type::Double) {
                return data.num <= num.data.fl;
            } else {
                return data.fl <= num.data.num;
            }
        } else return false;
    }
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, char>::value, bool>::type
    friend operator<=(const T& data, const let& num) {
        if (num._type < Type::Double) {
            // int | bool
            return data <= num.data.num;
        } else if (num._type == Type::Double) {
            return data <= num.data.fl;
        } else return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const let& obj) {
        #if defined(DEBUG) || defined(CPUF_LET_DEBUG)
        os << "(_type:" << obj._type.str() << ") ";
        #endif
        if (obj._type == Type::String) {
            os << obj.data.str;
        } else if (obj._type == Type::Int) {
            os << obj.data.num;
        } else if (obj._type == Type::Double) {
            os << obj.data.fl;
        } else if (obj._type == Type::Bool) {
            if (obj.data.num)
                os << "true";
            else
                os << "false";
        } else {
            os << "Null";
        }
        return os;
    }
};

using std::getline;
Type::Type(const let& data) : _t(data.type()) {}
#undef PRIVATE
#undef END