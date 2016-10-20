
LLVM pass to control aggressiveness of register allocation of CUDA kernels

Build:

    $ cd RegAllocControl
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -Xclang -load -Xclang build/RegAllocControl/RegAllocControl.* foo.cu
