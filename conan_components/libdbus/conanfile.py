from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get
from conan.tools.scm import Git
from conan.tools.layout import basic_layout
from conan.tools.gnu import Autotools, AutotoolsToolchain
from conan.tools.meson import Meson, MesonToolchain
from conan.tools.gnu import Autotools, AutotoolsToolchain, PkgConfigDeps
import os
class libdbusRecipe(ConanFile):
    name = "libdbus"
    version = "1.12.20"
    package_type = "library"

    # Optional metadata
    license = "BSD-3-Clause"
    author = "ithewei"
    url = "https://dbus.freedesktop.org/releases/dbus/"
    description = "D-Bus message bus system"
    topics = ("dbus")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # def source(self):
        # git = Git(self)
        # git.clone(url="https://github.com/philips/libdbus.git", target=".")
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
        # self.tool_requires("meson/[>=1.0.0]")
        # self.tool_requires("ninja/[>=1.11.0]")
        pass

    def generate(self):
        at_toolchain = AutotoolsToolchain(self)
        at_toolchain.generate()
        deps = PkgConfigDeps(self)
        deps.generate()

        # tc = MesonToolchain(self)
        # tc.generate()

    def requirements(self):
        # 添加 expat 库依赖
        self.requires("expat/[>=2.4.0]")

    def build(self):
        # self.run('pwd')
        autotools = Autotools(self)

        expat_dep = self.dependencies["expat"]
        expat_root = expat_dep.package_folder
        expat_include = os.path.join(expat_root, "include")
        expat_lib = os.path.join(expat_root, "lib")
        
        configure_args = [
            "--enable-shared" if self.options.shared else "--disable-shared",
            "--disable-static" if self.options.shared else "--enable-static",
            "--disable-tests",
            "--disable-asserts",
            "--disable-verbose-mode",
            "--disable-doxygen-docs",
            "--disable-xml-docs",
            "--disable-dnotify",
            "--disable-inotify",
            "--disable-kqueue",
            "--disable-systemd",
            "--disable-traditional-activation",
            "--disable-launchd",
            "--without-x",
            "--disable-selinux",
            "--disable-apparmor",
            "--disable-libaudit",
            "--disable-stats",
            f"EXPAT_CFLAGS=-I{expat_include}",
            f"EXPAT_LIBS=-L{expat_lib} -lexpat"
        ]
        # autotools.autoreconf()
        autotools.configure(args=configure_args)
        autotools.make()
        # meson = Meson(self)
        # meson.configure()
        # meson.build()

    def package(self):
        # meson = Meson(self)
        # meson.install()
        autotools = Autotools(self)
        autotools.install()
        # fix_apple_shared_install_name(self)

    def package_info(self):
        self.cpp_info.libs = ["dbus-1"]
        # self.cpp_info.libdirs = [
        #     "lib",
        #     os.path.join("lib", "libdbus"),
        #     os.path.join("lib", "libdbus", "plugins")
        # ]
        # if self.options.shared:
        #     self.cpp_info.libs = ["hv"]
        # else:
        #     self.cpp_info.libs = ["hv_static"]
