# cpprafting-interpreters
Crafting interpretes, but in C++. Why C++ when there is Rust? Very good question, and the answer is there is a LOT of C++ code out there, so it's very important that a system engineer is capable in both.
Wait ... I see no copy constructors ... Why? Why are there so many moves? Why is there so much C-like code?
Well, there are very many ways to write C++ in. I like the elegance of C and the ownership system of Rust, so there are a lot of ownership (and move) semantics in the code. It's easier to think in terms of that. Also, copy constructors are a performance killer because it's very easy to abuse them, I like to make this explicit. You will find that the C++ code written in this repo reads a lot like a mix of C and Rust, with occasional C++ shenanigans (god bless shared_ptr).

As long as the code is clear, does the right thing, does not leak memory then any style is fine, this is just how I *currently* prefer to write C++
