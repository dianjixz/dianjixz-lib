import os, sys
import Path
import uuid
import subprocess
env['SDK_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..')))
env['SDK_TOOLS_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..'/'tools')))
env['SDK_CACHE_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..'/'github_source')))
env['SDK_PROJECT_NAME'] = os.environ.get('PROJECT_NAME', os.path.basename(sys.path[0]))
env['SDK_PROJECT_PATH'] = os.environ.get('PROJECT_PATH', sys.path[0])
env['SDK_BUILD_PATH'] = os.environ.get('BUILD_PATH', str(Path(env['SDK_PROJECT_PATH'])/'build'))


class SconsBuild():
    """SCons build system for M5Stack projects"""
    env = None
    module_info = {
            "CPPPATH"     : [],
            "LIBPATH"     : [],
            "BINPATH"     : [],
            "LIBS"        : [],
            "FRAMEWORKS"  : [],
            "FRAMEWORKPATH" : [],
            "CPPDEFINES"  : [],
            "CXXFLAGS"    : [],
            "CCFLAGS"     : [],
            "SHLINKFLAGS" : [],
            "LINKFLAGS"   : [],
            
        }
    def __init__(self, env):
        self.env = env

    def Build(self):
        self.ParseCmd()
        self.Parse()
        self.Configure()
        self.CreatBuildTask()

    def CMDmenuconfig(self):
        pass
    def CMDclean(self):
        pass
    def CMDdistclean(self):
        pass
    def CMDsave(self):
        pass
    def CMDSET_CROSS(self):
        pass

    def CMDpush(self):
        pass
    def ParseCmd(self):
        TaskList = ['menuconfig', 'clean', 'distclean', 'save', 'SET_CROSS', 'push']
        for Cmd in TaskList:
            if Cmd in sys.argv:
                getattr(self, 'CMD'+Cmd)()
    def Configure(self):
        self.LoadConfig()
        self.SetBuildEnv()
        self.SetupEnv()
    def LoadTargets(self):
        self.FoundTarget()
        self.Load()


    def FoundTarget(self):
        Targets = []
        for component_path in env['COMPONENTS_PATH']:
            if os.path.exists(component_path):
                for component_name in os.listdir(component_path):
                    component_scons = str(Path(component_path)/component_name/'SConstruct')
                    if os.path.exists(component_scons):
                        Targets.append([component_scons])

        for component_path in env['EXT_COMPONENTS_PATH']:
            if os.path.exists(component_path):
                for component_name in os.listdir(component_path):
                    component_scons = str(Path(component_path)/component_name/'SConstruct')
                    if os.path.exists(component_scons):
                        Targets.append([component_scons])

        for project_dir in os.listdir(env['SDK_PROJECT_PATH']):
            if project_dir.startswith("main"):
                main_scons = str(Path(env['SDK_PROJECT_PATH'])/project_dir/'SConstruct')
                if os.path.exists(main_scons):
                    Targets.append([main_scons])
        self.env['Targets'] = Targets
    def Load(self, Target):
        exportobject = {
            'env': self.env.Clone(),
            'DefineProject': self.DefineProject,
            'DefineStatic': self.DefineStatic,
            'DefineLibrary': self.DefineLibrary,
            'ImportConan': self.ImportConan,
        }
        for Target in self.env['Targets']:
            Target_env = SConscript(Target[0], exports=exportobject)
            Target.append(Target_env)


    def LoadConfig(self):
        # gcc cross




        pass









    def SetBuildEnv(self):
        # Set up basic environment variables
        self.env['GCCPREFIX'] = ''
        self.env['GCCSUFFIX'] = ''
        self.env['LINK'] = '$SMARTLINK'
        self.env['SHLINK'] = '$LINK'
        self.env['OBJPREFIX'] = ''
        self.env['OBJSUFFIX'] = '.o'
        self.env['LIBPREFIX'] = 'lib'
        self.env['LIBSUFFIX'] = '.a'
        self.env['SHLIBPREFIX'] = '$LIBPREFIX'
        self.env['SHLIBSUFFIX'] = '.so'
        self.env['INCPREFIX'] = '-I'
        self.env['INCSUFFIX'] = ''
        self.env['LIBDIRPREFIX'] = '-L'
        self.env['LIBDIRSUFFIX'] = ''
        self.env['LIBLINKPREFIX'] = '-l'
        self.env['LIBLINKSUFFIX'] = ''
        self.env['CPPDEFPREFIX'] = '-D'
        self.env['CPPDEFSUFFIX'] = ''
        self.env['PROGSUFFIX'] = ''
        self.env['ARFLAGS'] = ['rc']
        self.env['SHCCFLAGS'] = ['$CCFLAGS', '-fPIC']
        self.env['SHLINKFLAGS'] = ['$LINKFLAGS', '-shared']
        self.env['CFLAGS'] = []
        self.env['CXXFLAGS'] = []
        self.env['ASFLAGS'] = []
        self.env['LINKFLAGS'] = []

        # Set up command strings
        self.env['CCCOM'] = '$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
        self.env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
        self.env['ASCOM'] ='$AS $ASFLAGS -o $TARGET $SOURCES'
        self.env['ARCOM'] = '$AR $ARFLAGS $TARGET $SOURCES'
        self.env['SHCCCOM'] = '$SHCC -o $TARGET -c $SHCFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
        self.env['SHCXXCOM'] = '$SHCXX -o $TARGET -c $SHCXXFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
        self.env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH $_LIBDIRFLAGS $_LIBFLAGS'
        self.env['SHLINKCOM'] = '$SHLINK -o $TARGET $SHLINKFLAGS $__SHLIBVERSIONFLAGS $__RPATH $SOURCES $_LIBDIRFLAGS $_LIBFLAGS'
        self.env['STRIPCOM'] = '$STRIP $SOURCES'
        
        if 'CONFIG_COMMPILE_DEBUG' not in os.environ:
            self.env['ASCOMSTR'] = "AS $SOURCES"
            self.env['CCCOMSTR'] = "CC $SOURCES"
            self.env['CXXCOMSTR'] = "CXX $SOURCES"
            self.env['SHCCCOMSTR'] = "CC -fPIC $SOURCES"
            self.env['SHCXXCOMSTR'] = "CXX -fPIC $SOURCES"
            self.env['ARCOMSTR'] = "LD $TARGET"
            self.env['SHLINKCOMSTR'] = "Linking $TARGET"
            self.env['LINKCOMSTR'] = 'Linking $TARGET'

        self.env.AddMethod(self.Fatal, "Fatal")


        # Configure toolchain prefix if specified
        if os.environ.get('CONFIG_TOOLCHAIN_PREFIX'):
            self.env['GCCPREFIX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"')    
        # Configure PATH based on host OS
        if self.env['HOST_OS'].startswith('win'):
            if os.environ.get('CONFIG_TOOLCHAIN_PATH'):
                self.env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + self.env['ENV']['PATH'] + ';' + os.getenv('Path')
            else:
                self.env['ENV']['PATH'] = self.env['ENV']['PATH'] + ';' + os.getenv('Path')
            self.env['GCCSUFFIX'] = '.exe'
            self.env['SPAWN'] = self.ourspawn
        elif self.env['HOST_OS'].startswith('posix'):
            if os.environ.get('CONFIG_TOOLCHAIN_PATH'):
                self.env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ':' + self.env['ENV']['PATH']
        else:
            logger.warning('Unknown OS: {}'.format(self.env["HOST_OS"]))

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
    def Fatal(self, msg):
        print("FATAL ERROR: " + msg)
        sys.exit(1)





    def SetupEnv(self):
        env['COMPONENTS_PATH'] = [str(Path(env['SDK_PATH'])/'components')]
        if 'EXT_COMPONENTS_PATH' in os.environ:
            for ecp in os.environ['EXT_COMPONENTS_PATH'].split(':'):
                env['COMPONENTS_PATH'].append(ecp)

    def DefineProject(self):
        pass
    def DefineStatic(self):

        """
        定义静态方法的函数
        该方法用于定义静态方法，目前为空实现（pass）
        静态方法不需要实例化类就可以调用
        """
        pass  # 空实现，表示该方法暂不执行任何操作
    def DefineLibrary(self):
        pass
    def ImportConan(self):
        pass
    def SampleWget(self, url, path):
        pass
    def SampleGitCloneComit(self, url, commit):
        pass

SConsignFile('build/sconsign.dblite')
env = Environment(tools=['gcc', 'g++', 'gnulink', 'ar', 'gas', 'as'])
ProjectTasks = SconsBuild(env)
ProjectTasks.Build()
Return('env')