#ifndef CPUF_COLOR
#define CPUF_COLOR
#ifndef __cplusplus
#error "cpuf/color is C++ header"
#endif

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include <memory>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>
/**
 * @brief ASCII color constants for console. Also supports RBG and HEX colors.
 * By default there are in scope foreiground constants, means they will change text color.
 * the color::bg namespace defines constants to change the background.
 * The 'rgb' function let you get an RGB color and hex converts the color to RGB and returns RBG().
 * inline int rgb(const unsigned char R, const unsigned char G, const unsigned char B);
 * inline int hex(const char* c);
 *  invalid_argument_exception: non hexadicimal character (begin with '#' considered hexadecimal character)
 *  out_of_range: the color either has 0 or over 6 digits
 * @file color
*/
namespace color {
    struct RGB { 
        unsigned char R, G, B;
        bool operator==(const RGB& _rgb) {
            return R == _rgb.R && G == _rgb.G && B == _rgb.B;
        }
        bool operator!=(const RGB& _rgb) {
            return R != _rgb.R || G != _rgb.G || B != _rgb.B;
        }
        std::string str() {
            std::string s = "(";
                        s += R;
                        s += ';';
                        s += G;
                        s += ';';
                        s += B;
                        s += ')';
            return s;
        }
        static RGB from_hex(const char* hex) {
            if (hex[0] == '#') hex++; // Skip the '#' character
            std::istringstream iss(hex);
            int hexValue;
            iss >> std::hex >> hexValue;

            RGB _rgb;
            _rgb.R = (hexValue >> 16) & 0xFF;
            _rgb.G = (hexValue >> 8) & 0xFF;
            _rgb.B = hexValue & 0xFF;

            return _rgb;
        }
        static inline RGB from_hex(const std::string hex) {
            return from_hex(hex.c_str());
        }
        std::string to_hex() {
            return to_hex(R, G, B);
        }
        static std::string to_hex(const RGB _rgb) {
            std::stringstream ss;
            ss << "#" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << _rgb.R
            << std::setw(2) << _rgb.G
            << std::setw(2) << _rgb.B;
            return ss.str();
        }
        static inline std::string to_hex(unsigned char R, unsigned char G, unsigned char B) {
            return to_hex(RGB {R, G, B});
        }
        operator std::string() {
            return to_hex();
        }
    };
    /** @brief Resets both foreiground and background to it's default state */
    constexpr const char* resetAll = "\033[0m";
    static inline std::string __rgb(const RGB& _rgb, const int& _ground) {
        // max value: "\033[38;2;255;255;255m"
        // max len: 20
        char p[20];
        std::sprintf(p, "\033[%d;2;%hhu;%hhu;%hhum", _ground, _rgb.R, _rgb.G, _rgb.B);
        std::string s = p;
        return s;
    }
    inline std::string rgb(const RGB& _rgb) {
        return __rgb(_rgb, 38);
    }
    inline std::string rgb(const unsigned char R, const unsigned char G, const unsigned char B) {
        return __rgb(RGB {R, G, B}, 38);
    }
    inline std::string hex(const std::string hex) {
        return __rgb (RGB::from_hex(hex), 38);
    }
    inline std::string hex(const char* hex) {
        return __rgb (RGB::from_hex(hex), 38);
    }
    namespace bg {
        inline std::string rgb(const RGB& _rgb) {
            return __rgb(_rgb, 48);
        }
        inline std::string rgb(const unsigned char R, const unsigned char G, const unsigned char B) {
            return __rgb(RGB {R, G, B}, 48);
        }
        inline std::string hex(const std::string hex) {
            return __rgb (RGB::from_hex(hex), 48);
        }
        inline std::string hex(const char* hex) {
            return __rgb (RGB::from_hex(hex), 48);
        }
    }
}
#define ADD_COLOR(name, value)                                  \
    namespace color {                                           \
        constexpr const char* name = "\033[3" #value "m";       \
        namespace bg {                                          \
            constexpr const char* name = "\033[4" #value "m";   \
        }                                                       \
    }
#define ADD_COLOR_CST(name, value)                              \
    namespace color {                                           \
        constexpr const char* name = "\033[3" value "m";        \
        namespace bg {                                          \
            constexpr const char* name = "\033[4" value "m";    \
        }                                                       \
    }
#define ADD_COLOR_BRIGHT(name, value)                           \
    namespace color {                                           \
        constexpr const char* name = "\033[9" #value "m";       \
        namespace bg {                                          \
            constexpr const char* name = "\033[10" #value "m";  \
        }                                                       \
    }
ADD_COLOR(reset, 9);
ADD_COLOR(black, 0);
ADD_COLOR(red, 1);
ADD_COLOR(green, 2);
ADD_COLOR(yellow, 3);
ADD_COLOR(blue, 4);
ADD_COLOR(purple, 5);
ADD_COLOR(teal, 6);
ADD_COLOR(white, 7);
ADD_COLOR(grey, 8);
ADD_COLOR_CST(orange, "8;5;208");      // 38;5;208
ADD_COLOR_CST(pink, "8;5;206");        // 38;5;206
ADD_COLOR_CST(brown, "8;5;130");       // 38;5;103
ADD_COLOR_BRIGHT(bright_black, 0);
ADD_COLOR_BRIGHT(bright_red, 1);
ADD_COLOR_BRIGHT(bright_green, 2);
ADD_COLOR_BRIGHT(bright_yellow, 3);
ADD_COLOR_BRIGHT(bright_blue, 4);
ADD_COLOR_BRIGHT(bright_magenta, 5);
ADD_COLOR_BRIGHT(bright_cyan, 6);
ADD_COLOR_BRIGHT(bright_white, 7);
#undef ADD_COLOR
#undef ADD_COLOR_CST
#undef ADD_COLOR_BRIGHT
#undef _CRT_SECURE_NO_WARNINGS
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // CPUF_COLOR