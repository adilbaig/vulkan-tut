// A Simple Vulkan Compute Example in C++
// https://bakedbits.dev/posts/vulkan-compute-example/

// Example modified to use GLSL and experiment with custom workgroup sizes.

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <fstream>
#include <string>

const uint32_t NumElements = 300 * 1000000; 
const std::string shaderFile("/home/adil/workspace/vulkan-tut/CustomCompute/build/bin/compute.spv");

static std::vector<char> readFile(const std::string &filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
  {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

int main()
{
  vk::ApplicationInfo AppInfo{"VulkanCustomCompute", 1, nullptr, 0, VK_API_VERSION_1_2};
  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

  vk::InstanceCreateInfo InstanceCreateInfo(vk::InstanceCreateFlags(), &AppInfo, validationLayers.size(), validationLayers.data());
  vk::Instance Instance = vk::createInstance(InstanceCreateInfo);

  // We're just taking the first device we find (`front()`)
  auto pDevice = Instance.enumeratePhysicalDevices().front();
  auto pDeviceProps = pDevice.getProperties();

  std::cout << "Device Name: " << pDeviceProps.deviceName << std::endl;

  const auto ApiVersion = pDeviceProps.apiVersion;
  std::cout << "Vulkan Version : " << VK_VERSION_MAJOR(ApiVersion) << "." << VK_VERSION_MINOR(ApiVersion) << "." << VK_VERSION_PATCH(ApiVersion) << std::endl;
  auto DeviceLimits = pDeviceProps.limits;
  std::cout << "Max Compute Shared Memory Size: " << DeviceLimits.maxComputeSharedMemorySize / 1024 << " KB" << std::endl;
  std::cout << "Max WorkGroup Size: " << DeviceLimits.maxComputeWorkGroupSize[0] << " " << DeviceLimits.maxComputeWorkGroupSize[1] << " " << DeviceLimits.maxComputeWorkGroupSize[2] << std::endl;

  // Now look for a Queue that supports `vk::QueueFlagBits::eCompute` (VK_QUEUE_COMPUTE_BIT) ...
  std::vector<vk::QueueFamilyProperties> QueueFamilyProps = pDevice.getQueueFamilyProperties();
  auto PropIt = std::find_if(QueueFamilyProps.begin(), QueueFamilyProps.end(), [](const vk::QueueFamilyProperties &Prop)
                             { return Prop.queueFlags & vk::QueueFlagBits::eCompute; });

  // .. Found it!
  const uint32_t ComputeQueueFamilyIndex = std::distance(QueueFamilyProps.begin(), PropIt);
  std::cout << "Compute Queue Family Index: " << ComputeQueueFamilyIndex << std::endl;

  // Let's create a device
  const auto noOfQueues = 1;
  float queuePriorities[noOfQueues] = {1.0};
  vk::DeviceQueueCreateInfo DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), ComputeQueueFamilyIndex, noOfQueues, &queuePriorities[0]);
  vk::DeviceCreateInfo DeviceCreateInfo(vk::DeviceCreateFlags(), DeviceQueueCreateInfo);
  vk::Device Device = pDevice.createDevice(DeviceCreateInfo);

  // Create Buffers (which is a view into memory, not actually storing data)
  const uint32_t BufferSize = sizeof(int32_t) * NumElements;

  vk::BufferCreateInfo BufferCreateInfo{
      vk::BufferCreateFlags(),
      BufferSize,
      vk::BufferUsageFlagBits::eStorageBuffer, // Usage
      vk::SharingMode::eExclusive,             // Sharing mode
      1,                                       // Number of queue family indices
      &ComputeQueueFamilyIndex                 // List of Queue family indices
  };
  vk::Buffer InBuffer = Device.createBuffer(BufferCreateInfo);
  vk::Buffer OutBuffer = Device.createBuffer(BufferCreateInfo);

  vk::PhysicalDeviceMemoryProperties MemoryProperties = pDevice.getMemoryProperties();
  uint32_t MemoryTypeIndex = uint32_t(~0);
  vk::DeviceSize MemoryHeapSize = uint32_t(~0);

  for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; ++i)
  {
    vk::MemoryType MemoryType = MemoryProperties.memoryTypes[i];
    if ((vk::MemoryPropertyFlagBits::eHostVisible & MemoryType.propertyFlags) &&
        (vk::MemoryPropertyFlagBits::eHostCoherent & MemoryType.propertyFlags))
    {
      MemoryHeapSize = MemoryProperties.memoryHeaps[MemoryType.heapIndex].size;
      MemoryTypeIndex = i;
      break;
    }
  }

  std::cout << "Memory Type Index: " << MemoryTypeIndex << std::endl;
  std::cout << "Memory Heap Size : " << MemoryHeapSize / 1024 / 1024 / 1024 << " GB" << std::endl;

  vk::MemoryRequirements InBufferMemoryRequirements = Device.getBufferMemoryRequirements(InBuffer);
  vk::MemoryRequirements OutBufferMemoryRequirements = Device.getBufferMemoryRequirements(OutBuffer);

  vk::MemoryAllocateInfo InBufferMemoryAllocateInfo(InBufferMemoryRequirements.size, MemoryTypeIndex);
  vk::MemoryAllocateInfo OutBufferMemoryAllocateInfo(OutBufferMemoryRequirements.size, MemoryTypeIndex);
  vk::DeviceMemory InBufferMemory = Device.allocateMemory(InBufferMemoryAllocateInfo);
  vk::DeviceMemory OutBufferMemory = Device.allocateMemory(InBufferMemoryAllocateInfo);

  int32_t *InBufferPtr = static_cast<int32_t *>(Device.mapMemory(InBufferMemory, 0, BufferSize));
  for (int32_t I = 0; I < NumElements; ++I)
  {
    InBufferPtr[I] = I;
  }
  Device.unmapMemory(InBufferMemory);

  // Binding the buffer to memory is what lets the GPU RW into the buffers.
  Device.bindBufferMemory(InBuffer, InBufferMemory, 0);
  Device.bindBufferMemory(OutBuffer, OutBufferMemory, 0);

  // Now read the compute shader
  std::vector<char> ShaderContents = readFile(shaderFile);
  vk::ShaderModuleCreateInfo ShaderModuleCreateInfo(
      vk::ShaderModuleCreateFlags(),                              // Flags
      ShaderContents.size(),                                      // Code size
      reinterpret_cast<const uint32_t *>(ShaderContents.data())); // Code
  vk::ShaderModule ShaderModule = Device.createShaderModule(ShaderModuleCreateInfo);

  // Set up Descripter sets
  const std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBinding = {
      {0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute},
      {1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute}};
  vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(
      vk::DescriptorSetLayoutCreateFlags(),
      DescriptorSetLayoutBinding);
  vk::DescriptorSetLayout DescriptorSetLayout = Device.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo);

  // Descripter sets are used to create a Pipeline Layout (not a Pipeline!)
  vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), DescriptorSetLayout);
  vk::PipelineLayout PipelineLayout = Device.createPipelineLayout(PipelineLayoutCreateInfo);
  vk::PipelineCache PipelineCache = Device.createPipelineCache(vk::PipelineCacheCreateInfo());

  vk::PipelineShaderStageCreateInfo PipelineShaderCreateInfo(
      vk::PipelineShaderStageCreateFlags(), // Flags
      vk::ShaderStageFlagBits::eCompute,    // Stage
      ShaderModule,                         // Shader Module
      "main");                              // Shader Entry Point
  vk::ComputePipelineCreateInfo ComputePipelineCreateInfo(
      vk::PipelineCreateFlags(), // Flags
      PipelineShaderCreateInfo,  // Shader Create Info struct
      PipelineLayout);           // Pipeline Layout
  vk::Pipeline ComputePipeline = Device.createComputePipeline(PipelineCache, ComputePipelineCreateInfo);

  // Back to creating Descriptor Sets
  vk::DescriptorPoolSize DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 2);
  vk::DescriptorPoolCreateInfo DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1, DescriptorPoolSize);
  vk::DescriptorPool DescriptorPool = Device.createDescriptorPool(DescriptorPoolCreateInfo);

  vk::DescriptorSetAllocateInfo DescriptorSetAllocInfo(DescriptorPool, 1, &DescriptorSetLayout);
  const std::vector<vk::DescriptorSet> DescriptorSets = Device.allocateDescriptorSets(DescriptorSetAllocInfo);
  vk::DescriptorSet DescriptorSet = DescriptorSets.front();
  vk::DescriptorBufferInfo InBufferInfo(InBuffer, 0, NumElements * sizeof(int32_t));
  vk::DescriptorBufferInfo OutBufferInfo(OutBuffer, 0, NumElements * sizeof(int32_t));

  const std::vector<vk::WriteDescriptorSet> WriteDescriptorSets = {
      {DescriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &InBufferInfo},
      {DescriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &OutBufferInfo},
  };
  Device.updateDescriptorSets(WriteDescriptorSets, {});

  // Let's start submitting work
  vk::CommandPoolCreateInfo CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), ComputeQueueFamilyIndex);
  vk::CommandPool CommandPool = Device.createCommandPool(CommandPoolCreateInfo);
  vk::CommandBufferAllocateInfo CommandBufferAllocInfo(
      CommandPool,                      // Command Pool
      vk::CommandBufferLevel::ePrimary, // Level
      1);                               // Num Command Buffers
  const std::vector<vk::CommandBuffer> CmdBuffers = Device.allocateCommandBuffers(CommandBufferAllocInfo);
  vk::CommandBuffer CmdBuffer = CmdBuffers.front();

  // Record commands
  vk::CommandBufferBeginInfo CmdBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  CmdBuffer.begin(CmdBufferBeginInfo);
  CmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, ComputePipeline);
  CmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, // Bind point
                               PipelineLayout,                  // Pipeline Layout
                               0,                               // First descriptor set
                               {DescriptorSet},                 // List of descriptor sets
                               {});                             // Dynamic offsets
  
  uint dispatchSize = (NumElements/1024) + 1; // layout (local_size_x = 512) in;  in the shader file
  std::cout << "Dispatch Size: " << dispatchSize << std::endl;
  CmdBuffer.dispatch(1024, 1, 1);
  CmdBuffer.end();

  // Submit commands to the queue, setup a fence, and wait for the fenc to complete
  vk::Queue Queue = Device.getQueue(ComputeQueueFamilyIndex, 0);
  vk::Fence Fence = Device.createFence(vk::FenceCreateInfo());

  vk::SubmitInfo SubmitInfo(0,           // Num Wait Semaphores
                            nullptr,     // Wait Semaphores
                            nullptr,     // Pipeline Stage Flags
                            1,           // Num Command Buffers
                            &CmdBuffer); // List of command buffers
  Queue.submit({SubmitInfo}, Fence);
  Device.waitForFences({Fence},       // List of fences
                       true,          // Wait All
                       uint64_t(-1)); // Timeout

  // Print out the results!
  InBufferPtr = static_cast<int32_t *>(Device.mapMemory(InBufferMemory, 0, BufferSize));
  int32_t *OutBufferPtr = static_cast<int32_t *>(Device.mapMemory(OutBufferMemory, 0, BufferSize));
  // for (uint32_t I = 0; I < NumElements; ++I)
  // {
  //   std::cout << InBufferPtr[I] << "(" << OutBufferPtr[I] << ")\n";
  // }
  // std::cout << std::endl;
  Device.unmapMemory(InBufferMemory);
  Device.unmapMemory(OutBufferMemory);

  // Clean up!
  Device.resetCommandPool(CommandPool, vk::CommandPoolResetFlags());
  Device.destroyFence(Fence);
  Device.destroyDescriptorSetLayout(DescriptorSetLayout);
  Device.destroyPipelineLayout(PipelineLayout);
  Device.destroyPipelineCache(PipelineCache);
  Device.destroyShaderModule(ShaderModule);
  Device.destroyPipeline(ComputePipeline);
  Device.destroyDescriptorPool(DescriptorPool);
  Device.destroyCommandPool(CommandPool);
  Device.freeMemory(InBufferMemory);
  Device.freeMemory(OutBufferMemory);
  Device.destroyBuffer(InBuffer);
  Device.destroyBuffer(OutBuffer);
  Device.destroy();
  Instance.destroy();

  std::cout << "Compute Complete." << std::endl;

  return EXIT_SUCCESS;
}
