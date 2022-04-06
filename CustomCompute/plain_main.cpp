#include <iostream>
#include <fstream>
#include <vector>

const uint32_t NumElements = 300 * 1000000;

int main()
{
  std::vector<uint32_t> InBuffer(NumElements);
  std::vector<uint32_t> OutBuffer(NumElements);
  for (int32_t I = 0; I < NumElements; ++I)
  {
    InBuffer[I] = I;
  }

  for (uint32_t I = 0; I < NumElements; ++I)
  {
    OutBuffer[I] = InBuffer[I] * 2;
  }

  // Sssh!
  // for (uint32_t I = 0; I < NumElements; ++I)
  // {
  //   std::cout << InBuffer[I] << "(" << OutBuffer[I] << ")\n";
  // }
  // std::cout << std::endl;

  return EXIT_SUCCESS;
}