import os
from SCons.Script import *
from SCons.Platform import Platform
from pathlib import Path
import sys
import shutil
import uuid
import subprocess
import tempfile
import logging
import parse

PROGRAM_PATH = os.path.normpath(os.environ.get('CONFIG_PROGRAM_PATH', os.getcwd()))
SDK_PATH = os.path.normpath(os.environ.get('CONFIG_SDK_PATH', str(Path(PROGRAM_PATH)/'..'/'..')))
TOOLS_PATH = os.path.normpath(os.environ.get('CONFIG_TOOLS_PATH', str(Path(SDK_PATH)/'tools'/'scons')))
REPOSITORY_PATH = os.path.normpath(os.environ.get('CONFIG_REPOSITORY_PATH', str(Path(SDK_PATH)/'github_source')))
COMPONENTS_PATH = [os.path.normpath(os.environ.get('CONFIG_COMPONENTS_PATH', str(Path(SDK_PATH)/'components')))]
if 'CONFIG_EXT_COMPONENTS_PATH' in os.environ:
    COMPONENTS_PATH.append(os.path.normpath(os.environ['CONFIG_EXT_COMPONENTS_PATH']))

BUILDDIR = os.path.normpath(os.environ.get('CONFIG_BUILDDIR', str(Path(PROGRAM_PATH)/'build')))
DISTDIR = os.path.normpath(os.environ.get('CONFIG_DISTDIR', str(Path(PROGRAM_PATH)/'dist')))
CONANHOMEDIR = os.path.normpath(os.environ.get('CONFIG_CONANHOMEDIR', str(Path(REPOSITORY_PATH)/'conan_home')))
CONANCOMPONENTDIR = os.path.normpath(os.environ.get('CONFIG_CONANCOMPONENTDIR', str(Path(SDK_PATH)/'conan_components')))

BUILD_CONFIG_PATH = os.environ.get('CONFIG_BUILD_CONFIG_PATH', str(Path(PROGRAM_PATH)/'build'/'config'))
CONFIG_TOOL_FILE = os.environ.get('CONFIG_TOOL_FILE', str(Path(SDK_PATH)/'tools'/'kconfig'/'genconfig.py'))

GLOBAL_CONFIG_MK_FILE = os.environ.get('CONFIG_GLOBAL_CONFIG_MK_FILE', str(Path(PROGRAM_PATH)/'build'/'config'/'global_config.mk'))
GLOBAL_CONFIG_H_FILE = os.environ.get('CONFIG_GLOBAL_CONFIG_H_FILE', str(Path(PROGRAM_PATH)/'build'/'config'/'global_config.h'))



def setup_logging():
    """Configure logging for the build system"""
    logging.basicConfig(
        level=logging.INFO,
        format='%(levelname)s: %(message)s'
    )
    return logging.getLogger('build')

logger = setup_logging()


def ourspawn(sh, escape, cmd, args, e):
    """Custom spawn function for executing commands with long argument lists"""
    filename = str(uuid.uuid4())
    newargs = ' '.join(args[1:])
    cmdline = cmd + " " + newargs
    
    # Handle command line length limit by writing arguments to a file
    if (len(cmdline) > 16 * 1024):
        with open(filename, 'w') as f:
            f.write(' '.join(args[1:]).replace('\\', '/'))
        # Execute with response file
        cmdline = cmd + " @" + filename
    
    # Execute the command
    proc = subprocess.Popen(
        cmdline, 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, 
        shell=False, 
        env=e
    )
    
    data, err = proc.communicate()
    rv = proc.wait()
    
    # Output handling function
    def res_output(_output, _s):
        if len(_s):
            if isinstance(_s, str):
                _output(_s)
            elif isinstance(_s, bytes):
                _output(str(_s, 'UTF-8'))
            else:
                _output(str(_s))
    
    # Write output to stdout/stderr
    res_output(sys.stderr.write, err)
    res_output(sys.stdout.write, data)
    
    # Clean up temporary file if created
    if os.path.isfile(filename):
        os.remove(filename)
    
    return rv
def copy_file(target, source, env):
    """Copy files or directories from source to target"""
    source_path = str(source[0])
    target_path = str(target[0])
    
    # Create target directory if it doesn't exist
    os.makedirs(os.path.dirname(target_path), exist_ok=True)
    
    # Copy file or directory
    if os.path.isfile(source_path):
        shutil.copy2(source_path, target_path)
    elif os.path.isdir(source_path):
        shutil.copytree(source_path, target_path)

def  GetLaunchDir():
    return Dir('.').srcnode().abspath



def run_kconfig_tool(menuconfig=False, build_type='Debug'):
    """Run the Kconfig tool to generate configuration files"""
    os.makedirs(BUILD_CONFIG_PATH, exist_ok=True)
    
    if not os.path.exists(CONFIG_TOOL_FILE):
        logger.error("Kconfig tool not found: {}".format(CONFIG_TOOL_FILE))
        sys.exit(1)
    
    cmd = [sys.executable, CONFIG_TOOL_FILE, "--kconfig", str(Path(SDK_PATH)/'Kconfig')]
    cmd.extend(["--defaults", str(Path(PROGRAM_PATH)/'config_defaults.mk')])
    
    # Environment variables
    cmd.extend([
        "--env", "SDK_PATH={}".format(SDK_PATH),
        "--env", "PROJECT_PATH={}".format(PROGRAM_PATH),
        "--env", "BUILD_TYPE={}".format(build_type)
    ])
    
    # Add menuconfig option if requested
    if menuconfig:
        cmd.extend(["--menuconfig", "True"])
    
    # Output files
    cmd.extend([
        "--output", "makefile", str(Path(PROGRAM_PATH)/'build'/'config'/'global_config.mk'),
        "--output", "header", str(Path(PROGRAM_PATH)/'build'/'config'/'global_config.h')
    ])
    
    return subprocess.call(cmd)


if 'distclean' in sys.argv:
    if os.path.exists('build'):
        shutil.rmtree('build')
    if os.path.exists('dist'):
        shutil.rmtree('dist')
    exit(0)

if 'menuconfig' in sys.argv:
    run_kconfig_tool(menuconfig=True)
    exit(0)
elif not os.path.exists(GLOBAL_CONFIG_MK_FILE):
    run_kconfig_tool(menuconfig=False)
try:
    with open(GLOBAL_CONFIG_MK_FILE, 'r') as f:
        pattern = parse.compile("CONFIG_{}={}\n")
        for line in f.readlines():
            Pobj = pattern.parse(line)
            if Pobj:
                key, value = Pobj.fixed
                key = 'CONFIG_' + key 
                if key not in os.environ:
                    os.environ[key] = value.strip('"')
                
except Exception as e:
    logger.error('Error parsing {}: {}'.format(GLOBAL_CONFIG_MK_FILE, e))
    exit(-1)

# print(os.environ)
# print(GLOBAL_CONFIG_MK_FILE)

class buildEnvironment:
    BuildEnv = None
    ProjectEnv = None
    def __init__(self, ProjectEnv, BuildEnv):
        self.BuildEnv = BuildEnv
        self.ProjectEnv = ProjectEnv
        self.local_info = {
            'REQUIREMENTS': [],
            'version': '0.0.1',
            'ALL_TARGETS': []
        }
        self.cpp_info = {
            'CPPPATH': [], 'LIBPATH': [], 'BINPATH': [],
            'LIBS': [], 'FRAMEWORKS': [], 'FRAMEWORKPATH': [],
            'CPPDEFINES': [], 'CXXFLAGS': [], 'CCFLAGS': [],
            'SHLINKFLAGS': [], 'LINKFLAGS': []
        }
        self.all_envs = {}



    def install_CPPPATH(self, path):
        if isinstance(path, list) or isinstance(path, tuple):
            for p in path:
                if not os.path.isabs(p):
                    p = os.path.join(self.BuildEnv['WORKDIR'], p)
                self.cpp_info['CPPPATH'].append(p)
        else:
            if not os.path.isabs(path):
                path = os.path.join(self.BuildEnv['WORKDIR'], path)
            self.cpp_info['CPPPATH'].append(path)
    def install_LIBS(self, libs):
        if isinstance(libs, list) or isinstance(libs, tuple):
            for p in libs:
                self.cpp_info['LIBS'].append(p)
        else:
            self.cpp_info['LIBS'].append(libs)


    def select_requirements(self, req):
        # print(self.ProjectEnv['ALL_ENVS'])
        if '/' in req:
            pass
        elif req in self.ProjectEnv['COMPONENTS']:
            if os.path.join(self.ProjectEnv['COMPONENTS'][req], 'Sconstruct') not in self.ProjectEnv['ALL_ENVS']:
                self.SConscript(os.path.join(self.ProjectEnv['COMPONENTS'][req], 'Sconstruct'))
        else:
            for i in self.ProjectEnv['ALL_ENVS']:
                if req in self.ProjectEnv['ALL_ENVS'][i]['env'].build.cpp_info['LIBS']:
                    self.BuildEnv.MergeFlags(self.ProjectEnv['ALL_ENVS'][i]['env'].build.cpp_info)


    def require(self, name):
        if isinstance(name, list) or isinstance(name, tuple):
            for req in name:
                self.local_info['REQUIREMENTS'].append(req)
                self.select_requirements(req)
        else:
            self.local_info['REQUIREMENTS'].append(name)
            self.select_requirements(name)



    def ObjectGenerated(self, _src, objs, Share = False):
        # print(os.getcwd(), _src)
        # print(self.BuildEnv['WORKDIR'],'----workdir')
        if _src.endswith('.c') or _src.endswith('.cpp') or _src.endswith('.s') or _src.endswith('.S') or _src.endswith('.cc') or _src.endswith('.CC'):
            obj = ''
            # if _src.startswith(self.ProjectEnv['SDK_PATH']):
            #     obj = os.path.relpath(_src, self.ProjectEnv['SDK_PATH']) + '.o'
            if _src.startswith('/'):
                obj = _src[1:] + '.o'
            elif _src.startswith('''C:\\'''):
                obj = _src[4:] + '.o'
            else:
                obj = _src + '.o'
            if Share:
                objs.append(self.BuildEnv.SharedObject(obj, _src))
            else:
                objs.append(self.BuildEnv.Object(obj, _src))
        else:
            objs.append(_src)
    def Program(self, target = None, source = None, *args, **kw):
        _target = os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.getcwd()),target) 
        srcs = []
        objs = []

        if isinstance(source, list) or isinstance(source, tuple):
            for src in source:
                _src = str(src)
                self.ObjectGenerated(_src, objs)
        else:
            self.ObjectGenerated(str(source), objs)

        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                self.BuildEnv.MergeFlags(self.ProjectEnv['ALL_ENVS'][found[0]]['env'].build.cpp_info)

        prog = self.BuildEnv.Program(_target, objs, *args, **kw)
        self.BuildEnv['ENVTARGET'] = prog

        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                Depends(prog, self.ProjectEnv['ALL_ENVS'][found[0]]['prog'])


        self.BuildEnv.Command(
            os.path.join(self.ProjectEnv['DISTDIR'], target), 
            _target, 
            action=copy_file
        )

        return prog

    def StaticLibrary(self, target = None, source = None, *args, **kw):
        _target = os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.getcwd()),target) 
        
        srcs = []
        objs = []

        if isinstance(source, list) or isinstance(source, tuple):
            for src in source:
                _src = str(src)
                self.ObjectGenerated(_src, objs)
        else:
            self.ObjectGenerated(str(source), objs)


        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                self.BuildEnv.MergeFlags(self.ProjectEnv['ALL_ENVS'][found[0]]['env'].build.cpp_info)
        
        prog = self.BuildEnv.StaticLibrary(_target, objs, *args, **kw)

        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                Depends(prog, self.ProjectEnv['ALL_ENVS'][found[0]]['prog'])


        # self.ProjectEnv['ALL_ENVS'][self.BuildEnv['SCRIPT_PATH']]['prog'] = prog
        self.ProjectEnv['ALL_ENVS'][self.BuildEnv['SCRIPT_PATH']]['prog'] = prog
        self.BuildEnv.build.cpp_info['LIBS'].append(target[3:])
        self.BuildEnv.build.cpp_info['LIBPATH'].append(os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.getcwd())))
        return prog


    def SharedLibrary(self, target = None, source = None, *args, **kw):
        _target = os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.getcwd()),target) 
        
        srcs = []
        objs = []

        if isinstance(source, list) or isinstance(source, tuple):
            for src in source:
                _src = str(src)
                self.ObjectGenerated(_src, objs, True)
        else:
            self.ObjectGenerated(str(source), objs, True)

        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                self.BuildEnv.MergeFlags(self.ProjectEnv['ALL_ENVS'][found[0]]['env'].build.cpp_info)

        prog = self.BuildEnv.SharedLibrary(_target, objs, *args, **kw)

        for req in self.local_info['REQUIREMENTS']:
            found = list(filter(lambda x: os.path.basename(os.path.dirname(x))==req, self.ProjectEnv['ALL_ENVS'].keys()))
            if found :
                Depends(prog, self.ProjectEnv['ALL_ENVS'][found[0]]['prog'])


        self.ProjectEnv['ALL_ENVS'][self.BuildEnv['SCRIPT_PATH']]['prog'] = prog
        self.BuildEnv.build.cpp_info['LIBS'].append(target[3:])
        self.BuildEnv.build.cpp_info['LIBPATH'].append(os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.getcwd())))


        self.BuildEnv.Command(
            os.path.join(self.ProjectEnv['DISTDIR'], target), 
            _target, 
            action=copy_file
        )

        return prog

    def SConscript(self, scripts ,variant_dir = '',duplicate = 0,exports = None,  *args, **kw):
        new_env = self.ProjectEnv.Clone()
        new_env.build = buildEnvironment(self.ProjectEnv, new_env)
        # print(os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.path.dirname(scripts))), '-----------------a')
        new_env['SCRIPT_PATH'] = scripts
        self.ProjectEnv['ALL_ENVS'][scripts] = {'env':new_env}
        if os.path.isabs(scripts):
            new_env['WORKDIR'] = os.path.dirname(scripts)
        else:
            new_env['WORKDIR'] = os.path.join(os.getcwd(), os.path.dirname(scripts))
        # os.chdir(os.path.dirname(scripts))
        SConscript(scripts, 
                variant_dir=os.path.join(self.ProjectEnv['BUILDDIR'], os.path.basename(os.path.dirname(scripts))), 
                duplicate=0, 
                exports={
                        'env': new_env,
                        'AGetLaunchDir':GetLaunchDir
                        }, 
                *args, **kw)
        return new_env
    def conan_install(libs=[]):
        from multiprocessing import Process
        def conan_install_process(conan_libs):
            import os
            # os.chdir('/tmp')
            from conan.cli.cli import main as conan_main
            # conan install . --requires=libv4l/1.32.0 --generator=SConsDeps --options=jsoncpp/*:shared=False --layout=cmake_layout
            cmd = ['install','..', '--output-folder=build','--profile:host=./build/config/corss-toolchina', '--build=missing', '--generator=SConsDeps']
            for lib in conan_libs:
                cmd.append(f'--requires={lib}')
            conan_main(cmd)
        p = Process(target=conan_install_process, args=(libs,), kwargs={})
        p.start()
        p.join()

def generate(env):
    SConsignFile('build/sconsign.dblite')
    env.Tool(Platform('posix'))
    env.Tool('gcc')
    env.Tool('g++')
    env.Tool('gnulink')
    env.Tool('ar')
    env.Tool('gas')
    env.Tool('as')

    env['GCCSUFFIX'] = ''
    env['CC'] = '${_concat(GCCPREFIX, "gcc", GCCSUFFIX, __env__)}'          
    env['CXX'] = '${_concat(GCCPREFIX, "g++", GCCSUFFIX, __env__)}'     
    env['AR'] = '${_concat(GCCPREFIX, "ar", GCCSUFFIX, __env__)}'      
    env['AS'] = '${_concat(GCCPREFIX, "as", GCCSUFFIX, __env__)}'        
    env['STRIP'] = '${_concat(GCCPREFIX, "strip", GCCSUFFIX, __env__)}'  
    env['OBJCOPY'] = '${_concat(GCCPREFIX, "objcopy", GCCSUFFIX, __env__)}'     
    env['SIZE'] = '${_concat(GCCPREFIX, "size", GCCSUFFIX, __env__)}'  

    if 'CONFIG_COMMPILE_DEBUG' not in os.environ:
        env['ASCOMSTR'] = "AS $SOURCES"
        env['CCCOMSTR'] = "CC $SOURCES"
        env['CXXCOMSTR'] = "CXX $SOURCES"
        env['SHCCCOMSTR'] = "CC -fPIC $SOURCES"
        env['SHCXXCOMSTR'] = "CXX -fPIC $SOURCES"
        env['ARCOMSTR'] = "LD $TARGET"
        env['SHLINKCOMSTR'] = "Linking $TARGET"
        env['LINKCOMSTR'] = 'Linking $TARGET'

    env['GCCPREFIX'] = os.path.join(os.environ.get('CONFIG_TOOLCHAIN_PATH', ''), os.environ.get('CONFIG_TOOLCHAIN_PREFIX', '')) 
    
    if env['HOST_OS'].startswith('win'):
        env['SPAWN'] = ourspawn
        env['GCCSUFFIX'] = '.exe'
        env['ENV']['Path'] = os.environ.get('CONFIG_TOOLCHAIN_PATH', '') + ';' + os.environ['Path'] if os.path.exists(os.environ.get('CONFIG_TOOLCHAIN_PATH', '')) else os.environ['Path']
    else:
        env['ENV']['PATH'] = os.environ.get('CONFIG_TOOLCHAIN_PATH', '') + ':' + os.environ['PATH'] if os.path.exists(os.environ.get('CONFIG_TOOLCHAIN_PATH', '')) else os.environ['PATH']

    env['PROGRAM_PATH'] = PROGRAM_PATH
    env['SDK_PATH'] = SDK_PATH
    env['TOOLS_PATH'] = TOOLS_PATH
    env['REPOSITORY_PATH'] = REPOSITORY_PATH
    env['COMPONENTS_PATH'] = COMPONENTS_PATH

    env['BUILDDIR'] = BUILDDIR
    env['DISTDIR'] = DISTDIR
    env['CONANHOMEDIR'] = CONANHOMEDIR
    env['CONANCOMPONENTDIR'] = CONANCOMPONENTDIR
    if 'CONAN_HOME' not in os.environ:
        os.environ['CONAN_HOME'] = CONANHOMEDIR

    env['COMPONENTS'] = {}
    for COMPONENT_PATH in env['COMPONENTS_PATH']:
        comps = Glob(str(Path(COMPONENT_PATH)/'*'/'Sconstruct'))
        for comp in comps:
            comp_name = os.path.basename(os.path.dirname(str(comp)))
            env['COMPONENTS'][comp_name] = str(Path(COMPONENT_PATH)/comp_name)
    env['ALL_ENVS'] = {}
    env['WORKDIR'] = os.getcwd()
    env.GetLaunchDir = GetLaunchDir
    # env.Append(CPPPATH=BUILD_CONFIG_PATH)
    env.build = buildEnvironment(env, env)
    # print(sys.argv)








def exists(env):
    """
    检查工具链是否存在
    """
    # return env.Detect('arm-none-eabi-gcc')
    pass

