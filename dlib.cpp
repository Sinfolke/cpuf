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
#include "dlib.h"
namespace std {
    /**
     * @brief The default flag for the dlib class when the program is compiled for unix systems. On Windows all interaction ignored.
     *        Default state is RTLD_LAZY
     */
    int dlibDefFlags = 0; 
    dlib_exception::dlib_exception(const char*& e, Kind k) {
        kind = k;
        m += e;
    }

    dlib_exception::dlib_exception(const std::string& e, Kind k) {
        kind = k;
        m += e;
    }

    const char* dlib_exception::what() const noexcept {
        return m.c_str();
    }
}
dlib::dlib(const char* path) {
    loadlib(path);
}
dlib::dlib(const std::string path) {
    loadlib(path);
}
dlib::dlib() {}
dlib::~dlib() {
    free();
}
// Free the dynamic library if loaded
void dlib::free() {
    if (handler != nullptr) {
    #ifdef _WIN32
        FreeLibrary(handler);
    #else
        dlclose(handler);
    #endif
        handler = nullptr;
    }
}
#ifdef CPUF_DLIB_NO_EXTENSION
/**
 * @brief Load a library. If the library is already loaded it is overriden
 * 
 * @param path The path to the dynamic library without extension.
 * The extension is added based on the platform an app is compiled for.
 * You may disable automatic extension by defining CPUF_DLIB_NO_EXTENSION
 */
inline void dlib::loadlib(const const char* path) {
    loadlib(std::string(path));
}
inline void dlib::loadlib(const std::string path) {
    if (handler) free();
#   ifdef _WIN32
    handler = LoadLibraryA(path.c_str());
#   else
    handler = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
#   endif
    if (handler == nullptr) {
        std::string m = "cannot open '" + path + "'";
        throw std::dlib_err(m, std::dlib_err::Kind::loadlib);
    }
}
#else
/**
 * @brief Load a library. This function is directly used by constructor. If the library is already loaded it is overriden
 * 
 * @param path The path to the dynamic library without extension.
 * The extension is added based on the platform an app is compiled for.
 * You may disable automatic extension by defining CPUF_DLIB_NO_EXTENSION
*/
void dlib::loadlib(const char* path) {
    loadlib(std::string(path));
}
void dlib::loadlib(std::string path) {
#ifdef _WIN32
    path += ".dll";
    handler = LoadLibraryA(path.c_str());
#else
    #ifdef __APPLE__
    path += ".dylib";
    #else
    path += ".so";
    #endif
    handler = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif
    if (handler == nullptr) {
        std::string error_message = "cannot open '" + path + "', error: " + dlerror();
        throw std::dlib_exception(error_message, std::dlib_exception::Kind::loadlib);
    }

}
#endif
/**
 * @brief Sets the flag to value you specify
 * @param flag Flag you specify. Use | operator if need multiple
 */
inline void dlib::setFlag(const int& flag) {
#ifndef _WIN32
    f = flag;
#endif
}
/**
 * @brief Adds flag you specify 
 * @param flag Flag you specify. Use | operator if need multiple
 */
inline void dlib::addFlag(const int& flag) {
#ifndef _WIN32
    f |= flag;
#endif
}
/**
 * @brief Remove flag you specify 
 * @param flag Flag to remove
 */
inline void dlib::removeFlag(const int& flag) {
#ifndef _WIN32
    f &= ~flag;
#endif
}
/**
 * @brief Set flags to its default state (std::dlibDefFlags)
*/
inline void dlib::clearFlags() {
#ifndef _WIN32
    f = std::dlibDefFlags;
#endif
}
#undef CPUF_DLIB_NO_EXTENSION
#undef CPUF_DLIB_DEFAULT_FLAG
