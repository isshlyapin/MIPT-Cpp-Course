from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

class MainRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    
    options = { 
        "ecc_opt": [True, False],
        "btest_opt": [True, False],
        "importstd_opt": [True, False]
    }
    
    default_options = { 
        "ecc_opt": False,
        "btest_opt": False,
        "importstd_opt": True
    }

    def requirements(self):
        self.requires("gtest/1.16.0")
        
    def layout(self):
        cmake_layout(self, build_folder="build")

    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.ecc_opt:
            tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        if self.options.btest_opt:
            tc.variables["BUILD_TEST"] = "ON"
        if self.options.importstd_opt:
            tc.variables["USE_IMPORT_STD"] = "ON"
        else:
            tc.variables["USE_IMPORT_STD"] = "OFF"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()