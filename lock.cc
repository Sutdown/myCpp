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

const int N = 100000;  // 数组大小
std::vector<int> S(N); // 共享数组 S
std::shared_mutex locks[N];

// 获取随机数
int getRandomIndex()
{
  static thread_local std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<> dist(0, N - 1);
  return dist(rng);
}

// 更新数组
void updateArray(int i, int j)
{
  int i1 = (i + 1) % N;
  int i2 = (i + 2) % N;

  // 加锁
  std::vector<int> indices = {i, i1, i2, j};
  std::sort(indices.begin(), indices.end());
  std::vector<std::unique_lock<std::shared_mutex>> locks_list;
  for (int idx : indices) {
    locks_list.emplace_back(locks[idx]);
  }

  // 更新
  S[j] = S[i] + S[i1] + S[i2];
}

// 工作线程，模拟并发访问和更新
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
  // 初始化数组
  for (int i = 0; i < N; ++i) {
    S[i] = i;
  }

  // 工作线程数量和每个线程的操作次数
  int num_workers = 4;        
  int num_operations = 10000; 

  // 创建工作线程
  std::vector<std::thread> workers;
  for (int i = 0; i < num_workers; ++i) {
    workers.emplace_back(std::thread(worker, i, num_operations));
  }

  // 等待所有工作线程完成
  for (auto &w : workers) {
    w.join();
  }

  std::cout << "S[100] = " << S[100] << std::endl;

  return 0;
}
