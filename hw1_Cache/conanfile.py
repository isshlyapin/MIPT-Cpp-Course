from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class MainRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("cli11/2.5.0")
        self.requires("gtest/1.16.0")
        
    def build_requirements(self):
        self.tool_requires("cmake/4.0.1")
        self.tool_requires("ninja/1.13.1")

    def layout(self):
        cmake_layout(self, build_folder="build")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()