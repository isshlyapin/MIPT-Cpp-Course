import os

from conan import ConanFile
from conan.tools.cmake import CMake

class MainRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("gtest/1.16.0")
        
    def build_requirements(self):
        self.tool_requires("cmake/4.0.1")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()