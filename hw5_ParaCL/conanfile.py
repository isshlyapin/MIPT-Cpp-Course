from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

class MainRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    
    options = { 
        "ecc": [True, False],
        "btest": [True, False],
        "importstd": [True, False]
    }
    
    default_options = { 
        "ecc": False,
        "btest": False,
        "importstd": False
    }

    def requirements(self):
        self.requires("gtest/1.16.0")
        self.requires("bison/3.8.2")        
        self.requires("flex/2.6.4")
        self.requires("fmt/12.1.0")
        
    def layout(self):
        cmake_layout(self, build_folder="build")

    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.ecc:
            tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        if self.options.btest:
            tc.variables["BUILD_TEST"] = "ON"
        if self.options.importstd:
            tc.variables["USE_IMPORT_STD"] = "ON"
        else:
            tc.variables["USE_IMPORT_STD"] = "OFF"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()