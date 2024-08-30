/*
    A function used in decompiled C code to use assembler functions where
    no code fully decompilable
*/

#if !defined(__GNUC__) && !defined(__clang__)
#error "Sadly asm function works only with gcc and clang"
#endif
enum class REGISTER {
    EAX, EBX
};
void mov() {}
void mov(REGISTER A, REGISTER B) {}