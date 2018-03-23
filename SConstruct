#!python
import os, subprocess

# Local dependency paths
godot_headers_path = ARGUMENTS.get("headers", os.getenv("GODOT_HEADERS", "godot_headers/"))
openvr_path = ARGUMENTS.get("openvr", os.getenv("OPENVR_PATH", "openvr/"))

# default to release build, add target=debug to build debug build
target = ARGUMENTS.get("target", "release")

# platform= makes it in line with Godots scons file, keeping p for backwards compatibility
platform = ARGUMENTS.get("p", "linux")
platform = ARGUMENTS.get("platform", platform)

# start building our destination path
godot_openvr_path = 'demo/addons/godot-openvr/bin/'

# This makes sure to keep the session environment variables on windows, 
# that way you can run scons in a vs 2017 prompt and it will find all the required tools
env = Environment()
if platform == "windows":
    env = Environment(ENV = os.environ)

# bits
bits = 64
if 'bits' in env:
    bits = env['bits']

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
    godot_openvr_path = godot_openvr_path + 'osx/'
    env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64'])
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])

if platform == "linux":
    platform_dir = 'linux'
    godot_openvr_path = godot_openvr_path + 'x11/'
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++14'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

if platform == "windows":
    platform_dir = 'win'
    godot_openvr_path = godot_openvr_path + 'win' + str(bits) + '/'
    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD'])

# add our openvr library
platform_dir += str(bits)

env.Append(CPPPATH=[openvr_path + 'headers/'])
env.Append(LIBPATH=[openvr_path + 'lib/' + platform_dir])

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

library = env.SharedLibrary(target=godot_openvr_path + 'godot_openvr', source=sources)
Default(library)
