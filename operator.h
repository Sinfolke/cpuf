#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <forward_list>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <span>
namespace cpuf::op {
    extern bool addType;
}
#if __cplusplus >= 202302L // C++26
    #include <vector>  // std::inplace_vector
    //#include <hive>
#endif

// Generic template for range-based containers
template <typename Container>
std::ostream& printContainer(std::ostream& os, const Container& container, const char* name) {
    if (cpuf::op::addType)
        os << name << ' ';
    os << "{ ";
    for (const auto& elem : container) {
        os << elem << ", ";
    }
    os << "}";
    return os;
}
// Overload for key-value containers (map, multimap)
template <typename Container>
std::ostream& printKeyValueContainer(std::ostream& os, const Container& container, const char* name) {
    if (cpuf::op::addType)
        os << name << ' ';
    os << "{ ";
    for (const auto& [key, value] : container) {
        os << "(" << key << ": " << value << "), ";
    }
    os << "}";
    return os;
}

// Specialization for std::array (fixed size)
template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr) {
    return printContainer(os, arr, "std::array");
}

// Specialization for std::deque (double-ended queue)
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& dq) {
    return printContainer(os, dq, "std::deque");
}

// Specialization for std::forward_list (singly linked list)
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& fl) {
    return printContainer(os, fl, "std::forward_list");
}

#if __cplusplus >= 202302L
// Specialization for std::inplace_vector (C++26)
template <typename T, std::size_t Capacity>
std::ostream& operator<<(std::ostream& os, const std::inplace_vector<T, Capacity>& iv) {
    return printContainer(os, iv, "std::inplace_vector");
}

// Specialization for std::hive (C++26)
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::hive<T>& hv) {
    return printContainer(os, hv, "std::hive");
}
#endif
// specializaton for std::vector
template<class OS, class T>
typename std::enable_if<std::is_base_of<std::ostream, OS>::value, OS&>::type
operator<<(OS &os, const std::vector<T> &vec) {
    return printContainer(os, vec, "std::vector");
}
// specializaton for std::list
template<class OS, class T>
typename std::enable_if<std::is_base_of<std::ostream, OS>::value, OS&>::type
operator<<(OS &os, const std::list<T> &vec) {
    return printContainer(os, vec, "std::list");
}


// Specialization for std::set
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s) {
    return printContainer(os, s, "std::set");
}

// Specialization for std::multiset
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::multiset<T>& ms) {
    return printContainer(os, ms, "std::multiset");
}

// Specialization for std::map
template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
    return printKeyValueContainer(os, m, "std::map");
}

// Specialization for std::multimap
template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::multimap<K, V>& mm) {
    return printKeyValueContainer(os, mm, "std::multimap");
}

// Specialization for std::unordered_set
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& us) {
    return printContainer(os, us, "std::unordered_set");
}

// Specialization for std::unordered_multiset
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_multiset<T>& ums) {
    return printContainer(os, ums, "std::unordered_multiset");
}

// Specialization for std::unordered_map
template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& um) {
    return printKeyValueContainer(os, um, "std::unordered_map");
}

// Specialization for std::unordered_multimap
template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_multimap<K, V>& umm) {
    return printKeyValueContainer(os, umm, "std::unordered_multimap");
}
// Print function for stack
template <typename T, typename Container>
std::ostream& operator<<(std::ostream& os, std::stack<T, Container> s) {
    if (cpuf::op::addType)
        os << "std::stack ";
    os << "{ ";
    while (!s.empty()) {
        os << s.top() << " ";
        s.pop();
    }
    os << "}";
    return os;
}

// Print function for queue
template <typename T, typename Container>
std::ostream& operator<<(std::ostream& os, std::queue<T, Container> q) {
    if (cpuf::op::addType)
        os << "std::queue ";
    os << "{ ";
    while (!q.empty()) {
        os << q.front() << " ";
        q.pop();
    }
    os << "}";
    return os;
}

// Print function for priority_queue
template <typename T, typename Container, typename Compare>
std::ostream& operator<<(std::ostream& os, std::priority_queue<T, Container, Compare> pq) {
    if (cpuf::op::addType)
        os << "std::priority_queue ";
    os << "{ ";
    while (!pq.empty()) {
        os << pq.top() << " ";
        pq.pop();
    }
    os << "}";
    return os;
}
#if __cplusplus >= 202002L
// Print function for std::span (C++20)
template <typename T, std::size_t Extent>
std::ostream& operator<<(std::ostream& os, std::span<T, Extent> sp) {
    return printContainer(os, sp, "std::span");
}
#endif
#ifdef __cpp_lib_mdspan
// Print function for std::mdspan (C++23)
template <typename T, std::size_t... Extents>
std::ostream& operator<<(std::ostream& os, std::mdspan<T, Extents...> mdsp) {
    os << "std::mdspan { ";
    for (std::size_t i = 0; i < mdsp.extent(0); ++i) {
        os << mdsp[i] << " ";
    }
    os << "}";
    return os;
}
#endif