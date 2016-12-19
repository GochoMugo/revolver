#!/usr/bin/env python


from conans import ConanFile, CMake


class RevolverConan(ConanFile):
    name = "revolver"
    description = "Push, Shift and Revolve with your Items File"
    version = "0.1.0"
    url = "https://github.com/GochoMugo/revolver"
    license = "MIT"
    settings = "os", "compiler", "build_type", "arch"
    exports = [
        "include/*",
        "src/*",
        "CHANGELOG.md",
        "LICENSE.txt",
        "README.md",
        "CMakeLists.txt",
    ]

    def build(self):
        cmake = CMake(self.settings)
        self.run("cmake %s %s" % (self.conanfile_directory, cmake.command_line))
        self.run("cmake --build . --config Release %s" % (cmake.build_config))

    def package(self):
        self.copy("*", dst="include", src="include")
        self.copy("*.so", dst="lib")
        self.copy("*.lib", dst="lib")

    def package_info(self):
        self.cpp_info.libs = ["revolver"]
