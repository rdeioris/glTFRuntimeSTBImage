# glTFRuntimeSTBImage
Example Extension for glTFRuntime using stb_image.h as the textures/images loader

The Unreal Engine glTFRuntime plugin has the ability to be extended by external modules/plugins in various ways.

This is an example implementing the OnTexturePixels API allowing the Textures/Images loader to parse more formats than the default ones using the famous stb_image.h header-only library.

Just enable the plugin to activate the new loader (the 'default one' based on the Unreal ImageWrapper module will be disabled).
