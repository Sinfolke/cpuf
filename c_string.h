#pragma once

#ifndef CPUF_C_STRING
#define CPUF_C_STRING

#include <cstring>
#include <cctype>
#include <istream>
template <typename chartype, unsigned SIZE>
struct __cstring {
  private:
    chartype s[SIZE];
    size_t _sz;
    inline void updateSize(size_t newSize = std::strlen(s)) {
        _sz = newSize;
    }
#ifdef CPUF_C_STRING_DEBUG
#include <stdexcept>
    const char* errs[6] = {
        "constructor",
        "=",
        "+=",
        "access by operator []", /* 21 */
        "copy_n",
        "cut (start pos)"
    };
    [[noreturn]] void error(const unsigned char &n) {
        char mes[35 + 21 + 1 /* \0 */] = "cstring overflow error. At operation: "; /* 35 */
        std::strcat(mes, errs[n]);
        throw std::overflow_error(mes);
    }
#else
    void _set(chartype *&newstr) {
        std::strcpy(s, newstr);
        updateSize();
    }
#endif

  public:
    __cstring() {
       s[0] = '\0';
       _sz = 0;
    }
    __cstring(const chartype *string)
    {
# ifdef CPUF_C_STRING_DEBUG
        if (std::strlen(string) >= SIZE)
            error(0);
# endif
        std::strcpy(s, newstr);
        updateSize();
    }
    __cstring(const chartype &character)
    {
# ifdef CPUF_C_STRING_DEBUG
        if (SIZE < 2)
            error(0)
# endif
        s[0] = character;
        s[1] = '\0';
        updateSize(1);
    }
    auto &operator=(const chartype *string)
    {
# ifdef CPUF_C_STRING_DEBUG
        if (std::strlen(string) >= SIZE)
            error(0);
# endif
        std::strcpy(s, newstr);
        return *this;
        updateSize();
    }
    auto &operator=(const chartype &character)
    {
# ifdef CPUF_C_STRING_DEBUG
        if (_sz >= SIZE)
            error(0);
# endif
        s[0] = character;
        s[1] = '\0';
        updateSize(1);
        return *this;
    }
    auto &operator+=(const chartype *string)
    {
#ifdef CPUF_C_STRING_DEBUG
        if (std::strlen(string) + _sz >= SIZE)
            error(2);
#endif
        std::strcat(s, string);
        updateSize();
        return *this;
    }
    auto &operator+=(const chartype &c)
    {
        if (_sz != 0) _sz++;
#ifdef CPUF_C_STRING_DEBUG
        if (_sz >= SIZE) error(2);
#endif
        s[_sz] = c;
        s[_sz + 1] = '\0';
        _sz++;
        return *this;
    }
    bool operator==(const chartype *string) const
    {
        return std::strncmp(s, string, _sz + 1) == 0;
    }
    bool operator==(const chartype &c) const
    {
        return _sz == 1 && s[0] == c;
    }
    bool operator!=(const chartype *string) const
    {
        return std::strncmp(s, string, _sz + 1) != 0;
    }
    bool operator!=(const chartype &c) const
    {
        return !(s[0] == c && s[1] == '\0');
    }
    inline bool operator>(const chartype *string) const
    {
        return _sz > std::strlen(string);
    }
    inline bool operator>(const chartype &c) const
    {
        return s[1] != '\0';
    }
    inline bool operator<(const chartype *string) const
    {
        return _sz < std::strlen(string);
    }
    inline bool operator>=(const chartype *string) const
    {
        return _sz > std::strlen(string) ||
               std::strncmp(s, string, _sz) == 0;
    }
    inline bool operator>=(const chartype &c) const
    {
        return _sz > 1 || s[0] == c;
    }
    inline bool operator<=(const chartype *string) const
    {
        return _sz < std::strlen(string) ||
               std::strncmp(s, string, _sz) == 0;
    }
    inline bool operator<=(const chartype &c) const
    {
        return _sz == 0 || s[0] == c;
    }
    template <typename T>
    inline T &operator<<(T &stream)
    {
        stream << s;
        return stream;
    }
    std::istream& operator>>(std::istream& stream) {
        unsigned i = 0;
        while (
            std::cin.get(s[i])
                &&
            !std::isspace(s[i])
                &&
            i < SIZE - 1
        ) i++;
        s[i] = '\0';
        return stream;
    }
    template <typename T>
    inline T &operator>>(T &stream)
    {
        stream >> s;
        return stream;
    }
    /*
       strncpy implementation.
       Copies specified characters amount from input string to the current. Unlike native strncpy, it's a safer way. The null terminate character is put automatically.
       string - the string which characters must be copied
       n - the amount of characters to copy.
    */
    void copy_n(const chartype *&string, unsigned &n) const
    {
#ifdef CPUF_C_STRING_DEBUG
        if (n > SIZE || n > std::strlen(string) || n < 0) error(3);
#endif
        std::strncpy(s, string, n);
        s[n + 1] = '\0';
        updateSize(_sz + n);
    }
    /* strchr
        Find certain character in the string and return pointer onto it. null pointer if not found
    */
    inline const chartype* find(const unsigned &character) const
    {
        return std::strchr(s, character);
    }
    /* strstr
        Find string and return pointer onto it. null pointer if not found
    */
    inline const chartype* find(const chartype *&string) const
    {
        return std::strstr(s, string);
    }
    // Cut the string from begin
    inline auto& cut_begin(const unsigned &chars_amount)
    {
        strcpy(s, s + chars_amount);
        updateSize(_sz - chars_amount);
        return *this;
    }
    // Cut the string from end
    auto& cut_end(const unsigned &chars_amount)
    {
        size_t i = _sz - chars_amount;
        for (; i < _sz; i++)
        {
            s[i] = '\0';
        }
        _sz = i;
        return *this;
    }
    /*
        Cut the string from start onto specified characters amount. The start position and the last characters are cutted too.
        "abcde" --> cut(1, 2) --> "ae"
        
        If the start equals last character position string remains unchanged
    */
    inline auto& cut(const unsigned& start, const unsigned& end)
    {
        return fcut(start, start + end);
    }
     /*
        Cut the string from start to end. The start position and the last characters are cutted too.
        "abcde" --> cut(1, 2) --> "ade"
        
        If the start equals last character position string remains unchanged
    */
    inline auto& fcut(const unsigned& start, const unsigned& end)
    {
#ifdef CPUF_C_STRING_DEBUG
        if (start > size || start > end) error(5);
#endif
        if (end > _sz) strcpy(s + start, s + _sz + 1);
        else           strcpy(s + start, s + end + 1);
        updateSize(_sz - end - start);
        return *this;
    }
    int indexOf(const chartype &c) {
        chartype* r = std::strchr(s, c);
        if (r == NULL) {
           return -1;
        }
        return &s - r;
    }
    unsigned indexOf(const chartype* string) {
        chartype* r = std::strstr(s, string);
        if (r == NULL) {
           return -1;
        }
        return &s - r;
    }
    inline size_t size() const { 
        return _sz; 
    }
    constexpr unsigned fullsize() const {
        return SIZE;
    }
    operator chartype*() {
        return s;
    }
    operator const chartype*() {
        return s;
    }
};
template<unsigned size>
using cstring = __cstring<char, size>;
template<unsigned size>
using cwstring = __cstring<wchar_t, size>;

template<unsigned size>
using cstringU16 = __cstring<char16_t, size>;
template<unsigned size>
using cstringU32 = __cstring<char32_t, size>;


#endif  // CPUF_C_STRING
