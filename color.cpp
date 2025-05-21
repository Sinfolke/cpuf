module cpuf.color;
import std;
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
/**
 * @brief ASCII color constants for console. Supports RBG and HEX colors.
 * As default there are in scope foreground constants, means they will change text color.
 * the color::bg namespace defines constants to change the background.
 * The 'rgb' function let you get an RGB color and hex converts the color to RGB and returns RBG().
 * inline int rgb(const unsigned char R, const unsigned char G, const unsigned char B);
 * inline int hex(const char* c);
 *  invalid_argument_exception: non hexadicimal character (begin with '#' considered hexadecimal character)
 *  out_of_range: the color either has 0 or over 6 digits
 * @file color
*/
namespace color {
    bool RGB::operator==(const RGB& _rgb) {
            return R == _rgb.R && G == _rgb.G && B == _rgb.B;
            }
    bool RGB::operator!=(const RGB& _rgb) {
            return R != _rgb.R || G != _rgb.G || B != _rgb.B;
    }
    std::string RGB::str() {
        std::string s = "(";
                    s += R;
                    s += ';';
                    s += G;
                    s += ';';
                    s += B;
                    s += ')';
        return s;
    }
    RGB RGB::from_hex(const char* hex) {
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
    RGB RGB::from_hex(const std::string hex) {
        return from_hex(hex.c_str());
    }
    std::string RGB::to_hex() {
        return to_hex(R, G, B);
    }
    std::string RGB::to_hex(const RGB _rgb) {
        std::stringstream ss;
        ss << "#" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(2) << _rgb.R
        << std::setw(2) << _rgb.G
        << std::setw(2) << _rgb.B;
        return ss.str();
    }
    std::string RGB::to_hex(unsigned char R, unsigned char G, unsigned char B) {
        return to_hex(RGB {R, G, B});
    }
    /** @brief Resets both foreiground and background to it's default state */
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
#undef _CRT_SECURE_NO_WARNINGS
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif