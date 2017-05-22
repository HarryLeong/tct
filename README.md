tct
===

If You Can Read Chinese,Pelase Read `中文README.md`, Else, Sorry, You Must Continue To Read Chinglist.

##Abstract
This is a CUI tool for counting the number of files in some directories.
or the number of lines of those files.The origin idea is counting the lines
of code I wrote.

##How to use
This is a CUI tool.
You can use it, just like `tct -d=.`
This tool will print number of files in current working directory and number of lines of
all those files.

The file `count-self.bat` is a good example.

Use `tct --help` to get more information about how to use this tool.

##How to build
It bases c++11, particular filesystem library.
If you are using VS2015 or newer version(Express is enough, it is free.), you can compile this by following setps.

*   Download and Install cmake(It was known cmake version of >3.8 can work, not sure of lower version.).

*   Run `build-win32.bat`

*   Go to directory `tct-win32/`,You should see `tct.sln`(You know what it is).
