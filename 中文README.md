tct
===

##Abstract
这是一个命令行工具,用来统计文本文件行数.初始的想法是,统计我写的源文件行数.

##How to use
这是一个命令行工具
使用起来大概就是`tct -d=.`
他会打印当前工作目录的所有文件的数量和所有文件的行数.

一个具体的例子可以看`count-self.bat`文件.

使用`tct --help`来看看具体怎么使用.

##How to build
源代码基于c++11.
它依赖于boost库和标准库.
如果你使用VS2012(Express版本就可以了,它免费).那么你可以通过以下步骤来编译.

*   首先,你一定要先编译boost库,它需要boost filesystem模块.

*   下载并且安装cmake

*   打开cmd,输入`cmkae`如果一切正常,你应该得到一堆输出.

*   打开`CMakeLists.txt`并且修改如下变量.

    (1)将include_directories( "../boost/boost_1_53_0/build/include/boost-1_53")改为
    include_directories( "your/boost/incude/path")

    (2)将link_directories("../boost/boost_1_53_0/build/lib") 改为
    link_directories( "your/boost/lib/path(this file contains .lib .dll ...)")

*   将boost的dll所在目录添加到`PATH`环境变量.

*   运行`build-win32.bat`

*   到`tct/../tct-win32/`目录,应该可以看到`tct.sln`(你应该知道是什么吧).