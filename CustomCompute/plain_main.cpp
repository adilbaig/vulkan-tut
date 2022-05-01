#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

const auto n =  std::thread::hardware_concurrency();
const uint32_t NumElements = 300 * 1000000;
std::vector<uint32_t> InBuffer(NumElements);
std::vector<uint32_t> OutBuffer(NumElements);

void doSomething(uint32_t I, uint32_t end)
{
  for (uint32_t I = 0; I < end; ++I)
  {
    OutBuffer[I] = InBuffer[I] * 2;
  }
}

int main()
{
  for (int32_t I = 0; I < NumElements; ++I)
  {
    InBuffer[I] = I;
  }

  std::cout << "Starting " << n << " threads" << std::endl;
  
  std::vector<std::thread> threads(n);
  uint chunkSize = NumElements/n;

  for (int i = 0; i < n; i++)
  {
    auto start = i * chunkSize;
    auto end = start + chunkSize;
    if (end > NumElements) {
      end = NumElements;
    }

    threads[i] = std::thread(doSomething, start, end);
  }

  for (auto &th : threads)
  {
    th.join();
  }

  // Sssh!
  for (uint32_t I = NumElements - 100; I < NumElements; ++I)
  {
    std::cout << InBuffer[I] << "(" << OutBuffer[I] << ")\n";
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}