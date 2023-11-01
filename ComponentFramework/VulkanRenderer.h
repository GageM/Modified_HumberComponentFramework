#pragma once
#include "Renderer.h"
#include <SDL_vulkan.h>
#include <SDL_image.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <unordered_map>
#include <array>
#include <chrono>

#include "Vector.h"
#include "VMath.h"
#include "MMath.h"
#include "Hash.h"
using namespace MATH;

#define MAX_LIGHTS 5

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


// Use Validation Layers In Debug Mode
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capibilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
	Vec3 pos;
	Vec3 normal;
	Vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 1;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, normal);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 2;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	// Check if the Vertices are EXACTLY Identical
	bool operator == (const Vertex& other) const {
		return ((pos == other.pos) && (normal == other.normal) && (texCoord == other.texCoord));
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const noexcept {
			size_t hash1 = hash<Vec3>()(vertex.pos);
			size_t hash2 = hash<Vec3>()(vertex.normal);
			size_t hash3 = hash<Vec2>()(vertex.texCoord);
			size_t result = ((hash1 ^ (hash2 << 1)) >> 1) ^ (hash3 << 1);
			return result;
		}

	};
}

struct UniformBufferObject {
	Matrix4 model;
	Matrix4 view;
	Matrix4 proj;
};

struct LightUBO {
	Vec3 position[MAX_LIGHTS];
	Vec4 color[MAX_LIGHTS];
	float intensity[MAX_LIGHTS];
	Vec3 falloff[MAX_LIGHTS];
};

struct Buffer {
	VkBuffer bufferID;
	VkDeviceMemory memoryID;
};

class VulkanRenderer : public Renderer
{
public:
	VulkanRenderer(const VulkanRenderer&) = delete;
	VulkanRenderer(VulkanRenderer&&) = delete;
	VulkanRenderer& operator=(const VulkanRenderer&) = delete;
	VulkanRenderer& operator=(VulkanRenderer&&) = delete;

	VulkanRenderer();
	~VulkanRenderer();
	SDL_Window* CreateWindow(std::string name_, int width_, int height_);
	bool OnCreate();
	void  OnDestroy();
	void Render();

	void SetUBO(
		const Matrix4& projection, const Matrix4& view,
		const Matrix4& model
	);

	void SetLightUBO(
	) {}

	SDL_Window* GetWindow() { return window; }

private:
	const size_t MAX_FRAMES_IN_FLIGHT = 2;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	SDL_Event sdlEvent;
	uint32_t windowWidth;
	uint32_t windowHeight;
	SDL_Window* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkRenderPass renderPass;
	VkDescriptorSetLayout desscriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;

	VkCommandPool commandPool;

	Buffer vertex;
	Buffer index;

	std::vector<Buffer> uniformBuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	bool frameBufferResized = false;

	bool hasStencilComponent(VkFormat format);

	void initVulkan();
	void createInstance();
	void createSurface();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void recreateSwapChain();
	void updateUniformBuffer(uint32_t currentImage);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void createRenderPass();
	void createDescriptorSetLayout();

	void createFrameBuffers();
	void createCommandPool();
	void createDepthResources();

	void createTextureImageView();
	void createTextureSampler();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createVertexBuffer();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptoPool();
	void createDescriptorSets();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createCommandBuffers();

	void createSyncObjects();
	void cleanup();
	void cleanupSwapChain();
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkFormat findDepthFormat();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	// For Debugging Vulkan

	VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	void destroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	void populateDebugMessengerCreateInfor(VkDebugUtilsMessengerCreateInfoEXT& creasteInfo);

	void setupDebugMessenger();
	
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	// End For Debugging Vulkan

	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;


	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	// Swap Chain

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFrameBuffers;

	// End Swap Chain


	UniformBufferObject ubo;

	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	static std::vector<char> readFile(const std::string& filename);
};

