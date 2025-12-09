from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

class MainRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    
    options = { 
        "ecc": [True, False],
        "btest": [True, False]
    }
    
    default_options = { 
        "ecc": False,
        "btest": False
    }

    def requirements(self):
        self.requires("catch2/3.8.0")
        
    def layout(self):
        cmake_layout(self, build_folder="build")

    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.ecc:
            tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        if self.options.btest:
            tc.variables["BUILD_TEST"] = "ON"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
