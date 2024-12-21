#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <atomic>
#include <cassert>
#include <algorithm>

const int N = 100000;
std::vector<int> S(N);              // 整数数组
std::shared_mutex locks[N];
std::atomic<int> lock_conflicts{0}; // 记录锁争用次数

// 获取随机数
int getRandomIndex()
{
  static thread_local std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<> dist(0, N - 1);
  return dist(rng);
}

void updateArray(int i, int j)
{
  int i1 = (i + 1) % N;
  int i2 = (i + 2) % N;

  if (i == j || i == i1 || i == i2)
    return;

  // 多个线程对锁的获取顺序不同容易导致死锁
  // 所以这里采用全局锁顺序的策略，但是经测试仍然可能出现死锁的情况
  std::vector<int> indices = {i, i1, i2, j};
  std::sort(indices.begin(), indices.end());
  auto last = std::unique(indices.begin(), indices.end());
  indices.erase(last, indices.end());

  std::vector<std::unique_lock<std::shared_mutex>> locks_list;
  
  for (int idx : indices)
  {
    bool conflict = !locks[idx].try_lock(); // 尝试获取锁
    if (conflict)
    {
      lock_conflicts++;  // 记录锁争用
      locks[idx].lock(); // 阻塞等待锁
    }
    locks_list.emplace_back(std::unique_lock<std::shared_mutex>(locks[idx], std::adopt_lock));
  }

  S[j] = S[i] + S[i1] + S[i2];
}

void worker(int id, int num_operations)
{
  for (int op = 0; op < num_operations; ++op)
  {
    int i = getRandomIndex();
    int j = getRandomIndex();
    updateArray(i, j);
  }
}

int main()
{
  for (int i = 0; i < N; ++i)
  {
    S[i] = i;
  }

  int num_workers = 4;        // 工作者数量
  int num_operations = 10000; // 每个工作者的操作次数

  auto start_time = std::chrono::high_resolution_clock::now();

  std::vector<std::thread> workers;
  for (int i = 0; i < num_workers; ++i)
  {
    workers.emplace_back(std::thread(worker, i, num_operations));
  }

  for (auto &w : workers)
  {
    w.join();
  }

  auto end_time = std::chrono::high_resolution_clock::now();

  std::cout << "S[100] = " << S[100] << std::endl;
  std::cout << "Execution time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
            << " ms" << std::endl;
  std::cout << "Lock conflicts: " << lock_conflicts.load() << std::endl;

  return 0;
}