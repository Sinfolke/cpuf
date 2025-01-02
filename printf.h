#pragma once

#include <string>
#include <sstream>
#include <cstdarg>
#include <charconv>
#include <iostream>
#include <iomanip>
#include <functional>
#include <vector>
#include <ostream>  // For std::ostream

namespace ioconfig {
    // standard configurations
    extern const char
        *_char,
        *_uchar,
        *_short,
        *_ushort,
        *_int,
        *_uint,
        *_long,
        *_ulong,
        *_llong,
        *_ullong
    ;
    extern const char 
        *_char_hex,
        *_char_hex_big,
        *_short_hex,
        *_short_hex_big,
        *_int_hex,
        *_int_hex_big,
        *_long_hex,
        *_long_hex_big,
        *_llong_hexx,
        *_llong_hex_big
    ;
    extern const char *str;
    int eval_simple(const char *conf, const char *input);
};

// Base case for the case when only one argument is left
static void print_nth(std::ostream &out, int index) {
    out << std::string("<none>");
}

template<typename T, typename ...Args>
static void print_nth(std::ostream &out, int index, T first, Args&&... args) {
    if (index == 0) {
        out << first;
    } else {
        print_nth(out, index - 1, args...);
    }
}

// General case for when there are more than one argument
template<typename Actual>
static void print_ntht(std::ostream& out, int index) {
    out << std::string("<none>");
}
// Primary template for variadic arguments
template<typename Actual, typename T, typename... Args>
static void print_ntht(std::ostream& out, int index, T first, Args... rest) {
    if (index == 0) {
        out << reinterpret_cast<Actual&>(first);
    } else {
        print_ntht<Actual>(out, index - 1, rest...);
    }
}

static long long convert_to_num(const char* start, char* end) {
    long long id = std::strtoll(start, &end, 10);
    #ifdef CPUF_NON_ZERO_COUNT
        id++;
    #endif
    return id;
}

template<typename... Args>
static int out_list(std::ostream& out, int id, const char* start, int iterator, int* printed, Args... args) {
    char* end;
    *printed = convert_to_num(start, end);

    for (int i = 0; i < *printed; i++) {
        print_nth(out, id + (i * iterator), args...);
    }

    return end - start;
}

template<typename... Args>
static int out_woffset(std::ostream& out, int id, const char* start, int sign, int* printed, Args... args) {
    char* end;
    *printed = convert_to_num(start, end);

    print_nth(out, id + (*printed * sign), args...);

    return end - start;
}

// Macros to simplify the `cpuf_internal_print_fmt` implementation
#define TRY(var) if ((len = ioconfig::eval_simple(ioconfig::var, format)) != 0)
#define TRY_EVAL(var) else TRY(var)
#define printt(type) print_ntht<type>(out, id, args...); (*printed)++
#define print print_nth(out, id, args...); (*printed)++
#define PRINT_HEX(type) \
    out << std::setw(sizeof(type)) << std::setfill('0') << std::hex; \
    printt(type); \
    (*printed)++
#define PRINT_HEX_UPPER(type) \
    out << std::setw(sizeof(type)) << std::setfill('0') << std::hex << std::uppercase; \
    printt(type); \
    out << std::nouppercase; \
    (*printed)++

/**
 * Internal function to handle format specifiers and print accordingly.
 * This version writes to a provided `std::ostream`.
 */
template<typename... Args>
static int cpuf_internal_do_fmt(int id, const char* format, int* printed, std::ostream& out, Args&&... args) {
    int len = 0;
    // TRY blocks to evaluate format specifiers
    TRY(_char) {
        printt(char);
    }
    TRY_EVAL(_uchar) {
        printt(unsigned char);
    }
    TRY_EVAL(_short) {
        printt(short);
    }
    TRY_EVAL(_ushort) {
        printt(unsigned short);
    }
    TRY_EVAL(_int) {
        printt(int);
    }
    TRY_EVAL(_uint) {
        printt(unsigned int);
    }
    TRY_EVAL(_long) {
        printt(long);
    }
    TRY_EVAL(_ulong) {
        printt(unsigned long);
    }
    TRY_EVAL(_llong) {
        printt(long long);
    }
    TRY_EVAL(_ullong) {
        printt(unsigned long long);
    }

    // For hexadecimal types
    TRY_EVAL(_char_hex) {
        PRINT_HEX(char);
    }
    TRY_EVAL(_char_hex_big) {
        PRINT_HEX_UPPER(char);
    }
    TRY_EVAL(_short_hex) {
        PRINT_HEX(short);
    }
    TRY_EVAL(_short_hex_big) {
        PRINT_HEX_UPPER(short);
    }
    TRY_EVAL(_int_hex) {
        PRINT_HEX(int);
    }
    TRY_EVAL(_int_hex_big) {
        PRINT_HEX_UPPER(int);
    }
    TRY_EVAL(_long_hex) {
        PRINT_HEX(long);
    }
    TRY_EVAL(_long_hex_big) {
        PRINT_HEX_UPPER(long);
    }
    TRY_EVAL(_llong_hexx) {
        PRINT_HEX(long long);
    }
    TRY_EVAL(_llong_hex_big) {
        PRINT_HEX_UPPER(long long);
    }
    TRY_EVAL(str) {
        // string
        print;
    }
    else if (isdigit(*format)) {
        char* end;
        long long num = convert_to_num(format, end);
        print;
        len = static_cast<int>(end - format);
    }
    else if (*format == '$') {
        // Output any type
        len = 1;
        *printed = 1;
        print;
    }
    else if (*format == '+') {
        len = (*++format == '+') ?
            out_list(out, id, format, 1, printed, args...) : out_woffset(out, id, format, 1, printed, args...);
    }
    else if (*format == '-') {
        len = (*++format == '-') ?
            out_list(out, id, format, -1, printed, args...) : out_woffset(out, id, format, -1, printed, args...);
    }
    else {
        return 0; // Unrecognized format specifier
    }
    return len;
}

#undef TRY
#undef TRY_EVAL
#undef PRINT_HEX
#undef PRINT_HEX_UPPER
#undef printt
#undef print

/**
 * Internal function to handle format specifiers and print accordingly.
 * This version writes to `std::cout`.
 */
template<typename ...Args>
static int cpuf_internal_print_fmt(int id, const char* format, int* printed, Args&&... args) {
    // Reuse the existing `cpuf_internal_print_fmt` by passing `std::cout` as the stream
    // To maintain functionality, we call the `std::ostream` version and then output it
    std::ostringstream oss;
    int len = cpuf_internal_do_fmt(id, format, printed, oss, args...);
    std::cout << oss.str();
    return len;
}

template<typename ...Args>
static int cpuf_internal_print_fmt_stderr(int id, const char* format, int* printed, Args&&... args) {
    // Reuse the existing `cpuf_internal_print_fmt` by passing `std::cout` as the stream
    // To maintain functionality, we call the `std::ostream` version and then output it
    std::ostringstream oss;
    int len = cpuf_internal_do_fmt(id, format, printed, oss, args...);
    std::cerr << oss.str();
    return len;
}

namespace cpuf {
/**
 * `cpuf_printf` writes the formatted output directly to `std::cout`.
 */
template<typename ...Args>
int printf(const char* format, Args&&... args) {
    int id = 0;
    const char* start = format;
    while (*format != '\0') {
        if (*format == '%') {
            format++;
            if (*format != '%') {
                int printed = 0;
                int len = cpuf_internal_print_fmt(id, format, &printed, args...);
                format += len;
                id += printed;
                continue;
            }
        }
        std::cout << *format++;
    }
    return static_cast<int>(format - start);
}

/**
 * Overloaded `cpuf_printf` that accepts a `std::string` as the format.
 */
template<typename ...Args>
int printf(const std::string& format, Args&&... args) {
    int id = 0;
    int i = 0;
    while (i < static_cast<int>(format.size())) {
        if (format[i] == '%') {
            i++;
            if (i < static_cast<int>(format.size()) && format[i] != '%') {
                int printed = 0;
                int len = cpuf_internal_print_fmt(id, format.c_str() + i, &printed, args...);
                i += len;
                id += printed;
                continue;
            }
        }
        std::cout << format[i++];
    }
    return i;
}

template<typename ...Args>
int perror(const std::string& format, Args&&... args) {
    int id = 0;
    int i = 0;
    while (i < static_cast<int>(format.size())) {
        if (format[i] == '%') {
            i++;
            if (i < static_cast<int>(format.size()) && format[i] != '%') {
                int printed = 0;
                int len = cpuf_internal_print_fmt_stderr(id, format.c_str() + i, &printed, args...);
                i += len;
                id += printed;
                continue;
            }
        }
        std::cout << format[i++];
    }
    return i;
}
/**
 * `cpuf_sprintf` formats the string and returns it as `std::string`.
 */
template<typename ...Args>
std::string sprintf(const char* format, Args&&... args) {
    int id = 0;
    const char* start = format;
    std::stringstream buf;
    while (*format != '\0') {
        if (*format == '%') {
            format++;
            if (*format != '%') {
                int printed = 0;
                int len = cpuf_internal_do_fmt(id, format, &printed, buf, args...);
                format += len;
                id += printed;
                continue;
            }
        }
        buf << *format++;
    }
    return buf.str();
}

/**
 * Overloaded `cpuf_sprintf` that accepts a `std::string` as the format.
 */
template<typename ...Args>
std::string sprintf(const std::string& format, Args&&... args) {
    int id = 0;
    int i = 0;
    std::stringstream buf;
    while (i < static_cast<int>(format.size())) {
        if (format[i] == '%') {
            i++;
            if (i < static_cast<int>(format.size()) && format[i] != '%') {
                int printed = 0;
                int len = cpuf_internal_do_fmt(id, format.c_str() + i, &printed, buf, args...);
                i += len;
                id += printed;
                continue;
            }
        }
        buf << format[i++];
    }
    return buf.str();
}


} // cpuf