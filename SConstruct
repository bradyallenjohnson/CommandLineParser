
env = Environment()
ccflags = [
  '-O2',
#  '-g'
]
env.Append(CPPPATH = '#', CCFLAGS = ccflags)
#env.SharedLibrary(target = 'CmdLineParser', source = 'CmdLineParser.cc')
libTarget = env.StaticLibrary(target = 'CmdLineParser', source = 'CmdLineParser.cc')
env.Default(libTarget)
env.Alias('library', libTarget)

env.Append(LIBS = 'CmdLineParser', LIBPATH = '#')
binTarget = env.Program(target = 'exampleSkeletonMain', source = 'exampleSkeletonMain.cc')
env.Alias('example', binTarget)
