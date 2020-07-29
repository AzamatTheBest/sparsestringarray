# sparsestringarray


A sparsestringarray is an array-like data structure that provides constant time access to its
elements, and constant time insertion and deletion. It layers array semantics over an
ordered collection of C strings, with the understanding that most of the strings are empty
