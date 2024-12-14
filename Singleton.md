# 单例模式

顾名思义，单例模式就是只提供一个类的实例。

简单来说的需要思考到的要点有：

- 全局只存在一个实例，不能拷贝和赋值
- 有实例时直接返回该实例，没有实例时创建实例
- 创建实例考虑线程安全

## 双重锁校验

两次锁的出现其实都是为了确保线程安全，

**只有确保实例为空**时，我们才需要加锁创立新的实例，

加完第一次锁之后，可能**其它线程已经创建实例**，此时需要再次进行判断，判断为空后就可以创建实例。

```cpp
class Singleton {
 private:
  static volatile Singleton* instance;
  static std::mutex mtx;
  Singleton() {}

 public:
        Singleton(Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
  
  static Singleton* getInstance() {
   if (instance == nullptr) {
    std::lock_guard<std::mutex> lock(mtx);
    if (instance == nullptr) {
     instance = new Singleton();
    }
   }
   return const_cast<Singleton*>(instance);
  }
};

volatile Singleton* Singleton::instance = nullptr;
std::mutex Singleton::mtx;
```

## 静态局部变量

由于static只会在同一个地方分配内存，并且即使在局部函数中创建它的作用域也是在全局，利用这种特性，同样可以实现单例。

并且巧妙的利用了c++11中的一个特性：**magic static**

> 变量初始化时进入声明，并发线程会阻塞等待初始化结束。

因此

```cpp
class Singleton {
 private:
  Singleton() {};

 public:
  ~Singleton() {};
  Singleton(Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
    
  static Singleton& get_instance() {
   static Singleton instance;
   return instance;
  }
};

int main() {
 Singleton& instance_1 = Singleton::get_instance();
 Singleton& instance_2 = Singleton::get_instance();

 return 0;
}
```

注意使用时也需要采用&的方式。
