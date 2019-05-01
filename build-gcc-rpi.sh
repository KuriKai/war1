#!/bin/sh

# Include paths
IncludePath="../deps/include"

# Lib paths
LibPath="../deps/lib/arm32"

# ProfilerFlags="-pg"
# OptimizeFlags="-O2"
# AssemblyFlags="-g -Wa,-ahl"
DebugFlags="-g -O1 -D __DEBUG__"
# PrintPathsFlags="-v"
CommonCompilerFlags="-std=c99 -Wall -Wno-misleading-indentation -x c $ProfilerFlags $OptimizeFlags $AssemblyFlags $DebugFlags $PrintPathsFlags -I $IncludePath"
CommonLinkerFlags="-L $LibPath -l glfw -l m"

mkdir -p build

cd build
rm -f war1

if gcc $CommonCompilerFlags $CommonLinkerFlags ../src/war1.c ../deps/include/glad/glad.c $GladSrcPath -o war1; then
cd ..
echo "Generating ctags - Start";
ctags --tag-relative --extra=f --language-force=C --recurse=yes
echo "Generating ctags - Done";
fi
