# BohdiEngine

## Example

To run the example project, clone the repo, and run `pod install` from the Example directory first.

## Requirements

iOS devices support OpenGLES 3.0 with arm64 CPU (>=iPhone5s)

## Installation

BohdiEngine is available through [CocoaPods](http://cocoapods.org). To install
it, simply add the following line to your Podfile:

```ruby
pod "BohdiEngine"
```

If you need to use the developing version:

```
target 'App' do
    pod 'BohdiEngine', :git => 'https://github.com/sunpaq/BohdiEngine-pod.git', :branch => 'develop'
end
```

## Build your App use BohdiEngine

Please notice that you should have default shaders and sample model in your
App's Resource folder. please just copy the following files:

    2.obj
    beengine.mtl
    MCGLRenderer.fsh
    MCGLRenderer.vsh
    MCSkyboxShader.fsh
    MCSkyboxShader.vsh

    from
    <PodRoot>/BohdiEngine/Assets/models
    <PodRoot>/BohdiEngine/Assets/shaders

for OpenGL setup and Engine usage, please check the Example of this Pod (Objective-C)
there also have a [demo written use swift](https://github.com/sunpaq/BohdiEngineDemoSwift)

## Author

Sun YuLi, sunpaq@gmail.com

## License

BohdiEngine is available under the BSD license. See the LICENSE file for more info.
