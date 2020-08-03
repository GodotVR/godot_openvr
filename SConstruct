#!python
import os
import sys

# Reads variables from an optional file.
customs = ['../custom.py']
opts = Variables(customs, ARGUMENTS)

# Gets the standart flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our parameters
opts.Add(EnumVariable('target', "Compilation target", 'release', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", 'windows', ['windows', 'x11', 'linux', 'osx']))
opts.AddVariables(
    PathVariable('openvr_path', 'The path where the OpenVR repo is located.', 'openvr/'),
    PathVariable('target_path', 'The path where the lib is installed.', 'demo/addons/godot-openvr/bin/'),
    PathVariable('target_name', 'The library name.', 'libgodot_openvr', PathVariable.PathAccept),
)
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))
opts.Add(EnumVariable('bits', "CPU architecture", '64', ['32', '64']))

# Other needed paths
godot_headers_path = "godot-cpp/godot_headers/"
godot_cpp_path = "godot-cpp/"
godot_cpp_library = "libgodot-cpp"

# Updates the environment with the option variables.
opts.Update(env)

# Check some environment settings
if env['use_llvm']:
    env['CXX'] = 'clang++'

# Try to detect the host platform automatically.
# This is used if no `platform` argument is passed
if sys.platform.startswith('linux'):
    host_platform = 'linux'
elif sys.platform == 'darwin':
    host_platform = 'osx'
elif sys.platform == 'win32' or sys.platform == 'msys':
    host_platform = 'windows'
else:
    raise ValueError(
        'Could not detect platform automatically, please specify with '
        'platform=<platform>'
    )

# fix needed on OSX
def rpath_fix(target, source, env):
    os.system('install_name_tool -id @rpath/libgodot_openvr.dylib {0}'.format(target[0]))
    os.system('install_name_tool -change @rpath/OpenVR.framework/Versions/A/OpenVR @loader_path/OpenVR.framework/Versions/A/OpenVR {0}'.format(target[0]))

# platform dir for openvr libraries
platform_dir = ''

# Setup everything for our platform
if env['platform'] == 'windows':
    env['target_path'] += 'win' + env['bits'] + '/'
    godot_cpp_library += '.windows'
    platform_dir = 'win'
    if not env['use_llvm']:
        if host_platform == 'windows':
            # This makes sure to keep the session environment variables on windows,
            # that way you can run scons in a vs 2017 prompt and it will find all the required tools
            env.Append(ENV = os.environ)
    
            env.Append(CCFLAGS = ['-DWIN32', '-D_WIN32', '-D_WINDOWS', '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS'])
            if env['target'] in ('debug', 'd'):
                env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '-MDd'])
            else:
                env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD'])
        elif host_platform == 'linux' or host_platform == 'osx':
            # Cross-compilation using MinGW
            if env['bits'] == '64':
                env['CXX'] = 'x86_64-w64-mingw32-g++'
                env['AR'] = "x86_64-w64-mingw32-ar"
                env['RANLIB'] = "x86_64-w64-mingw32-ranlib"
                env['LINK'] = "x86_64-w64-mingw32-g++"
            elif env['bits'] == '32':
                env['CXX'] = 'i686-w64-mingw32-g++'
                env['AR'] = "i686-w64-mingw32-ar"
                env['RANLIB'] = "i686-w64-mingw32-ranlib"
                env['LINK'] = "i686-w64-mingw32-g++"
            env.Append(CCFLAGS=['-g', '-O3', '-std=c++14', '-Wwrite-strings'])
            env.Append(LINKFLAGS=[
                '--static',
                '-Wl,--no-undefined',
                '-static-libgcc',
                '-static-libstdc++',
            ])
    
    # untested
    else:
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
        else:
            env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])

# untested
elif env['platform'] == 'osx':
    env['target_path'] += 'osx/'
    godot_cpp_library += '.osx'
    platform_dir = 'osx'
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-g','-O2', '-arch', 'x86_64'])
    else:
        env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64'])
    env.Append(CXXFLAGS='-std=c++11')
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])

elif env['platform'] in ('x11', 'linux'):
    env['target_path'] += 'x11/'
    godot_cpp_library += '.linux'
    platform_dir = 'linux'
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
    else:
        env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

# Complete godot-cpp library path
if env['target'] in ('debug', 'd'):
    godot_cpp_library += '.debug'
else:
    godot_cpp_library += '.release'

godot_cpp_library += '.' + str(env['bits'])

# Update our include search path 
env.Append(CPPPATH=[
    '.', 
    'src/', 
    'src/open_vr/', 
    godot_headers_path,
    godot_cpp_path + 'include/',
    godot_cpp_path + 'include/core/',
    godot_cpp_path + 'include/gen/',
    env['openvr_path'] + 'headers/'])

# Add our godot-cpp library
env.Append(LIBPATH=[godot_cpp_path + 'bin/'])
env.Append(LIBS=[godot_cpp_library])

# Add our openvr library
platform_dir += str(env['bits'])

if (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LINKFLAGS=['openvr_api.lib'])
elif env['platform'] == "osx":
    env.Append(LINKFLAGS = ['-F' + env['openvr_path'] + 'bin/osx64', '-framework', 'OpenVR'])
else:
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LIBS=['openvr_api'])

# Add our sources
sources = Glob('src/*.c')
sources += Glob('src/*.cpp')
sources += Glob('src/*/*.c')
sources += Glob('src/*/*.cpp')

# Build our library
library = env.SharedLibrary(target=env['target_path'] + env['target_name'], source=sources)
if env['platform'] == "osx":
    env.AddPostAction(library, rpath_fix)

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
