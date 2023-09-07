# mandlebrot_multithread
Multithreaded mandlebrot set calculator. Programmed it to get familiar with multithreaded workloads and atomics in C++


To compile use:

`g++ .\mand.cpp lodepng.cpp -Ofast -Wall -Wextra -pedantic ` 

To run use:

`.\a.exe <real> <imaginary> <range> <width> <threads>`

Exmaple images:

![mandlebrot1](./examples/mandlebrot3.png)
![mandlebrot2](./examples/mandlebrot2.png)
![mandlebrot3](./examples/mandlebrot.png)


