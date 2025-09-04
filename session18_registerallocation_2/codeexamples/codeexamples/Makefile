CC=clang
source=test.c

twoaddr: test.c
	clang -O0 -emit-llvm -S -Xclang -disable-O0-optnone $< -o $@.ll
	llc -O0 -global-isel -stop-after=instruction-select -simplify-mir -o - twoaddr.ll > twoaddrafterisel.mir
	llc -mtriple=x86_64 -run-pass=twoaddressinstruction twoaddrafterisel.mir -o aftertwoaddr.mir

elimphi: example3_after_mem2reg.ll
	llc -O0 -global-isel -stop-after=instruction-select $< -o - > $@.mir
	llc -mtriple=x86_64 -run-pass=phi-node-elimination $@.mir -o $@_after.mir

coalescer: elimphi
	llc -mtriple=x86_64 -run-pass=register-coalescer elimphi_after.mir -o elimcopy_after_coalesce.mir

dumpregalloc: test.c
	clang -O0 -emit-llvm -S -Xclang -disable-O0-optnone $< -o $@.ll
	llc -debug-only=regalloc test.ll 2> test_regalloc_debugdump.txt	

regallocator: elimphi	
	llc -mtriple=x86_64 -run-pass=greedy elimphi_after.mir -o elimphi_after_greedy.mir
	llc -mtriple=x86_64 -regalloc=basic elimphi_after.mir -o elimphi_after_basic.mir -run-pass=greedy
	llc -mtriple=x86_64 -regalloc=fast elimphi_after.mir -o elimphi_after_fast.mir -run-pass=greedy

clean:
	rm -f test*.ll twoaddr.ll *.mir *.s


#exercise: eliminate COPY from elimcopy_after_greedy.mir
