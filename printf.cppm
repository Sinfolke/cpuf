export module cpuf.printf;
import std;

// Helper trait
template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

// Shortcut
template <typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;

// Concepts
template <typename T>
concept IsPrintable = is_streamable_v<T>;

// Range concept
template <typename T>
concept Range = requires(T t) {
    t.begin(); t.end();
};

// Stream non-printable ranges
export template <typename T>
auto operator<<(std::ostream& os, const T& range)
    -> std::enable_if_t<
           Range<T> &&
           !is_streamable_v<T>,
           std::ostream&
       >
{
    // Implementation for printing range of non-streamable elements
    os << "[";
    for (auto it = range.begin(); it != range.end(); ++it) {
        if (it != range.begin())
            os << ", ";
        os << *it; // This will only compile if *it is streamable.
    }
    os << "]";
    return os;
}


// Stream pairs
export template<typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p) {
    os << '(' << p.first << ", " << p.second << ')';
    return os;
}
// Operator<< for tuples
export template <typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& tup) {
    os << '(';
    print_tuple_impl(os, tup, std::index_sequence_for<Ts...>{});
    os << ')';
    return os;
}

namespace cpuf {

    // Base case: no arguments left
    inline void print_nth(std::ostream& out, int index) {
        out << "<none>";
    }

    // Recursive variadic print_nth, perfect forwarding args
    template <typename T, typename... Args>
    void print_nth(std::ostream& out, int index, T&& first, Args&&... args) {
        if (index == 0) {
            out << std::forward<T>(first);
        } else {
            print_nth(out, index - 1, std::forward<Args>(args)...);
        }
    }

    // Internal helper: processes the format string and outputs to the given ostream
    template <typename... Args>
    int vprintf_impl(std::ostream& out, const char* format, Args&&... args) {
        int id = 0;
        const char* start = format;
        while (*format) {
            if (*format == '{' && *(format + 1) == '}') {
                print_nth(out, id++, std::forward<Args>(args)...);
                format += 2;
            } else {
                out << *format++;
            }
        }
        return static_cast<int>(format - start);
    }

    // Overload for std::string format: just forward to the const char* version
    template <typename... Args>
    int vprintf_impl(std::ostream& out, const std::string& format, Args&&... args) {
        return vprintf_impl(out, format.c_str(), std::forward<Args>(args)...);
    }
    export template <typename... Args>
    int printf(const char* format, Args&&... args) {
        auto r = vprintf_impl(std::cout, format, std::forward<Args>(args)...);
#ifndef DEBUG
        std::cout << std::endl;
#else
        std::cout << '\n';
#endif
        return r;
    }

    export template <typename... Args>
    int printf(const std::string& format, Args&&... args) {
        auto r = vprintf_impl(std::cout, format, std::forward<Args>(args)...);
#ifndef DEBUG
        std::cout << std::endl;
#else
        std::cout << '\n';
#endif
        return r;
    }
    // Public printf writes to std::cout
    export template <typename... Args>
    int dprintf(const char* format, Args&&... args) {
        return vprintf_impl(std::cout, format, std::forward<Args>(args)...);
    }

    export template <typename... Args>
    int dprintf(const std::string& format, Args&&... args) {
        return vprintf_impl(std::cout, format, std::forward<Args>(args)...);
    }

    // Public perror writes to std::cerr
    export template <typename... Args>
    int perror(const char* format, Args&&... args) {
        return vprintf_impl(std::cerr, format, std::forward<Args>(args)...);
    }

    export template <typename... Args>
    int perror(const std::string& format, Args&&... args) {
        return vprintf_impl(std::cerr, format, std::forward<Args>(args)...);
    }

    // sprintf returns std::string instead of int (makes sense)
    export template <typename... Args>
    std::string sprintf(const char* format, Args&&... args) {
        std::ostringstream os;
        vprintf_impl(os, format, std::forward<Args>(args)...);
        return os.str();
    }

    export template <typename... Args>
    std::string sprintf(const std::string& format, Args&&... args) {
        std::ostringstream os;
        vprintf_impl(os, format, std::forward<Args>(args)...);
        return os.str();
    }

    // convert_to_num unchanged
    inline long long convert_to_num(const char* start, char*& end) {
        long long id = std::strtoll(start, &end, 10);
    #ifdef CPUF_NON_ZERO_COUNT
        id++;
    #endif
        return id;
    }

} // namespace cpuf
