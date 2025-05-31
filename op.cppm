
export module cpuf.op;
import std;
import std.compat;
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