# (Register Allocation - 2)

This directory contains slides and codeexamples on register allocation in LLVM. 

Acknowledgements (slides courtesy):

- https://www.cs.cmu.edu/afs/cs/academic/class/15745-s18/www/lectures/L12-Register-Allocation.pdf
- Prof. Andrew Myers

Further reading / Viewing:
- https://web.cs.ucla.edu/~palsberg/paper/aplas05.pdf
- https://www.youtube.com/watch?v=IK8TMJf3G6U


## I. Installing `llvm`

```
git clone https://github.com/llvm/llvm-project.git
git checkout tags/llvmorg-20.1.0
```

Building with cmake and Ninja:
- see `build_command.txt`
- After successful build, edit `setenv.sh` and execute `source setenv.sh` to point to your newly built Clang compiler.



## II. Examples (in codeexample directory)
- `make twoaddr`. Uses the `twoaddressinstruction` pass. This creates MIR file after rewriting using two address instruction. Observe the differences in `twoaddrafterisel.mir` and `aftertwoaddr.mir`.
- `make elimphi`. Uses the `phi-node-elimination` pass. This eliminates the phi node with a COPY. observe `elimphi.mir` and `elimphi_after`.mir.
- `make coalescer`. Uses the `register-coalescer` pass. This removes some COPY from MIR. observe `elimphi_after.mir` and `elimcopy_after_coalesce.mir`.
- `make dumpregalloc`. Uses the `-debug-only=regalloc` option with llc to record detailed activity of the register allocator. Observe how slot indexes, live intervals, register coalescing, and register allocator work. 
- `make regallocator`. Selects available register allocators in LLVM with `-regalloc=basic` and `-regalloc=fast` in combination with `-run-pass=greedy` option to the llc. For each register allocator, this produces mir files that can be compared.


**exercise**: Mentioned at the end of the `Makefile`.


