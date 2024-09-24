#include <iostream>
#include <mutex>
#include <atomic>

class singleton1
{
public:
    static singleton1 *GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new singleton1();
        }
        return m_instance;
    }

private:
    singleton1() {}
    ~singleton1()
    {
        std::cout << "~singleton1()\n";
    }
    singleton1(const singleton1 &) = delete;
    singleton1 &operator=(const singleton1 &) = delete;
    singleton1(singleton1 &&) = delete;
    singleton1 &operator=(singleton1 &&) = delete;
    //存储在全局静态区，指针指向的是堆区，堆上需要手动构造和释放，这里没有delete操作，无法被正确释放
    static singleton1 *m_instance; 
};
singleton1 *singleton1::m_instance = nullptr;

//这个是单线程环境下正确的版本
class singleton2
{
public:
    static singleton2 *GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new singleton2();
            atexit(Desturctor);
        }
        return m_instance;
    }

private:
    static void Desturctor() {
        if (m_instance != nullptr) {
            delete m_instance;
            m_instance = nullptr;
        }
    }
    singleton2() {}
    ~singleton2()
    {
        std::cout << "~singleton1()\n";
    }
    singleton2(const singleton2 &) = delete;
    singleton2 &operator=(const singleton2 &) = delete;
    singleton2(singleton2 &&) = delete;
    singleton2 &operator=(singleton2 &&) = delete;
    static singleton2 *m_instance; 
};
singleton2 *singleton2::m_instance = nullptr;

//线程安全的单例
//多处理器，多核心情况下有问题
class singleton3
{
public:
    static singleton3 *GetInstance()
    {
        // std::lock_guard<std::mutex> lock(m_mutex); //粒度太大
        if (m_instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            //双检测，防止new两次
            if (m_instance == nullptr) {
                m_instance = new singleton3();
                // 1. 分配内存
                // 2. 调用构造函数
                // 3. 返回对象指针
                atexit(Desturctor);
            }
        }
        return m_instance;
    }

private:
    static void Desturctor() {
        if (m_instance != nullptr) {
            delete m_instance;
            m_instance = nullptr;
        }
    }
    singleton3() {}
    ~singleton3()
    {
        std::cout << "~singleton1()\n";
    }
    singleton3(const singleton3 &) = delete;
    singleton3 &operator=(const singleton3 &) = delete;
    singleton3(singleton3 &&) = delete;
    singleton3 &operator=(singleton3 &&) = delete;
    static singleton3 *m_instance; 
    static std::mutex m_mutex;
};
singleton3 *singleton3::m_instance = nullptr;
std::mutex singleton3::m_mutex;

//双检查线程安全版本
class singleton4
{
public:
    static singleton4 *GetInstance()
    {
        singleton4* tmp = m_instance_atomic.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            tmp = m_instance_atomic.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                tmp = new singleton4();
                std::atomic_thread_fence(std::memory_order_release);
                m_instance_atomic.store(tmp, std::memory_order_release);
                atexit(Desturctor);
            }
        }
        return tmp;
    }

private:
    static void Desturctor() {
        singleton4 *tmp = m_instance_atomic.load(std::memory_order_relaxed);
        if (tmp != nullptr) {
            delete tmp;
            tmp = nullptr;
        }
    }
    singleton4() {}
    ~singleton4()
    {
        std::cout << "~singleton1()\n";
    }
    singleton4(const singleton4 &) = delete;
    singleton4 &operator=(const singleton4 &) = delete;
    singleton4(singleton4 &&) = delete;
    singleton4 &operator=(singleton4 &&) = delete;
    static std::atomic<singleton4 *> m_instance_atomic;
    static std::mutex m_mutex;
};
std::atomic<singleton4 *> singleton4::m_instance_atomic{nullptr};
std::mutex singleton4::m_mutex;

class singleton5
{
public:
    static singleton5* GetInstance()
    {
        //C++11 静态局部变量线程安全
        //全局静态区内存自动释放，调用析构函数
        //只在第一  GetInstance() 被调用时才创建对象
        static singleton5 instance; 
        return &instance;
    }
private:
    singleton5() {}
    ~singleton5() {
        std::cout << "~singleton5()\n";
    }
    singleton5(const singleton5&) = delete;
    singleton5& operator=(const singleton5&) = delete;
    singleton5(singleton5 &&) = delete;
    singleton5& operator=(singleton5&&) = delete;
};

template<typename T>
class singleton
{
public:
    static T* GetInstance()
    {
        static T instance;
        return &instance;
    }
protected:
    virtual ~singleton() {
        std::cout << "~singleton()\n";
    }
    singleton() {}
private:
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete; 
};

class designPattern : public singleton<designPattern>
{
    friend class singleton<designPattern>; // 友元类,为了让基类可以调用子类的私有函数
public:
    // ...

private:
    designPattern() {}
    ~designPattern() {
        std::cout << "~designPattern()\n";
    }
};

int main() {
    // 堆上资源不能正确析构
    singleton1 *s1 = singleton1::GetInstance();
    // 栈上资源可以正确析构
    singleton2 *s2 = singleton2::GetInstance();
    // 线程安全
    singleton3 *s3 = singleton3::GetInstance();
    // 双检查线程安全
    singleton4 *s4 = singleton4::GetInstance();
    // C++11 静态局部变量线程安全
    singleton5 *s5 = singleton5::GetInstance();
    designPattern *s6 = designPattern::GetInstance();
    return 0;
}