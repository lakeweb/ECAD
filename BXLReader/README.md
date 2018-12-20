# BXLReader
Read and Parse BXL Electronic Component Description Files

Ultimate usage is to expose the Component namespace to the user and let the rest happen behind the scene.

Thanks to erichVK5, https://github.com/erichVK5, for the work on BXLtoText and the Boost authors.

If using the Microsoft Compiler, it must be vs141, VS2017, as the older compilers are not compliant enough to compile Boost::Spirit::x3.

4/11/2018
Converted to streaming file and fixed memory leaks.
