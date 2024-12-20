#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <atomic>
#include <cassert>

const int N = 100000;  // 数组大小
std::vector<int> S(N); // 共享数组 S

// 用于保护数组中每个元素的锁
std::shared_mutex locks[N];

// 获取随机数
int getRandomIndex()
{
  static std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<> dist(0, N - 1);
  return dist(rng);
}

// 更新数组
void updateArray(int i, int j)
{
  int i1 = (i + 1) % N;
  int i2 = (i + 2) % N;

  // 按一致的顺序获取锁，防止死锁
  if (i < i1)
  {
    std::unique_lock<std::shared_mutex> lock1(locks[i]);
    std::unique_lock<std::shared_mutex> lock2(locks[i1]);
    std::unique_lock<std::shared_mutex> lock3(locks[i2]);
    std::unique_lock<std::shared_mutex> lock4(locks[j]);

    // 执行更新操作
    S[j] = S[i] + S[i1] + S[i2];
  } else {
    std::unique_lock<std::shared_mutex> lock1(locks[i]);
    std::unique_lock<std::shared_mutex> lock2(locks[i1]);
    std::unique_lock<std::shared_mutex> lock3(locks[i2]);
    std::unique_lock<std::shared_mutex> lock4(locks[j]);

    // 执行更新操作
    S[j] = S[i] + S[i1] + S[i2];
  }
}

// 工作线程函数，模拟并发访问和更新
void worker(int id, int num_operations)
{
  for (int op = 0; op < num_operations; ++op)
  {
    int i = getRandomIndex();
    int j = getRandomIndex();

    // 确保 j 不在 [i, i+2] 范围内
    if (j >= i && j <= i + 2)
    {
      continue;
    }

    // 使用 2PL 更新数组
    updateArray(i, j);
  }
}

int main()
{
  // 初始化数组
  for (int i = 0; i < N; ++i)
  {
    S[i] = i;
  }

  // 工作线程数量和每个线程的操作次数
  int num_workers = 4;        // 例如 4 个工作线程
  int num_operations = 10000; // 每个线程执行 10,000 次操作

  // 创建工作线程
  std::vector<std::thread> workers;
  for (int i = 0; i < num_workers; ++i)
  {
    workers.push_back(std::thread(worker, i, num_operations));
  }

  // 等待所有工作线程完成
  for (auto &w : workers)
  {
    w.join();
  }

  // 可选，打印一个示例结果以验证更新的正确性
  std::cout << "S[100] = " << S[100] << std::endl;

  return 0;
}
