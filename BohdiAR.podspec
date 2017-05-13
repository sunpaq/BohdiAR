#
# Be sure to run `pod lib lint BohdiAR.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'BohdiAR'
  s.version          = '0.1.3'
  s.summary          = 'wrap OpenCV-Dynamic framework and ArUco library for AR apps'

  s.homepage         = 'https://github.com/sunpaq/BohdiAR-pod'
  s.license          = { :type => 'BSD', :file => 'LICENSE' }
  s.author           = { 'sunpaq' => 'sunpaq@gmail.com' }
  s.source           = { :git => 'https://github.com/sunpaq/BohdiAR-pod.git', :tag => s.version.to_s }

  s.ios.deployment_target = '8.0'
  s.source_files = 'BohdiAR/Classes/**/*.{h,hpp,cpp,mm}'
  s.private_header_files = 'BohdiAR/Classes/ArUco2/**/*.{h,hpp}', 'BohdiAR/Classes/Core/**/*.{h,hpp}'

  s.frameworks = 'Foundation', 'AVFoundation', 'UIKit', 'AssetsLibrary', 'CoreMedia'
  s.dependency 'OpenCV-Dynamic'

end
