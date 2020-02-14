# Ajisai
A real-time rendering demo based on Vulkan. A sample video can be found [here](https://youtu.be/Do81PN6Fack).

## Required Libraries
To compile this demo, the following third party libraries are required
* [GLFW-3.2.1](https://github.com/glfw/glfw)
* [assimp-4.0.1](https://github.com/assimp/assimp)
* [GLM](https://github.com/g-truc/glm)
* [stb image loader](https://github.com/nothings/stb)

## Implemented rendering algorithm
* Real-time GGX shading ([reference](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)).
* Real-time polygonal-light shading with linearly transformed cosines ([reference](https://eheitzresearch.wordpress.com/415-2/)).
* This vulkan renderer supports deferred shading, shadow mapping and normal mapping.
