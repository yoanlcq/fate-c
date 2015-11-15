#include <fst/implementation.h>

int main(int argc, char *argv[])
{
    struct fst_implementation fst_impl;
    fst_implementation_init(&fst_impl);
    return fst_implementation_exec(&fst_impl);
}
