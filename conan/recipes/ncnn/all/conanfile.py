from fileinput import filename
from conans import ConanFile, tools
import os
import subprocess

required_conan_version = ">=1.33.0"

class NcnnConan(ConanFile):
    name = "ncnn"
    description = "ncnn is a high-performance neural network inference computing framework optimized for mobile platforms."
    topics = ("dnn", "cnn", "opencv")
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/Tencent/ncnn"
    license = "BSD"

    settings = "os"
    options = {
        "with_vulkan": [True, False]
    }
    default_options = {
        "with_vulkan": True
    }

    def source(self):
        if self.options.with_vulkan:
            key = "%s_%s_vulkan" % (self.version, self.settings.os)
        else:
            key = "%s_%s" % (self.version, self.settings.os)
        file_name = "%s.zip" % key
        if self.settings.os == "Macos":
            tools.download(**self.conan_data["sources"][key], filename = file_name)
            subprocess.call(["unzip", "-q", file_name])

    def package(self):
        if self.settings.os == "Macos":
            self.copy("*", symlinks = True)

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "ncnn"
        self.cpp_info.names["cmake_find_package_multi"] = "ncnn"
        self.cpp_info.libs.append("ncnn")
        if self.settings.os == "Macos":
            self.cpp_info.includedirs.append(os.path.join("lib", "ncnn.framework", "Headers"))
            self.cpp_info.frameworks.extend(["ncnn", "glslang", "openmp"])
            self.cpp_info.frameworkdirs.append(self.package_folder)
 