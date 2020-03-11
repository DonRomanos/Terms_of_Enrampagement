[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

# Terms of Enrampagement (Working Title)

Since I like games, why not make one. The main thing I want to achieve with this is to enjoy coding, discovering and solving some Problems. Experiment with Network & Performance Programming.

## What kind of game should I make

I actually started my first programming steps with a game called "Warcraft 3" it came with a map editor and a very strange (or cumbersome) programming language called *JASS*.

Back then I made a game called *Archer Wars* which was a simple Battle Arena kind of game. You control one unit which has a set of abilities and your goal is to get points by killing other players.

There were some items and interactions with the terrain possible which made the game more interesting.

So why not try to revive this game? But this time lets try to code it from scratch!

These are the things I want to have a look at:

* Rendering - Lets write a basic engine to get a feel for graphics programming
* Network Coding - Can we have a server that handles a loooot of clients, how many units is the limit, how can I handle synchronization errors
* Some fun stuff, like cool abilities and a lot of projectiles

## Architecture

Right now, not much there. My idea is to see the game as a deterministic simulation that has to be synchronized on all clients while the server is the master. For this I will try to implement one game state that is always updated for all the players.

Each player action will be a command, which will be sent to the server, which in turn distributes it to each other player. The command will have a timestamp and the clients can then take into account at what time the command was issued and what happened in the meantime.

Thats the basic Idea, lets see what we can do.

Problems that are likely to be encountered

* Out of sync detection / handling
* A uniform clock across all connected instances (clients and server)
* Floating Point precision errors accumulating and making the game non deterministic

## Rendering

I experimented a bit with Vulkan for this project and tried to gain an understanding of its concepts. This is not meant to be a tutorial but rather an explanation of how I understand that Vulkan works.

<details><summary>Click here if you are interested in my experience with graphics programming</summary>
<p>

### Vulkan Experience

So you have decided to use Vulkan eh? Think twice, I can see only three kinds of people who should bother with it:

* Masochists
* People who **really** have a need for performance (and get paid for it)
* People who enjoy learning and have masochistic traits

Unfortunately I seem to be of the last kind, therefore this section. Be aware it took me roughly 1000 lines to output a triangle on the screen. 

In Vulkan you have control over practiacally everything, which is good. However it also means you have to explicitly set everything up, Vulkan does not implicitly use any defaults. I mostly followed this excellent [tutorial](https://vulkan-tutorial.com/) until I had something on the screen. Then I went and restructured the code, all while trying to understand better how Vulkan works and whats the rationale behind its design.

#### Basic Concepts

As Vulkan is a C-API it does not by default work with any C++ features like containers. Vulkan follows a pattern when returning a list, such functions take a counter and a data variable by reference.

* First call the function with a nullptr and an empty counter -> it will set the counter to the appropriate value
* Then call the same Function again with the counter and a pointer to allocated memory -> the memory will be filled with appropriate structs

Example:

```cpp
// Get the count of physical devices
uint32_t deviceCount = 0;
vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

// Allocate required memory using vector constructor (requires C++11) and fill the data
std::vector<VkPhysicalDevice> devices{ deviceCount };
vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
```

Most Objects you create in Vulkan require quite a bit of parameters, these parameters are passed in structs of the form *Vk...CreateInfo*

```cpp
VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // used for versioning inside of vulkan
        .queueFamilyIndex = selected_queue_family.index,
        .queueCount = 1
    };
```

Any resource you acquire in Vulkan has to be released through an appropriate call to vk..Destroy if not otherwise specified

```cpp
vkDestroyInstance(instance, nullptr); // 2nd parameter is an allocation function which can be used for memory logging etc.
```

#### Validation Layers

Validation Layers are Vulkans way to check for errors and debug. Since Vulkan is a high performance and low level API error checking is kept to an absolute minimum. To help debugging it is recommended to activate at least some basic layers in debug with **VK_LAYER_LUNARG_standard_validation**. Layers are activated by the loader, either through passing them into the instance or via some [registry /environment variables](https://vulkan.lunarg.com/doc/view/1.0.13.0/windows/layers.html).

#### Extensions

Extensions are specific operations that the driver can, but does not have to support. The most common one you will use is the windowing system *VK_USE_PLATFORM_XXX_KHR* where XXX is the platform name, e.g. *WINDOWS, LINUX, ANDROID...*. Luckily **Glfw** helps us here and abstracts all the platform specific code away from us and delivers us with a list of extensions that are required for rendering. The abstraction of the windowing system is based on `VkSurfaceKHR` which is part of an extension, as can be seen in the ending KHR.

#### Vulkan Device

The most central part of your Vulkan application is a `VkDevice` this is something comparable to an OpenGl context, it is used to keep track of resources for example.
This is not the actual physical GPU but an abstraction above. However it does require you to specify which GPU you want to use. Here you should also check if all your required features, extensions, validation layers are supported by your physical device.

The setup will look something along these lines

```cpp
if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS) 
{
    throw std::runtime_error("Failed to create logical device!");
}
```

The `device_create_info` contains all required information as usual. Important is that you make sure the GPU you selected actually supports all of your required operations.

To setup the device, you roughly need the following steps:

* Create a Vulkan Instance (here you can give names and versions to your engine, e.g. for driver optimizations and specify the Vulkan version)
* Select a physical device (make sure the device supports what you want, e.g. extensions for presenting and graphics)
* Select which queue families you need (e.g. for graphics and present support [for this you need to know the surface you render to])
* Now you can finally create the device

As a last step you should also acquire handles to the queues you are using in order to interact with them later.

**Note** Vulkan divides the queues into families with properties. A queue family can execute one or more operations listed in their `properties` member. The family will also tell you how many queues of each family you can create. The queues are created together with the `VkDevice` and its handles acquired through `vkGetDeviceQueue`. Queues will be released along with the `VkDevice`. I could not find any information when it is benefitial to create more than one queue of any family. However you should always use the most specialized queue for your operations.

#### Swapchain

After having a Device and Queues we can now proceed to create the SwapChain. A Swapchain has 3 important properties:

* SurfaceFormat, this specifies the color space of the images, e.g. VK_FORMAT_B8G8R8A8_SRGB
* PresentMode, specifies how images will be presented to screen e.g. fifo, immediatly...
* Extent, this is just the size of the images

In order to set those values you have to query whats available for your device and then pick the suitable setting for you.

#### References

* [vulkan-tutorial.com](https://vulkan-tutorial.com/)
* [vulkan.lunarg.com](https://vulkan.lunarg.com/doc/view/1.0.57.0/windows/tutorial/html)
* [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/) - Reference and SDK
* [vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)

</p>
</details>

## Required Libraries and Tools

* CMake
* Conan
* GTest
* Google Microbenchmark
* GLFW
* Vulkan

Install the Vulkan SDK and make sure you have an environment variable VULKAN_SDK (should automatically be done when installing the SDK, maybe restart your IDE / shell)

The rest should be taken care of by Conan and CMake :) (Conan Package for the SDK is currently in progress, once its available I will use it)

**Note**: By default this uses C++20 because I want to play around with the newest things.

## Running this in WSL2

I tried running the application in WSL2, but could not get it to work because of this error:
*"Error: GLX: Failed to load GLX"*

Here is what I did:

* Install OpenGL dependencies to enable linking (some opengl libraries were missing e.g. xcursor, xrender,...)
* Install vcXsrv on windows to have something to display something (works with glxgears)
* Run vcXsrv with *Multiple Windows*, *No Client*, and **without** *Native OpenGL*
* Set Display: export DISPLAY=:0
* Run the application ...

This always resulted in the above error, I tried googling and adjusting paths and installing some packages but nothing worked. Maybe it is due to vcXsrv but I did no try anything else yet, will let it rest for now.

## Building

Requirements:

* CMake
* Conan
* Python (for Conan)

To build simply run those commands. ( If you want to use specific compiler settings I recommend setting up and using a conan profile, e.g. conan install .. -p gcc-9 ..)

```shell
mkdir build && cd build
conan install .. --build missing
conan build ..
```

Now you should see the library successfully compiling and running the tests.
