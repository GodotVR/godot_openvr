#!python
import os, subprocess

def rpath_fix(target, source, env):
    os.system('install_name_tool -id @rpath/libgodot_openvr.dylib {0}'.format(target[0]))
    os.system('install_name_tool -change @rpath/OpenVR.framework/Versions/A/OpenVR @loader_path/OpenVR.framework/Versions/A/OpenVR {0}'.format(target[0]))

# Local dependency paths
godot_headers_path = ARGUMENTS.get("headers", os.getenv("GODOT_HEADERS", "godot_headers/"))
openvr_path = ARGUMENTS.get("openvr", os.getenv("OPENVR_PATH", "openvr/"))

# default to release build, add target=debug to build debug build
target = ARGUMENTS.get("target", "release")

# platform= makes it in line with Godots scons file, keeping p for backwards compatibility
platform = ARGUMENTS.get("p", "linux")
platform = ARGUMENTS.get("platform", platform)

# This makes sure to keep the session environment variables on windows, 
# that way you can run scons in a vs 2017 prompt and it will find all the required tools
env = Environment()
if platform == "windows":
    env = Environment(ENV = os.environ)

if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

def add_sources(sources, directory):
    for file in os.listdir(directory):
        if file.endswith('.c'):
            sources.append(directory + '/' + file)
        elif file.endswith('.cpp'):
            sources.append(directory + '/' + file)

platform_dir = ''
if platform == "osx":
    platform_dir = 'osx'
    env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64'])
    env.Append(CXXFLAGS='-std=c++11')
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])

if platform == "linux":
    platform_dir = 'linux'
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++14'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

if platform == "windows":
    platform_dir = 'win'
    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD'])

# add our openvr library
if 'bits' in env:
    platform_dir += env['bits']
else:
    platform_dir += '64'

env.Append(CPPPATH=[openvr_path + 'headers/'])

if (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LIBPATH=[openvr_path + 'lib/' + platform_dir])
    env.Append(LINKFLAGS=['openvr_api.lib'])
else:
    if platform == "osx":
        env.Append(LINKFLAGS = ['-F' + openvr_path + 'bin/osx64', '-framework', 'OpenVR'])
    else:
        env.Append(LIBPATH=[openvr_path + 'lib/' + platform_dir])
        env.Append(LIBS=['openvr_api'])

# need to add copying the correct file from 'openvr/bin/' + platform_bin to demo/bin/
# for now manually copy the files

# and our stuff
env.Append(CPPPATH=['.', godot_headers_path])

sources = []
add_sources(sources, "src")

library = env.SharedLibrary(target='demo/bin/godot_openvr', source=sources)
if platform == "osx":
    env.AddPostAction(library, rpath_fix)

Default(library)
