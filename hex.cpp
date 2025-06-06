module cpuf.hex;
import std;
import std.compat;
namespace hex {
    static const char* bintable[] = {
        "0000", "0001", "0010", // 1 2 3
        "0011", "0100", "0101", // 4 5 6
        "0110", "0111", "1000", // 7 8 9
        "1001", "1010", "1011", // A B C
        "1100", "1101", "1111"  // D E F
    };
    
long long to_decimal(std::string hex) {
    if (hex.size() >= 2 && hex[0] == '0' && hex[1] == 'x') {
        // Remove the "0x" prefix
        hex = hex.substr(2);
    }
    long long resultnum = 0;
    for (size_t i = 0, size = hex.size() - 1; i <= size; i++) {
        char& c = hex[i];
        // A-F may be represented with any capslock
        c = tolower(c);
        if (c >= '0' && c <= '9') {
            /*
                With ASCII encoding the numbers
                are represented from 48 up to 57.
                substd::string the 'c' and '0' would produce the needed number
            */
            resultnum += (c - '0') * (1LL << (4 * (size - i)));
        } else if (c >= 'a' && c <= 'f') {
            // similar as above
            resultnum += (10 + c - 'a') * (1LL << (4 * (size - i)));
        } else {
            throw std::invalid_argument("Unknown hex character '" + std::string(1, c) + "'");
        }
    }
    return resultnum;
}
std::string to_binary(std::string hex) {
    std::string bin;
    if (hex.size() >= 2 && hex[0] == '0' && hex[1] == 'x') {
        // Remove the "0x" prefix
        hex = hex.substr(2);
    }
    for (size_t i = 0, size = hex.size(); i < size; ++i) {
        char c = hex[i];
        c = tolower(c);
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'F')
            c -= 'A';
        else if (c >= 'a' && c <= 'f') {
            c -= 'a';
            c += 10;
        }
        else
            throw std::invalid_argument("The digit is not hex");
        bin += bintable[c];
    }
    return bin;
}
std::string to_binary(long long num) {
    std::string bin;
    do {
        const unsigned hexdig = num % 16;
        bin += bintable[hexdig];
        num /= 16;
    } while (num > 0);
    // Reverse the std::stringing to get the correct order
    std::reverse(bin.begin() + 2, bin.end());
    return bin;
}



std::string from_binary(const std::string& bin) {
        std::string hex = "0x";
        size_t size = bin.size();
        if (size % 4 != 0) {
            // incorrect bits order
            throw std::invalid_argument("Incorrect bits order");
        }
        for (size_t i = 0; i < size; i += 4) {
            char bit[4] = { '0', '0', '0', '0' };
            for (size_t j = 0; j < 4; j++) {
                if (i + j < size) {
                    bit[j] = bin[i + j];
                }
            }
            if (
                (bit[0] != '0' && bit[0] != '1') ||
                (bit[1] != '0' && bit[1] != '1') ||
                (bit[2] != '0' && bit[2] != '1') ||
                (bit[3] != '0' && bit[3] != '1')
            ) {
                throw std::invalid_argument("The string is not a sequence of bits: " + bin);
            }
            int decimal = 0;
            for (int j = 0; j < 4; j++) {
                decimal += 
                    (1 << (3 - j)) /* 2^( 3 - j ) */
                        * 
                    (bit[j] - '0');
            }
            hex += 
            (decimal <= 9) 
                        ? 
                (char)    (decimal + '0') 
                        :
                (char)    (decimal - 10 + 'A');
        }
        return hex;
}

std::string from_decimal(long long num) {
    std::string hex;
    do {
        const unsigned hexdig = num % 16;
        if (hexdig < 10)
            hex += '0' + hexdig;
        else
            hex += ('A' - 10) + hexdig;
        num /= 16;
    } while (num > 0);
    // Reverse the std::string to get the correct order
    std::reverse(hex.begin() + 2, hex.end());
    return hex;
}
} // hex
/*
class Hex {
    private:
        std::string _d;
    public:
    Hex(const long long decimal) {
        _d = hex::from_decimal(hex);
    }
    Hex(const std::string hex) {
        _d = hex;
    }
    void from_binary(std::string) {
        _d = hex::from_binary(_d);
    }
    void binary() {
        _d = hex::to_binary(_d);
    }
    long long decimal() {
        return hex:to_decimal(_d);
    }
    Hex& operator=(const long long decimal) {
        _d = hex::from_decimal(decimal);
        return *this;
    }
    Hex& operator=(const std::string hex) {
        _d = hex;
        return *this;
    }
    bool operator==(const long long decimal) {
        return hex::to_decimal(_d) == decimal;
    }
    bool operator==(const std::string hex) {
        return _d == hex;
    }
    bool operator!=(const long long decimal) {
        return hex::to_decimal(_d) != decimal;
    }
    bool operator!=(const std::string hex) {
        return _d != hex;
    }
    bool operator>(const long long decimal) {
        return hex::to_decimal(_d) > decimal;
    }
    bool operator<(const std::string hex) {
        return _d != hex;
    }

};
*/