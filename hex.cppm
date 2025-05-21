export module cpuf.hex;
import std;

export namespace hex {
    long long to_decimal(std::string hex);
    std::string to_binary(std::string hex);
    std::string to_binary(long long num);
    std::string from_binary(const std::string& bin);
    std::string from_decimal(long long num);
}