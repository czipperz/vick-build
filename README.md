# vick-build

This is a program to compile `vick`.  It is built because previous
build systems for `vick` were bad.

* Make was hard to use and was slow.  In addition, making non trivial
  dependencies work was a major pain and very hard to work with.
* Shake takes forever to install on new systems.  Like three minutes.
  It's ridiculous.  That made continuous integration a huge pain.

This project will emulate the shake build but will be fast to install
on new systems.
