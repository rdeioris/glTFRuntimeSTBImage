# glTFRuntimeSTBImage
Example Extension for glTFRuntime using stb_image.h as the textures/images loader

The Unreal Engine glTFRuntime plugin has the ability to be extended by external modules/plugins in various ways.

This is an example implementing the FglTFRuntimeImageLoader API allowing the Textures/Images loader to parse more formats than the default ones using the famous stb_image.h header-only library.

You can add your loaders to the glTFRuntimeImagesConfig structure:

![glTFRuntimeImageLoaderSTBImage](Screenshots.PNG?raw=true "glTFRuntimeImageLoaderSTBImage")

The gltf file in the example is as simple as

```json
{
        "images":
        [
                {
                        "uri": "mannequin.gif"
                }
        ]
}

```
