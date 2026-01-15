from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get
from conan.tools.scm import Git
from conan.tools.layout import basic_layout
from conan.tools.gnu import Autotools, AutotoolsToolchain
from conan.tools.meson import Meson, MesonToolchain
import os
class libv4lRecipe(ConanFile):
    name = "libv4l"
    version = "1.32.0"
    package_type = "library"

    # Optional metadata
    license = "BSD-3-Clause"
    author = "ithewei"
    url = "https://linuxtv.org/downloads/v4l-utils/"
    description = "v4l2 library and tools for video4linux devices"
    topics = ("v4l2")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # def source(self):
        # git = Git(self)
        # git.clone(url="https://github.com/philips/libv4l.git", target=".")
    def source(self):
        get(self, **self.conan_data["sources"][self.version])
    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        basic_layout(self)
    
    def build_requirements(self):
        # 确保 meson 和 ninja 可用
        self.tool_requires("meson/[>=1.0.0]")
        self.tool_requires("ninja/[>=1.11.0]")

    def generate(self):
        # at_toolchain = AutotoolsToolchain(self)
        # at_toolchain.generate()
        tc = MesonToolchain(self)
        tc.generate()

    def build(self):
        # self.run('pwd')
        # autotools = Autotools(self)
        # # autotools.autoreconf()
        # # autotools.configure()
        # autotools.make(args=["-C", self.source_folder])
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        meson = Meson(self)
        meson.install()
        # autotools = Autotools(self)
        # autotools.install()
        # fix_apple_shared_install_name(self)

    def package_info(self):
        self.cpp_info.libs = ["v4lconvert"]
        self.cpp_info.libdirs = [
            "lib",
            os.path.join("lib", "libv4l"),
            os.path.join("lib", "libv4l", "plugins")
        ]
        # if self.options.shared:
        #     self.cpp_info.libs = ["hv"]
        # else:
        #     self.cpp_info.libs = ["hv_static"]
