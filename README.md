# stylized-animation

link to the animation tutorial using Assimp : https://www.youtube.com/watch?v=r6Yv_mh79PI

## How to run the project

> cmake -B build
> cd build
> make
> cd ..
> ./stylize-animation

## Commands of UI 

- F1 : toggle camera moving with mouse
- F2 : reload shaders
- F3 : toggle animation


## Explaination on the project for developpers

### The render passes

We have 2 render passes :
- hybrid render pass which will render all the mesh objects into a framebuffer which is composed of 3 textures. the first for rendering the objects as usual (which contains vec4), a second for metadata which contains uvec4, and the third for normals and z value (before projection). The last one is used later for contour detection. More textures can be added if needed. Note that having several output texture in a frambuffer is called Multiple Render Targets (MRT).
- final render pass which is actually a post processing render pass. Only a special mesh (a squad) is rendered is this pass. It takes as a texture ("sceneTexture" unifrom sample2D in fragment shaders) the first texture of the framebuffer from the hybrid render pass and display it on the squad which will be drawn on the screen. It will take more textures in the future.

Each object has its own shaders for the hybrid render pass.
There is only one set of shaders for the final render pass wich is an attribute of the scene (finalRenderPassShaderHandle attribute).

### Shaders for hybrid render pass
The vertex shader will receive the vertices of the objects of the scene. animated_mesh.vert is an example.

The fragment shader needs to write in both textures. Therefore you must declare two output variables : 

```
layout(location = 0) out vec4 FragColor0; //scene
layout(location = 1) out uvec4 FragColor1; //metadata
layout(location = 2) out uvec4 FragColor2; //normals 
```

The r and b component of FragColor1 is reserved by meshId and contour detection. 

Please look at hybrid.frag to find an example of this kind of fragment shader and to correctly fill up FragColor1.

### Shaders for final render pass
The vertex shader takes 2 triangles whose xy component corresponds directly to their position in the screen. It must output a vec2 texCoord, whose components range from 0 to 1. Look at final.vert for an example.

The fragment shader will receive a vec2 texCoord and the first texture of the hybrid render pass' framebuffer will be bound to `uniform sampler2D sceneTexture`. See final.frag for an example.