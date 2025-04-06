#pragma once
// definions
// CPUF_DLIB_NO_EXTENSION
// CPUF_DLIB_DEFAULT_FLAG x
# include <stdexcept>
# include <string>
# ifdef _WIN32
#  include <Windows.h>
# else
#  include <dlfcn.h>
# endif
namespace std {
    /**
     * @brief The default flag for the dlib class when the program is compiled for unix systems. On Windows all interaction ignored.
     *        Default state is RTLD_LAZY
     */
    extern int dlibDefFlags; 
    class dlib_exception : public std::exception {
    public:
        enum class Kind {
            loadlib,
            loadfun,
            load
        };
        dlib_exception(const char*& e, Kind k);

        dlib_exception(const std::string& e, Kind k);

        virtual const char* what() const noexcept override;
        Kind kind;
    private:
        std::string m;
    };
}
/**
 * @brief A cross-platform class for dynamic library loading. 
 */
class dlib {
    public:
    /**
     * @brief Construct the object and load dynamic library
     *
     * @param path The path to the dynamic library without extension.
     * The extension is added based on the platform an app is compiled for.
     * You may disable automatic extension by defining CPUF_DLIB_NO_EXTENSION
     */
    dlib(const char* path);
    dlib(const std::string path);
    dlib();
    ~dlib();
    // Free the dynamic library if loaded
    void free();
#ifdef CPUF_DLIB_NO_EXTENSION
    /**
     * @brief Load a library. If the library is already loaded it is overriden
     * 
     * @param path The path to the dynamic library without extension.
     * The extension is added based on the platform an app is compiled for.
     * You may disable automatic extension by defining CPUF_DLIB_NO_EXTENSION
     */
    inline void loadlib(const std::string path);
#else
    /**
     * @brief Load a library. This function is directly used by constructor. If the library is already loaded it is overriden
     * 
     * @param path The path to the dynamic library without extension.
     * The extension is added based on the platform an app is compiled for.
     * You may disable automatic extension by defining CPUF_DLIB_NO_EXTENSION
    */
    void loadlib(const char* path);
    void loadlib(std::string path);
#endif
    /**
     * @brief Sets the flag to value you specify
     * @param flag Flag you specify. Use | operator if need multiple
     */
    inline void setFlag(const int& flag);
    /**
     * @brief Adds flag you specify 
     * @param flag Flag you specify. Use | operator if need multiple
     */
    inline void addFlag(const int& flag);
    /**
     * @brief Remove flag you specify 
     * @param flag Flag to remove
     */
    inline void removeFlag(const int& flag);
    /**
     * @brief Set flags to its default state (std::dlibDefFlags)
    */
    inline void clearFlags();
    /**
     * @brief Load function from currently loaded library
     * 
     * @tparam ReturnT      The function return type
     * @tparam Params       Function parameter types
     * @param name          The name of function
     * @return returns it's pointer that you can use to call within your code
     */
    template<typename ReturnT, typename... Params>
    inline ReturnT(*loadfun(const char* name))(Params...) {
        auto p = reinterpret_cast<ReturnT(*)(Params...)>(
#ifdef _WIN32
            GetProcAddress(handler, name)
#else
            dlsym(handler, name)
#endif
        );
        if (p == nullptr) {
            free();
            std::string n = "cannot load function '";
            n += name;
            n+= "'";
            throw std::dlib_exception(n, std::dlib_exception::Kind::loadfun);
        }
        return p;
    }
    template<typename ReturnT, typename... Params>
    inline ReturnT(*loadfun(std::string name))(Params...) {
        auto p = reinterpret_cast<ReturnT(*)(Params...)>(
#ifdef _WIN32
            GetProcAddress(handler, name.c_str())
#else
            dlsym(handler, name.c_str())
#endif
        );
        if (p == nullptr) {
            free();
            name.insert(0, "cannot load function '");
            name += "'";
            throw std::dlib_exception(name, std::dlib_exception::Kind::loadfun);
        }
        return p;
    }
    /**
     * @brief Load something from dynamic library with your own type
     * 
     * @tparam T    The type of thing you want to load
     * @param name  The name of thing you want to load
     * @return T    Returns pointer to thing you want to load
     */
    template<typename T>
    T load(std::string name) {
        T p = reinterpret_cast<T>(
#ifdef _WIN32
            GetProcAddress
#else
            dlsym
#endif
                (handler, name.c_str())
        );
        if (p == nullptr) {
            free();
            name.insert(0, "cannot load '");
            name += "'";
            throw std::dlib_exception(name, std::dlib_exception::Kind::load);
        }
        return p;
    }
    /**
     * @brief Whether currently library is loaded (handler != nullptr)
     * 
     * @return true 
     * @return false 
     */
    constexpr bool isLoaded() const {
        return handler != nullptr;
    }
    /**
     * @brief Whether currently library is not loaded and handler is being free (handler == nullptr)
     * 
     * @return true 
     * @return false 
     */
    constexpr bool isfree() const {
        return handler == nullptr;
    }
    private:
    template <typename Signature>
    struct FunctionPtr;

    template <typename R, typename... Args>
    struct FunctionPtr<R(Args...)> {
        using type = R (*)(Args...);
    };
#ifdef _WIN32
        HMODULE handler;
#else
        void* handler;
        int f;
#endif
};


#undef CPUF_DLIB_NO_EXTENSION
#undef CPUF_DLIB_DEFAULT_FLAG
