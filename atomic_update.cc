#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>

const int N = 100000;
std::vector<std::atomic<int>> S(N); // 使用原子操作的整数数组

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

  // 无锁原子更新
  int new_value = S[i].load() + S[i1].load() + S[i2].load();
  S[j].store(new_value);
}

// 工作线程
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
  for (int i = 0; i < N; ++i)
  {
    S[i].store(i);
  }

  int num_workers = 4;        // 工作者数量
  int num_operations = 10000; // 每个工作者的操作次数

  // 计时开始
  auto start_time = std::chrono::high_resolution_clock::now();

  // 创建工作线程
  std::vector<std::thread> workers;
  for (int i = 0; i < num_workers; ++i)
  {
    workers.emplace_back(std::thread(worker, i, num_operations));
  }

  // 等待所有工作线程完成
  for (auto &w : workers)
  {
    w.join();
  }

  // 计时结束
  auto end_time = std::chrono::high_resolution_clock::now();

  // 输出结果
  std::cout << "S[100] = " << S[100].load() << std::endl;
  std::cout << "Execution time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
            << " ms" << std::endl;

  return 0;
}
