from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get

class libhvRecipe(ConanFile):
    name = "libhv"
    version = "1.3.4"
    package_type = "library"

    # Optional metadata
    license = "BSD-3-Clause"
    author = "ithewei"
    url = "github.com/ithewei/libhv/wiki"
    description = "A c/c++ network library for developing TCP/UDP/SSL/HTTP/WebSocket/MQTT client/server."
    topics = ("network", "tcp", "udp", "ssl", "http", "websocket", "mqtt")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    def source(self):
        get(self, **self.conan_data["sources"][self.version])
    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # self.cpp_info.libs = ["libhv"]
        if self.options.shared:
            self.cpp_info.libs = ["hv"]
        else:
            self.cpp_info.libs = ["hv_static"]
