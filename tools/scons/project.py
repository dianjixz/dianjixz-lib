# SConstruct
from pathlib import Path
import os, sys, shutil
import copy
from SCons.Node.FS import File
import subprocess

SDK_PATH = str(Path(os.getcwd())/'..'/'..')
PROJECT_NAME = os.path.basename(sys.path[0])
PROJECT_PATH = sys.path[0]

# print(sys.argv)
if 'menuconfig' in sys.argv:
    if not os.path.exists(str(Path(PROJECT_PATH)/'build')):
        os.mkdir(str(Path(PROJECT_PATH)/'build'))
    if not os.path.exists(str(Path(PROJECT_PATH)/'build'/'config')):
        os.mkdir(str(Path(PROJECT_PATH)/'build'/'config'))

    build_type = 'Debug'
    tool_path = str(Path(SDK_PATH)/'tools'/'kconfig'/'genconfig.py')
    if not os.path.exists(tool_path):
        print("[ERROR] kconfig tool not found:", tool_path)
        exit(1)
    # get default files
    # config_files = get_config_files(project_args.config_file, sdk_path, project_path)
    cmd = [sys.executable, tool_path, "--kconfig", str(Path(SDK_PATH)/'Kconfig')]
    for path in [str(Path(PROJECT_PATH)/'config_defaults.mk')]:
        cmd.extend(["--defaults", path])
    cmd.extend(["--menuconfig", "True", "--env", "SDK_PATH={}".format(SDK_PATH),
                                        "--env", "PROJECT_PATH={}".format(PROJECT_PATH),
                                        "--env", "BUILD_TYPE={}".format(build_type)])
    cmd.extend(["--output", "makefile", str(Path(PROJECT_PATH)/'build'/'config'/'global_config.mk')])
    cmd.extend(["--output", "header", str(Path(PROJECT_PATH)/'build'/'config'/'global_config.h')])
    res = subprocess.call(cmd)

    exit(0)

# print(sys.argv)
if 'clean' in sys.argv:
    try:
        shutil.rmtree('build')
    except:
        pass
    try:
        os.remove('.sconsign.dblite')
    except:
        pass
    
    exit(0)

if 'distclean' in sys.argv:
    paths_to_remove = ['build', 'dist', '.sconsign.dblite', '.config.old', '.config', '.config.mk']
    for path in paths_to_remove:
        try:
            if os.path.exists(path):
                if os.path.isdir(path):
                    shutil.rmtree(path)
                else:
                    os.remove(path)
        except :
            pass
    exit(0)

if not os.path.exists(str(Path(PROJECT_PATH)/'build'/'config'/'global_config.mk')):
    if not os.path.exists(str(Path(PROJECT_PATH)/'build')):
        os.mkdir(str(Path(PROJECT_PATH)/'build'))
    if not os.path.exists(str(Path(PROJECT_PATH)/'build'/'config')):
        os.mkdir(str(Path(PROJECT_PATH)/'build'/'config'))

    build_type = 'Debug'
    tool_path = str(Path(SDK_PATH)/'tools'/'kconfig'/'genconfig.py')
    if not os.path.exists(tool_path):
        print("[ERROR] kconfig tool not found:", tool_path)
        exit(1)
    # get default files
    # config_files = get_config_files(project_args.config_file, sdk_path, project_path)
    cmd = [sys.executable, tool_path, "--kconfig", str(Path(SDK_PATH)/'Kconfig')]
    for path in [str(Path(PROJECT_PATH)/'config_defaults.mk')]:
        cmd.extend(["--defaults", path])
    cmd.extend(["--env", "SDK_PATH={}".format(SDK_PATH),
                                        "--env", "PROJECT_PATH={}".format(PROJECT_PATH),
                                        "--env", "BUILD_TYPE={}".format(build_type)])
    cmd.extend(["--output", "makefile", str(Path(PROJECT_PATH)/'build'/'config'/'global_config.mk')])
    cmd.extend(["--output", "header", str(Path(PROJECT_PATH)/'build'/'config'/'global_config.h')])
    res = subprocess.call(cmd)

with open(str(Path(PROJECT_PATH)/'build'/'config'/'global_config.mk'), 'r') as f:
    for line in f.readlines():
        if not line.startswith('#') and line.strip():
            tmpstr = line.split('=')
            os.environ[tmpstr[0]] = tmpstr[1].replace(
                '\n', '').replace('""', '')

# Set the compiler and flags (modify as needed)
# env = Environment(platform='posix')
# env = Environment(tools=['gcc','link', 'g++', ''])


if sys.platform.startswith('win'):
    env = Environment(tools=['gcc', 'link', 'ar', 'g++', 'as'])
    if os.environ.get('CONFIG_TOOLCHAIN_PREFIX') is not None:
        if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
            env['CC'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc.exe')) 
            env['AS'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as.exe' ))
            env['AR'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar.exe' ))
            env['CXX'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"')) / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
            env['LINK'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))/ str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
            env['PROGSUFFIX'] = ''
        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + env['ENV']['PATH']
        
elif sys.platform.startswith('linux'):
    env = Environment()
    if os.environ.get('CONFIG_TOOLCHAIN_PREFIX') is not None:
        if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
            env['CC'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc'
            env['AS'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as'
            env['AR'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar'
            env['CXX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++'
            env['LINK'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++'

        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip(
                '"') + ':' + env['ENV']['PATH']
else:
    print('unknow os!')

env.Append(CPPPATH=[str(Path(PROJECT_PATH)/'build'/'config')])

# print(env['ENV']['PATH'])



env['PROJECT_PATH'] = PROJECT_PATH
env['PROJECT_NAME'] = os.path.basename(PROJECT_PATH)
env['PROJECT_TOOL_S'] = str(Path(SDK_PATH)/'tools'/'scons'/'SConstruct_tool.py')

env['COMPONENTS'] = []
env['COMPONENTS_ENV'] = env.Clone()
env['COMPONENTS_PATH'] = [str(Path(SDK_PATH)/'components')]
# env['COMPONENTS_PATH'] = []



for component in env['COMPONENTS_PATH']:
    for component_name in os.listdir(component):
        if os.path.exists(str(Path(component)/component_name/'SConscript')):
            env['component_dir'] = str(Path(component)/component_name)
            SConscript(str(Path(component)/component_name/'SConscript'), exports='env')



for project_dir in os.listdir(PROJECT_PATH):
    if project_dir.startswith("main"):
        env['component_dir'] = str(Path(PROJECT_PATH)/project_dir)
        SConscript(str(Path(PROJECT_PATH)/project_dir/'SConscript'), exports='env')

task_lists = {}
for iteam in env['COMPONENTS']:
    task_lists[iteam['target']] = iteam

# print(task_lists)
import asyncio


async def _commpile(task):
    global task_lists
    global env
    for requirement in task['REQUIREMENTS']:
        if requirement in task_lists:
            await task_lists[requirement]['event'].wait()
    
    mkasdad = copy.deepcopy(task['REQUIREMENTS'])

    for requirement in mkasdad:
        if requirement in task_lists:
            task_lists[task['target']]['PRIVATE_INCLUDE'] += task_lists[requirement]['INCLUDE']
            task_lists[task['target']]['REQUIREMENTS'] += task_lists[requirement]['REQUIREMENTS']
            task_lists[task['target']]['LINK_SEARCH_PATH'] += task_lists[requirement]['LINK_SEARCH_PATH']
            task_lists[task['target']]['DEFINITIONS'] += task_lists[requirement]['DEFINITIONS']
            task_lists[task['target']]['LINK_SEARCH_PATH'].append(os.path.dirname(str(task_lists[requirement]['COMPONENT'][0])))

    if not os.path.exists('build'):
        os.mkdir('build')

    build_env = env.Clone()
    OBJS = []
    # print(task_lists[task['target']]['REQUIREMENTS'] + task_lists[task['target']]['STATIC_LIB'] + task_lists[task['target']]['DYNAMIC_LIB'])

    reduse = lambda value: sorted(set(value),key=value.index)
    _LIBS = reduse(task_lists[task['target']]['REQUIREMENTS'] + task_lists[task['target']]['STATIC_LIB'] + task_lists[task['target']]['DYNAMIC_LIB'])
    _LIBPATH = reduse(task_lists[task['target']]['LINK_SEARCH_PATH'])
    _CCFLAGS = reduse(task_lists[task['target']]['DEFINITIONS_PRIVATE'] + task_lists[task['target']]['DEFINITIONS'])
    _CPPPATH = reduse(task_lists[task['target']]['INCLUDE'] + task_lists[task['target']]['PRIVATE_INCLUDE'])
    _LINKFLAGS = reduse(task_lists[task['target']]['LDFLAGS'])
    
    
    # _LIBS = ((task_lists[task['target']]['REQUIREMENTS'] + task_lists[task['target']]['STATIC_LIB'] + task_lists[task['target']]['DYNAMIC_LIB']))
    # _LIBPATH = ((task_lists[task['target']]['LINK_SEARCH_PATH']))
    # _CCFLAGS = ((task_lists[task['target']]['DEFINITIONS_PRIVATE'] + task_lists[task['target']]['DEFINITIONS']))
    # _CPPPATH = ((task_lists[task['target']]['INCLUDE'] + task_lists[task['target']]['PRIVATE_INCLUDE']))
    # _LINKFLAGS = ((task_lists[task['target']]['LDFLAGS']))
    build_env.Append(LIBS=_LIBS)
    build_env.Append(LIBPATH=_LIBPATH)
    build_env.MergeFlags(_CCFLAGS)
    build_env.Append(CPPPATH=_CPPPATH)
    build_env.Append(LINKFLAGS=_LINKFLAGS)


    if task['REGISTER'] == 'project':
        component_dir = 'main_' + task['target']
        component_dir = str(Path('build')/component_dir)
        if not os.path.exists(component_dir):
            os.mkdir(component_dir)
        
        for sfile in task_lists[task['target']]['SRCS']:
            if isinstance(sfile, str) or isinstance(sfile, File):
                file = str(sfile)
            elif isinstance(sfile, list):
                file = str(sfile[0])
            ofile = file.replace('/', '_')
            ofile = ofile.replace('\\', '_')
            ofile = ofile.replace(':', '_')
            
            ofile = str(Path(component_dir)/ofile) + '.o'
            OBJS.append(build_env.Object(target = ofile, source = file))

        component=build_env.Program(target=str(Path('build')/task['target']), source=OBJS)
        # print(build_env.Dump())
        task_lists[task['target']]['COMPONENT'] = component
        build_env.Command(os.path.join('dist', task['target']), str(Path('build')/task['target']), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])

        def find_file_in_directories(directories, target_file):
            for directory in directories:
                result = search_file_recursive(directory, target_file)
                if result:
                    return result
            return None

        def search_file_recursive(directory, target_file):
            for root, dirs, files in os.walk(directory):
                if target_file in files:
                    return os.path.join(root, target_file)
            return None
        for obj in _LIBS:
            if '.so' in obj:
                result = find_file_in_directories(_LIBPATH, obj)
                if result:
                    build_env.Command(result, str(Path('build')/os.path.basename(result)), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])

    elif task['REGISTER'] == 'static':
        component_dir = task['target']
        component_dir = str(Path('build')/component_dir)
        if not os.path.exists(component_dir):
            os.mkdir(component_dir)
        
        for sfile in task_lists[task['target']]['SRCS']:
            if isinstance(sfile, str) or isinstance(sfile, File):
                file = str(sfile)
            elif isinstance(sfile, list):
                file = str(sfile[0])
            ofile = file.replace('/', '_')
            ofile = ofile.replace('\\', '_')
            ofile = ofile.replace(':', '_')
            
            ofile = str(Path(component_dir)/ofile) + '.o'
            OBJS.append(build_env.Object(target = ofile, source = file))
        component=build_env.Library(target=str(Path('build')/task['target']/task['target']), source=OBJS)

    elif task['REGISTER'] == 'shared':
        component = None
    
    task_lists[task['target']]['COMPONENT'] = component
    task_lists[task['target']]['event'].set()

    requirement_lists = []
    for requirement in mkasdad:
        if requirement in task_lists:
            requirement_lists.append(task_lists[requirement]['COMPONENT'])
    if requirement_lists:
        Depends(component, requirement_lists)

async def main():
    global task_lists

    for iteam in task_lists:
        task_lists[iteam]['event'] = asyncio.Event()

    tasks = []
    for iteam in task_lists:
        tasks.append(_commpile(task_lists[iteam]))

    await asyncio.gather(*tasks)

asyncio.run(main())



















