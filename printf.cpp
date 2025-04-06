static const char* _ioconfig_laststart = nullptr;
//std::vector<const char*> errbuf = {};
namespace ioconfig {
    // standard configurations
    const char
        *_char = "c|hhd",
        *_uchar = "hhu",
        *_short = "hd",
        *_ushort = "hu",
        *_int = "d|i",
        *_uint = "u",
        *_long = "ld|li",
        *_ulong = "lu",
        *_llong = "lld|lli",
        *_ullong = "llu|zu"
    ;
    const char 
        *_char_hex = "hhx",
        *_char_hex_big = "hhX",
        *_short_hex = "hx",
        *_short_hex_big = "hX",
        *_int_hex = "x",
        *_int_hex_big = "X",
        *_long_hex = "lx",
        *_long_hex_big = "lX",
        *_llong_hexx = "hhx",
        *_llong_hex_big = "hhX"
    ;
    const char* str = "s";
    int eval_simple(const char* conf, const char* input) {
        if (conf[1] == '\0') {
            // handle single character
            return (conf[0] == input[0]);
        }
        int equal = 1;
        const char* p = conf;
        _ioconfig_laststart = p;
        for (; *p != '\0'; p++, input++) {
            if (*p == '|') {
                if (equal)
                    return p - _ioconfig_laststart;
                // reset
                equal = 1;
                _ioconfig_laststart = p + 1;
                continue;
            }
            if (*p != *input)
                equal = 0;
            
        }
        if (equal)
            return p - _ioconfig_laststart;
        return 0;
    }
    // std::tuple<int, std::smatch> eval_complex(std::string conf, const char* input) {
    //     try {
    //         static std::regex pattern(conf);  // Static to reuse the compiled pattern
    //         std::smatch result;
    //         if (std::regex_search(input, result, pattern))
    //             return std::make_tuple(1, result);
    //     } catch (const std::regex_error& e) {
    //         // Handle or log the error
    //         std::cerr << "<Regex error: " << e.what() << '>';
    //     }
    //     return std::make_tuple(0, std::smatch());
    // }
};