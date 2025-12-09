
Import('env')
import os, sys
import Path
env['SDK_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..')))
env['SDK_TOOLS_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..'/'tools')))
env['SDK_CACHE_PATH'] = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..'/'github_source')))
env['SDK_PROJECT_NAME'] = os.environ.get('PROJECT_NAME', os.path.basename(sys.path[0]))
env['SDK_PROJECT_PATH'] = os.environ.get('PROJECT_PATH', sys.path[0])
env['SDK_BUILD_PATH'] = os.environ.get('BUILD_PATH', str(Path(env['SDK_PROJECT_PATH'])/'build'))


class SconsBuild():
    """SCons build system for M5Stack projects"""
    

    def __init__(self, env):
        self.env = env
        self.ParseCmd()
        self.Configure()
        self.LoadTargets()

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
        # 配置 config
        self.Config()
        # 加载 config 到环境变量
        self.LoadConfig()
        # 设置编译环境
        self.SetupEnv()
    def LoadTargets(self):
        Targets = self.FoundTarget()
        for Target in Targets:
            self.Load(Target)


    def FoundTarget(self):
        Targets = []
        for component_path in env['COMPONENTS_PATH']:
            if os.path.exists(component_path):
                for component_name in os.listdir(component_path):
                    component_scons = str(Path(component_path)/component_name/'SConstruct')
                    if os.path.exists(component_scons):
                        Targets.append(component_scons)

        for project_dir in os.listdir(env['SDK_PROJECT_PATH']):
            if project_dir.startswith("main"):
                main_scons = str(Path(env['SDK_PROJECT_PATH'])/project_dir/'SConstruct')
                if os.path.exists(main_scons):
                    Targets.append(main_scons)
        return Targets
    def Load(self, Target):
        exportobject = {
            'env': self.env,
            'DefineProject': self.DefineProject,
            'DefineStatic': self.DefineStatic,
            'DefineLibrary': self.DefineLibrary,
            'ImportConan': self.ImportConan,
        }
        SConscript(Target, exports=exportobject)


    def Config(self):
        pass
    def LoadConfig(self):
        pass
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


ProjectTasks = SconsBuild(env)
Return('env')