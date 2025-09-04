; ModuleID = 'test/example3.ll'
source_filename = "test/example3.ll"

define i32 @test(i32 %a, i32 %b, i1 %cond) {
entry:
  br i1 %cond, label %then, label %else

then:                                             ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %then
  %x.0 = phi i32 [ %a, %then ], [ %b, %else ]
  ret i32 %x.0
}
