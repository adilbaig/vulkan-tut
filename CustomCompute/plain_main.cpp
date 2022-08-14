#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

const auto n = std::thread::hardware_concurrency();
const uint32_t NumElements = 300 * 1000000;
std::vector<uint32_t> InBuffer(NumElements);
std::vector<uint32_t> OutBuffer(NumElements);

void doSomething(uint32_t start, uint32_t end)
{
  for (uint32_t I = start; I < end; ++I)
  {
    OutBuffer[I] = InBuffer[I] * 2;
  }
}

int main()
{
  // Fill in NumElements of numbers to InBuffer
  for (uint32_t I = 0; I < NumElements; ++I)
  {
    InBuffer[I] = I;
  }

  std::cout << "Starting " << n << " threads" << std::endl;

  std::vector<std::thread> threads(n);
  uint chunkSize = NumElements / n;

  // Start `n` threads, each will process a fixed size chunk of the input
  for (uint32_t i = 0; i < n; i++)
  {
    auto start = i * chunkSize;
    auto end = start + chunkSize;
    if (end > NumElements)
    {
      end = NumElements;
    }

    threads[i] = std::thread(doSomething, start, end);
  }

  for (auto &th : threads)
  {
    th.join();
  }

  // Just print the last X values!
  auto X = 5; 
  for (uint32_t I = NumElements - X; I < NumElements; ++I)
  {
    std::cout << InBuffer[I] << "(" << OutBuffer[I] << ")\n";
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}