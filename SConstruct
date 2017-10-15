#!python
import os, subprocess

# Local dependency paths, adapt them to your setup
#godot_headers_path = ARGUMENTS.get("headers", "godot_headers/")
godot_headers_path = ARGUMENTS.get("headers", "../../godot3-git/modules/gdnative/include")

target = ARGUMENTS.get("target", "debug")

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
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])

if platform == "linux":
    platform_dir = 'linux'
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++14'])
    env.Append(CXXFLAGS='-std=c++0x')

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

env.Append(CPPPATH=['openvr/headers/'])
env.Append(LIBPATH=['openvr/lib/' + platform_dir])

if (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LINKFLAGS=['openvr_api.lib'])
else:
    env.Append(LIBS=['openvr_api'])

# need to add copying the correct file from 'openvr/bin/' + platform_bin to demo/bin/
# for now manually copy the files

# and our stuff
env.Append(CPPPATH=['.', godot_headers_path])

sources = []
add_sources(sources, "src")

library = env.SharedLibrary(target='demo/bin/godot_openvr', source=sources)
Default(library)
