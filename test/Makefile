PROG := axpy.cu

CC := clang++

PASS := ../build/RegAllocControl/libRegAllocControlPass.so

INCPATH := /usr/local/cuda-7.5/samples/common/inc
LIBPATH := /usr/local/cuda-7.5/lib64/
LIBS := -lcudart -lcuda 

OPTLEVEL := -O0
CLANGOPTS := $(OPTLEVEL) -Xclang -load -Xclang $(PASS) -mllvm -reg_control=0

LLVMOPTS := -mllvm -debug-pass=Arguments

all: 
	$(CC) $(CLANGOPTS) $(LLVMOPTS) -I $(INCPATH) -L $(LIBPATH) $(LIBS) $(PROG) 

clean:
	rm -rf a.out *~
