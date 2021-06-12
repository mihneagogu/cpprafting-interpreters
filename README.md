# cpprafting-interpreters
Crafting interpretes, but in C++. Why C++? I like pain.
Wait ... I see no copy constructors ... Why? Why are there so many moves? Why is there so much C-like code?
Well, there are very many ways to write C++ in. I like the elegance of C and the ownership system of Rust, so there are a lot of ownership (and move) semantics in the code. It's easier to think in terms of that. Also, copy constructors make a lot more allocations, even if sometimes not necessary. It's personally more intuitive to use T& like a rust shared reference borrow and T&& rvalues when moving into functions. The reason why you see a bit of C-like code is because C keeps it simple, while it lacks some nice features (which C++ has). I try to keep the simplicity of C with a bit of the extra C++ help.

As long as the code is clear, does the right thing, does not leak memory then any style is fine, this is just how I *currently* prefer to write C++
