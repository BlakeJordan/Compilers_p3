set breakpoint pending on
file ./cRAPc
break cRAP::ToDoError::ToDoError
break cRAP::InternalError::InternalError

define t3
  set args tests/$arg0.lake -p --
  run
end
