tct
===

Sorry! windows only yet! But it is eary to port to other platform(just use boost, stdlib)!If You Are Chinese,Pelase Read `中文README.md`,Else,Sorry Again, You Must Continue To Read Chinglist.

##Abstract
This is a CUI tool for counting the number of files in some directories.
or the number of lines of those files.The origin idea is count the lines
of code I write.

##How to use
This is a CUI tool.
using it is just like `tct -d=.`
This tool will print number of files in current working directory and number fo lines of
all those files.

The file `count-self.bat` is a good example.

Use `tct --help` to get more information about how to use this tool.

##How to build
It bases c++11.
It relies boost filesystem module and std library.That's all.

If you use VS2012(Express is enough,it is free.), you can also compile this by following setps.

*   you should build boost filesystem projects first.It need filesystem module.

*   Download and Install cmake(If you have not.).

*   Open cmd and input`cmake`.You should get a lot of output.

*   Open file `CMakeLists.txt` and change these variable

    (1)change include_directories( "../boost/boost_1_53_0/build/include/boost-1_53") to
    include_directories( "your/boost/incude/path")

    (2)change link_directories("../boost/boost_1_53_0/build/lib") to
    link_directories( "your/boost/lib/path(this file contains .lib .dll ...)")

*   Add the boost dll to your `PATH` environment variable.

*   Run `build-win32.bat`

*   Go to directory `tct/../tct-win32/`,You should see `tct.sln`(You hnown what it is).