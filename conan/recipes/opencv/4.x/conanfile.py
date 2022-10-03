from conan.tools.microsoft import msvc_runtime_flag
from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os
import shutil

required_conan_version = ">=1.43.0"


class OpenCVConan(ConanFile):
    name = "opencv"
    license = "Apache-2.0"
    homepage = "https://opencv.org"
    description = "OpenCV (Open Source Computer Vision Library)"
    url = "https://github.com/conan-io/conan-center-index"
    topics = ("computer-vision", "deep-learning", "image-processing")

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    short_paths = True
    generators = "cmake", "cmake_find_package"
    _cmake = None

    @property
    def _source_subfolder(self):
        return "source_subfolder"

    @property
    def _build_subfolder(self):
        return "build_subfolder"

    @property
    def _contrib_folder(self):
        return "contrib"

    @property
    def _is_msvc(self):
        return str(self.settings.compiler) in ["Visual Studio", "msvc"]

    @property
    def _has_with_jpeg2000_option(self):
        return self.settings.os != "iOS"

    @property
    def _has_with_tiff_option(self):
        return self.settings.os != "iOS"

    @property
    def _has_with_ffmpeg_option(self):
        return False

    def export_sources(self):
        self.copy("CMakeLists.txt")
        for patch in self.conan_data.get("patches", {}).get(self.version, []):
            self.copy(patch["patch_file"])

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if self.settings.os != "Linux":
            del self.options.with_gtk
            del self.options.with_v4l
        if self.settings.os == "Macos":
            del self.settings.arch

        if self._has_with_ffmpeg_option:
            # Following the packager choice, ffmpeg is enabled by default when
            # supported, except on Android. See
            # https://github.com/opencv/opencv/blob/39c3334147ec02761b117f180c9c4518be18d1fa/CMakeLists.txt#L266-L268
            self.options.with_ffmpeg = self.settings.os != "Android"
        else:
            #del self.options.with_ffmpeg
            pass

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def requirements(self):
        pass

    def validate(self):
        if self.options.shared and self._is_msvc and "MT" in msvc_runtime_flag(self):
            raise ConanInvalidConfiguration("Visual Studio with static runtime is not supported for shared library.")
        if self.settings.compiler == "clang" and tools.Version(self.settings.compiler.version) < "4":
            raise ConanInvalidConfiguration("Clang 3.x can't build OpenCV 4.x due to an internal bug.")

    def build_requirements(self):
        pass

    def source(self):
        tools.get(**self.conan_data["sources"][self.version][0],
                  destination=self._source_subfolder, strip_root=True)

        tools.get(**self.conan_data["sources"][self.version][1],
                  destination=self._contrib_folder, strip_root=True)

        for patch in self.conan_data.get("patches", {}).get(self.version, []):
            tools.patch(**patch)

    def _patch_opencv(self):
        pass

    def _configure_cmake(self, arch = None):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.definitions["OPENCV_CONFIG_INSTALL_PATH"] = "cmake"
        self._cmake.definitions["OPENCV_BIN_INSTALL_PATH"] = "bin"
        self._cmake.definitions["OPENCV_LIB_INSTALL_PATH"] = "lib"
        self._cmake.definitions["OPENCV_3P_LIB_INSTALL_PATH"] = "lib"
        self._cmake.definitions["OPENCV_OTHER_INSTALL_PATH"] = "res"
        self._cmake.definitions["OPENCV_LICENSES_INSTALL_PATH"] = "licenses"

        self._cmake.definitions["WITH_FFMPEG"] = False
        self._cmake.definitions["WITH_TIFF"] = False
        self._cmake.definitions["BUILD_SHARED_LIBS"] = False
        self._cmake.definitions["BUILD_EXAMPLES"] = False
        self._cmake.definitions["BUILD_TESTS"] = False
        self._cmake.definitions["BUILD_PERF_TESTS"] = False
        self._cmake.definitions["BUILD_opencv_python2"] = False
        self._cmake.definitions["BUILD_opencv_python3"] = False
        self._cmake.definitions["OPENCV_GENERATE_PKGCONFIG"] = False
        self._cmake.definitions["BUILD_OPENEXR"] = True
        self._cmake.definitions["OPENCV_IO_FORCE_OPENEXR"] = True
        self._cmake.definitions["WITH_GSTREAMER"] = False
        if (self.settings.os == "Android"):
            self._cmake.definitions["BUILD_JAVA"] = False
            self._cmake.definitions["BUILD_ANDROID_EXAMPLES"] = False
            self._cmake.definitions["ANDROID_NATIVE_API_LEVE"] = 24
            self._cmake.definitions["ANDROID_PLATFORM"] = "android-24"
            self._cmake.definitions["WITH_OPENCL"] = True
        elif (self.settings.os == "Windows"):
            self._cmake.definitions["BUILD_WITH_STATIC_CRT"] = False
        
        if self.settings.os == "Macos":
            self._cmake.definitions["CMAKE_OSX_ARCHITECTURES"] = arch

        self._cmake.configure(build_folder=self._build_subfolder)
        return self._cmake

    def build(self):
        self._patch_opencv()
        if self.settings.os == "Macos":
            cmake = self._configure_cmake("arm64")
            cmake.build()
            cmake.install()
            shutil.copytree(os.path.join(self.package_folder, "lib"), os.path.join(self.package_folder, "lib", "arm64"))
            os.system("cmake --build %s --target clean" % (cmake.build_folder))
            os.remove(os.path.join(cmake.build_folder, "CMakeCache.txt"))
            shutil.rmtree(os.path.join(cmake.build_folder, "CMakeFiles"))

            del self._cmake
            cmake = self._configure_cmake("x86_64")
            cmake.build()
            cmake.install()

            base_dir = os.path.join(self.package_folder, "lib")
            for lib in os.listdir(base_dir):
                if lib.endswith(".a"):
                    print("lipo -create %s/arm64/%s %s/%s -o %s/%s" % (base_dir, lib, base_dir, lib, base_dir, lib))
                    os.system("lipo -create %s/arm64/%s %s/%s -o %s/%s" % (base_dir, lib, base_dir, lib, base_dir, lib))
            shutil.rmtree(os.path.join(self.package_folder, "lib", "arm64"))
            shutil.move(os.path.join(self.package_folder, "include", "opencv4", "opencv2"), os.path.join(self.package_folder, "include", "opencv2"))
            shutil.rmtree(os.path.join(self.package_folder, "include", "opencv4"))
        else:
            cmake = self._configure_cmake()
            cmake.build()

    def package(self):
        self.copy("LICENSE", dst="licenses", src=self._source_subfolder)
        if self.settings.os != "Macos":
            cmake = self._configure_cmake()
            cmake.install()
        tools.rmdir(os.path.join(self.package_folder, "cmake"))
        if os.path.isfile(os.path.join(self.package_folder, "setup_vars_opencv4.cmd")):
            tools.rename(os.path.join(self.package_folder, "setup_vars_opencv4.cmd"),
                         os.path.join(self.package_folder, "res", "setup_vars_opencv4.cmd"))

    def package_info(self):
        version = self.version.split(".")
        version = "".join(version) if self.settings.os == "Windows" else ""
        self.cpp_info.set_property("cmake_file_name", "OpenCV")

        if self.settings.os == "Windows":
            self.cpp_info.components["opencv_highgui"].system_libs = ["comctl32", "gdi32", "ole32", "setupapi", "ws2_32", "vfw32"]
        elif self.settings.os == "Macos":
            self.cpp_info.libs = [
                "IlmImf", "libwebp", "opencv_highgui", "opencv_video",
                "ade", "opencv_calib3d", "opencv_imgcodecs", "opencv_videoio",
                "ittnotify", "opencv_core", "opencv_imgproc", "quirc",
                "libjpeg-turbo", "opencv_dnn", "opencv_ml", "zlib",
                "libopenjp2", "opencv_features2d", "opencv_objdetect", 
                "libpng", "opencv_flann", "opencv_photo",
                "libprotobuf", "opencv_gapi", "opencv_stitching"
            ]
            self.cpp_info.frameworks = ["Cocoa", "Accelerate", "AVFoundation", "CoreGraphics", "CoreMedia", "CoreVideo", "QuartzCore", "OpenCL"]
        elif self.settings.os == "iOS":
            self.cpp_info.components["opencv_videoio"].frameworks = ["AVFoundation", "QuartzCore"]

        # TODO: to remove in conan v2 once cmake_find_package* generators removed
        self.cpp_info.filenames["cmake_find_package"] = "OpenCV"
        self.cpp_info.filenames["cmake_find_package_multi"] = "OpenCV"
