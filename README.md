# BohdiAR

[![CI Status](http://img.shields.io/travis/sunpaq/BohdiAR.svg?style=flat)](https://travis-ci.org/sunpaq/BohdiAR)
[![Version](https://img.shields.io/cocoapods/v/BohdiAR.svg?style=flat)](http://cocoapods.org/pods/BohdiAR)
[![License](https://img.shields.io/cocoapods/l/BohdiAR.svg?style=flat)](http://cocoapods.org/pods/BohdiAR)
[![Platform](https://img.shields.io/cocoapods/p/BohdiAR.svg?style=flat)](http://cocoapods.org/pods/BohdiAR)

## Requirements

BohdiAR depends on pod [OpenCV-Dynamic](https://cocoapods.org/pods/OpenCV-Dynamic)
which depends on cmake.

you can install cmake use [homebrew](https://brew.sh/)

```ruby
brew install cmake
```

## Installation

this pod is not published into cocoapods trunk right now !
please use it like below

```
target 'App' do
    pod 'BohdiAR', :git => 'https://github.com/sunpaq/BohdiAR.git', :branch => 'develop'
end
```

## Author

sunpaq@gmail.com

## License

BohdiAR is available under the BSD license. See the LICENSE file for more info.
