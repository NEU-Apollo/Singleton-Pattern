# Singleton-Pattern

up:底层原理剖析

单例模式要点
1. 构造函数和析构函数是私有的，不允许外部生成和释放
2. 静态成员变量和静态返回单例的成员函数
3. 仅有拷贝构造函数和复制运算符

cd "/home/weisiyuan/test/Singleton-Pattern/" && g++ -std=c++17 Singleton.cpp -o Singleton.exe && "/home/weisiyuan/test/Singleton-Pattern/"Singleton.exe
singleton_onceflag::GetInstance()
terminate called after throwing an instance of 'std::system_error'
  what():  Unknown error -1
Aborted
解决办法：
编译时候加上 -pthread 参数
报错原因：
1. 什么是 -pthread？
-pthread 是 g++（或 gcc）编译器的一个选项，用来启用 POSIX 线程（即 pthreads）。POSIX 线程是一种线程管理的标准，广泛应用于类 Unix 系统（例如 Linux）。

当你在程序中使用像 std::call_once、std::mutex 或 std::thread 这样的同步机制时，它们在内部依赖于 pthread 库来管理和同步线程。如果在编译时没有加入 -pthread 标志，程序就无法链接到必要的线程库，从而导致运行时错误。

2. 为什么 std::call_once 受影响？
std::call_once 是 C++ 线程库的一部分，它依赖操作系统提供的线程支持。其作用是确保某段代码（例如单例模式中的初始化）只被执行一次，即使有多个线程同时运行。

如果编译时没有使用 -pthread 标志，链接器不会包含所需的线程管理代码，因此当程序试图调用 std::call_once 时会失败，因为系统无法找到需要的线程相关函数。这会导致抛出 std::system_error，并伴随不明确的错误代码（如你遇到的“Unknown error -1”）。

3. -pthread 如何解决问题？
通过添加 -pthread 标志：

编译器知道你正在编写一个多线程应用程序。
在链接阶段，pthread 库会自动加入到你的程序中。
这确保了类似 std::call_once、std::mutex 和 std::thread 等多线程相关的功能能够正常工作，因为系统现在可以正确处理线程的创建、同步和管理。
总结来说，-pthread 标志确保了你的应用程序能够在系统层面上访问必要的线程功能，从而使 C++ 中的线程相关特性可以正常工作，不会再遇到运行时错误。