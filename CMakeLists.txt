cmake_minimum_required(VERSION 3.7)

include(CMake/out_of_tree.cmake)

project(devilutionX
  VERSION 0.0.1
  LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ASAN "Enable address sanitizer" ON)
option(DEBUG "Enable debug mode in engine" ON)
option(COTIRE "Enable cotire" OFF)
option(DIST "Dynamically link only glibc and SDL2" OFF)
option(FASTER "Enable FASTER in engine" ON)
option(BINARY_RELEASE "Enable options for binary release" OFF)

if(BINARY_RELEASE)
  set(CMAKE_BUILD_TYPE "Release")
  set(ASAN OFF)
  set(DEBUG OFF)
  set(COTIRE OFF)
  set(DIST ON)
  set(FASTER OFF)
endif()

if(DIST)
  set(sodium_USE_STATIC_LIBS ON)
endif()

list(APPEND CMAKE_MODULE_PATH "${devilutionX_SOURCE_DIR}/CMake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(WARNING "sizeof(void*) == ${CMAKE_SIZEOF_VOID_P}.")
  message(WARNING [[Not 32-bit, highly experimental!]])
endif()

if(COTIRE)
  include(CMake/cotire.cmake)
endif()

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(sodium REQUIRED)

add_library(smacker STATIC
  3rdParty/libsmacker/smk_bitstream.c
  3rdParty/libsmacker/smk_hufftree.c
  3rdParty/libsmacker/smacker.c)

add_library(Radon STATIC
  3rdParty/Radon/Radon/source/File.cpp
  3rdParty/Radon/Radon/source/Key.cpp
  3rdParty/Radon/Radon/source/Named.cpp
  3rdParty/Radon/Radon/source/Section.cpp)

add_library(StormLib STATIC
  3rdParty/StormLib/src/FileStream.cpp
  3rdParty/StormLib/src/SBaseCommon.cpp
  3rdParty/StormLib/src/SBaseFileTable.cpp
  3rdParty/StormLib/src/SBaseSubTypes.cpp
  3rdParty/StormLib/src/SCompression.cpp
  3rdParty/StormLib/src/SFileExtractFile.cpp
  3rdParty/StormLib/src/SFileFindFile.cpp
  3rdParty/StormLib/src/SFileGetFileInfo.cpp
  3rdParty/StormLib/src/SFileOpenArchive.cpp
  3rdParty/StormLib/src/SFileOpenFileEx.cpp
  3rdParty/StormLib/src/SFileReadFile.cpp)

add_library(PKWare STATIC
  3rdParty/PKWare/explode.cpp
  3rdParty/PKWare/implode.cpp)

add_library(devilution STATIC
  Source/appfat.cpp
  Source/automap.cpp
  Source/capture.cpp
  Source/codec.cpp
  Source/control.cpp
  Source/cursor.cpp
  Source/dead.cpp
  Source/debug.cpp
  Source/diablo.cpp
  Source/doom.cpp
# Source/dx.cpp
  Source/drlg_l1.cpp
  Source/drlg_l2.cpp
  Source/drlg_l3.cpp
  Source/drlg_l4.cpp
  Source/dthread.cpp
  Source/effects.cpp
  Source/encrypt.cpp
  Source/engine.cpp
  Source/error.cpp
  Source/fault.cpp
  Source/gamemenu.cpp
  Source/gendung.cpp
  Source/gmenu.cpp
  Source/help.cpp
  Source/init.cpp
  Source/interfac.cpp
  Source/inv.cpp
  Source/items.cpp
  Source/lighting.cpp
  Source/loadsave.cpp
  Source/logging.cpp
  Source/mainmenu.cpp
  Source/minitext.cpp
  Source/missiles.cpp
  Source/monster.cpp
# Source/movie.cpp
  Source/mpqapi.cpp
  Source/msgcmd.cpp
  Source/msg.cpp
  Source/multi.cpp
  Source/nthread.cpp
  Source/objects.cpp
  Source/pack.cpp
  Source/palette.cpp
  Source/path.cpp
  Source/pfile.cpp
  Source/player.cpp
  Source/plrmsg.cpp
  Source/portal.cpp
  Source/quests.cpp
  Source/render.cpp
  Source/restrict.cpp
  Source/scrollrt.cpp
  Source/setmaps.cpp
  Source/sha.cpp
# Source/sound.cpp
  Source/spells.cpp
  Source/stores.cpp
  Source/sync.cpp
  Source/textdat.cpp
  Source/themes.cpp
  Source/tmsg.cpp
  Source/town.cpp
  Source/towners.cpp
  Source/track.cpp
  Source/trigs.cpp
  Source/wave.cpp)
target_include_directories(devilution PRIVATE SourceS)

add_executable(devilutionx
  SourceX/dx.cpp
  SourceX/miniwin.cpp
  SourceX/miniwin_io.cpp
  SourceX/miniwin_msg_sdl.cpp
  SourceX/stub_rand.cpp
  SourceX/miniwin_thread.cpp
  SourceX/movie.cpp
  SourceX/sound.cpp
  SourceX/storm.cpp
  SourceX/storm_net.cpp
  SourceX/dvlnet/abstract_net.cpp
  SourceX/dvlnet/loopback.cpp
  SourceX/dvlnet/packet.cpp
  SourceX/dvlnet/base.cpp
  SourceX/dvlnet/frame_queue.cpp
  SourceX/dvlnet/tcp_client.cpp
  SourceX/dvlnet/tcp_server.cpp
  SourceX/dvlnet/udp_p2p.cpp
  SourceX/DiabloUI/credits.cpp
  SourceX/DiabloUI/diabloui.cpp
  SourceX/DiabloUI/dialogs.cpp
  SourceX/DiabloUI/mainmenu.cpp
  SourceX/DiabloUI/progress.cpp
  SourceX/DiabloUI/selconn.cpp
  SourceX/DiabloUI/selgame.cpp
  SourceX/DiabloUI/selhero.cpp
  SourceX/DiabloUI/title.cpp
  SourceX/main.cpp)

target_include_directories(devilutionx PRIVATE SourceS SourceX 3rdParty/asio/include .)

target_link_libraries(devilution PUBLIC PKWare Threads::Threads)

target_link_libraries(devilutionx PRIVATE
  devilution
  StormLib
  smacker
  Radon
  Threads::Threads
  SDL2::SDL2main
  SDL2::SDL2_ttf
  SDL2::SDL2_mixer
  sodium)

target_compile_definitions(devilution PUBLIC
  DEVILUTION_STUB
  "$<$<BOOL:${DEBUG}>:_DEBUG>"
  # Skip fades and other fluff
  "$<$<BOOL:${FASTER}>:FASTER>")

target_compile_definitions(devilutionx PUBLIC ASIO_STANDALONE)

# Enable ASAN in Debug mode
if(ASAN)
  target_compile_options(devilution PUBLIC -fsanitize=address -fsanitize-recover=address)
  target_link_libraries(devilution PUBLIC -fsanitize=address -fsanitize-recover=address)
endif()

if(DIST)
  target_link_libraries(devilutionx PUBLIC -static-libgcc -static-libstdc++)
endif()

# Note: In Debug mode, GCC generates spurious memory references that upset Valgrind,
# these options fix that.
target_compile_options(devilution PUBLIC $<$<CONFIG:Debug>:-fno-omit-frame-pointer>)

# Ignore serious warnings due to "quality" of decompiled code
# Currently, disable ignore all warnings (-w), to be removed later
target_compile_options(devilution PRIVATE -fpermissive -Wno-write-strings -Wno-multichar -w)
# Disable optimizations that can break non-standards-conforming code
target_compile_options(devilution PRIVATE -fno-strict-aliasing)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(devilution PRIVATE -fno-aggressive-loop-optimizations)
endif()
# Warnings for devilutionX
target_compile_options(devilution PUBLIC -Wno-unknown-pragmas)
target_compile_options(devilutionx PRIVATE -fpermissive -Wall -Wextra -Wno-multichar -Wno-unused-parameter)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Style issues
  target_compile_options(devilutionx PRIVATE -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses)
  # Silence warnings about __int64 alignment hack not always being applicable
  target_compile_options(devilutionx PRIVATE -Wno-ignored-attributes)
  # Fix: error: cast from pointer to smaller type 'unsigned char' loses information
  target_compile_options(devilutionx PRIVATE -fms-extensions)
endif()

if(COTIRE)
  set_target_properties(devilutionx PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
  set_target_properties(devilutionx PROPERTIES
    COTIRE_PREFIX_HEADER_INCLUDE_PATH "${devilutionX_SOURCE_DIR}"
    COTIRE_PREFIX_HEADER_IGNORE_PATH "${devilutionX_SOURCE_DIR}/SourceX")
  cotire(devilutionx)
endif()
