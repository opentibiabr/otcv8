
project(otclient)

# *****************************************************************************
# Options
# *****************************************************************************
option(TOGGLE_FRAMEWORK_SOUND "Use SOUND " ON)
option(TOGGLE_FRAMEWORK_XML "Use XML " ON)
option(TOGGLE_FRAMEWORK_NET "Use NET " ON)
option(TOGGLE_FRAMEWORK_EDITOR "Use Editor " OFF)
option(TOGGLE_DIRECTX "Use DX9 support" OFF)
option(TOGGLE_BIN_FOLDER "Use build/bin folder for generate compilation files" OFF)
option(TOGGLE_BOT_PROTECTION "Use bot protection" ON)
option(TOGGLE_PRE_COMPILED_HEADER "Use precompiled header (speed up compile)" OFF)
option(DEBUG_LOG "Enable Debug Log" OFF)
option(ASAN_ENABLED "Build this target with AddressSanitizer" OFF)
option(BUILD_STATIC_LIBRARY "Build using static libraries" OFF)

# *****************************************************************************
# Cmake Features
# *****************************************************************************
set(GNUCXX_MINIMUM_VERSION 9)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Make will print more details
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# *****************************************************************************
# Options Code
# *****************************************************************************

# Define Framework options for use in compilation
set(FRAMEWORK_DEFINITIONS "")
if (TOGGLE_FRAMEWORK_SOUND)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_SOUND)
endif()
if (TOGGLE_FRAMEWORK_XML)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_XML)
endif()
if (TOGGLE_FRAMEWORK_NET)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_NET)
endif()
if (TOGGLE_FRAMEWORK_EDITOR)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_EDITOR)
endif()

# Set for use bot protection
if(TOGGLE_BOT_PROTECTION)
	message(STATUS "Bot protection: ON")
else(TOGGLE_BOT_PROTECTION)
	message(STATUS "Bot protection: OFF")
endif(TOGGLE_BOT_PROTECTION)


# Set for use precompiled header
if(TOGGLE_PRE_COMPILED_HEADER)
	include(cotire)
	cotire(${PROJECT_NAME})
	message(STATUS "Use precompiled header: ON")
else()
	message(STATUS "Use precompiled header: OFF")
endif(TOGGLE_PRE_COMPILED_HEADER)


set(VERSION "1.0.0")

# *****************************************************************************
# Client
# *****************************************************************************
if (MSVC)
	add_executable(${PROJECT_NAME} "" ../cmake/icon/otcicon.rc)
else()
	add_executable(${PROJECT_NAME} "")
endif()

# *****************************************************************************
# Build flags
# *****************************************************************************
if (NOT MSVC)
	if (CMAKE_COMPILER_IS_GNUCXX)
		target_compile_options(${PROJECT_NAME}  PRIVATE  -Wno-deprecated-declarations)
	endif()
endif()

if(THREADS_HAVE_PTHREAD_ARG)
	target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DNDEBUG)
endif()

# *****************************************************************************
# Definitions code
# *****************************************************************************


# === ASAN ===
if(ASAN_ENABLED)
  log_option_enabled("asan")

  if(MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC /fsanitize=address)
  else()
	target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
	target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
  endif()
else()
  log_option_disabled("asan")
endif()


# === DEBUG LOG ===
# cmake -DDEBUG_LOG=ON ..
if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG_LOG=ON )
  log_option_enabled("DEBUG LOG")
  else()
  log_option_disabled("DEBUG LOG")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# *****************************************************************************
# Sanity Check
# *****************************************************************************

# === GCC Minimum Version ===
if (CMAKE_COMPILER_IS_GNUCXX)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS GNUCXX_MINIMUM_VERSION)
		message(FATAL_ERROR "GCC version must be at least ${GNUCXX_MINIMUM_VERSION}!")
	endif()
endif()


# Multiprocessor compiller
if(CMake_MSVC_PARALLEL)
	if(CMake_MSVC_PARALLEL GREATER 0)
		add_definitions(/MP${CMake_MSVC_PARALLEL})
	else()
		add_definitions(/MP)
	endif()
endif()


# Client options
add_definitions(-DCLIENT)
# Set bot protection
if(TOGGLE_BOT_PROTECTION)
	add_definitions(-DBOT_PROTECTION)
endif(TOGGLE_BOT_PROTECTION)
add_definitions(-D_WIN32_WINNT=0x0501)
add_definitions(${FRAMEWORK_DEFINITIONS})
add_definitions(-D"VERSION=${VERSION}")

# === Build options ===
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
add_definitions(-D"BUILD_TYPE=\\\"${CMAKE_BUILD_TYPE}\\\"")
message(STATUS "Build commit: ${BUILD_COMMIT}")
add_definitions(-D"BUILD_COMMIT=\\\"${BUILD_COMMIT}\\\"")
message(STATUS "Build revision: ${BUILD_REVISION}")
add_definitions(-D"BUILD_REVISION=\\\"${BUILD_REVISION}\\\"")

# *****************************************************************************
# Packages / Libs
# *****************************************************************************

find_package(LibZip REQUIRED)
find_package(ZLIB REQUIRED)

find_package(LuaJIT REQUIRED)

find_package(PhysFS REQUIRED)
find_package(OpenSSL QUIET)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(asio REQUIRED)
find_package(Threads REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(STDUUID CONFIG REQUIRED)

find_package(OpenAL CONFIG REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED)
if(UNIX AND NOT ANDROID)
	find_package(X11 REQUIRED)
endif()

# *****************************************************************************
# OTClient source files configuration
# *****************************************************************************
set(SOURCE_FILES
	main.cpp

	# client
    client/luafunctions_client.cpp
    client/client.cpp

    # core
    client/animatedtext.cpp
    client/animator.cpp
    client/container.cpp
    client/creature.cpp
    client/effect.cpp
    client/game.cpp
    client/item.cpp
    client/localplayer.cpp
    client/map.cpp
    client/mapio.cpp
    client/mapview.cpp
    client/minimap.cpp
    client/lightview.cpp
    client/missile.cpp
    client/outfit.cpp
    client/player.cpp
    client/spritemanager.cpp
    client/statictext.cpp
    client/thing.cpp
    client/thingtypemanager.cpp
    client/thingtype.cpp
    client/itemtype.cpp
    client/tile.cpp
    client/healthbars.cpp
    client/houses.cpp
    client/towns.cpp
    client/creatures.cpp

    # lua
    client/luavaluecasts_client.cpp

    # net
    client/protocolcodes.cpp
    client/protocolgame.cpp
    client/protocolgameparse.cpp
    client/protocolgamesend.cpp

    # ui
    client/uicreature.cpp
    client/uiitem.cpp
    client/uigraph.cpp
    client/uimap.cpp
    client/uiminimap.cpp
    client/uiprogressrect.cpp
    client/uimapanchorlayout.cpp
    client/uisprite.cpp

	framework/luafunctions.cpp

	framework/util/color.cpp
	framework/util/crypt.cpp
	framework/util/extras.cpp
	framework/util/pngunpacker.cpp
	framework/util/qrcodegen.c
	framework/util/stats.cpp

	# stdext
	framework/stdext/demangle.cpp
	framework/stdext/math.cpp
	framework/stdext/net.cpp
	framework/stdext/string.cpp
	framework/stdext/time.cpp
	framework/stdext/uri.cpp

	# core
	framework/core/adaptiverenderer.cpp
	framework/core/application.cpp
	framework/core/asyncdispatcher.cpp
	framework/core/binarytree.cpp
	framework/core/clock.cpp
	framework/core/config.cpp
	framework/core/configmanager.cpp
	framework/core/event.cpp
	framework/core/eventdispatcher.cpp
	framework/core/filestream.cpp
	framework/core/graphicalapplication.cpp
	framework/core/logger.cpp
	framework/core/module.cpp
	framework/core/modulemanager.cpp
	framework/core/resourcemanager.cpp
	framework/core/scheduledevent.cpp
	framework/core/timer.cpp

	# luaengine
	framework/luaengine/lbitlib.cpp
	framework/luaengine/luaexception.cpp
	framework/luaengine/luainterface.cpp
	framework/luaengine/luaobject.cpp
	framework/luaengine/luavaluecasts.cpp

	# otml
	framework/otml/otmldocument.cpp
	framework/otml/otmlemitter.cpp
	framework/otml/otmlexception.cpp
	framework/otml/otmlnode.cpp
	framework/otml/otmlparser.cpp

	# crash handler
	framework/platform/unixcrashhandler.cpp
	framework/platform/win32crashhandler.cpp
	framework/platform/win32platform.cpp
	framework/platform/unixplatform.cpp
	framework/platform/platform.cpp
	framework/graphics/animatedtexture.cpp
	framework/graphics/apngloader.cpp
	framework/graphics/atlas.cpp
	framework/graphics/bitmapfont.cpp
	framework/graphics/cachedtext.cpp
	framework/graphics/coordsbuffer.cpp
	framework/graphics/drawcache.cpp
	framework/graphics/drawqueue.cpp
	framework/graphics/fontmanager.cpp
	framework/graphics/framebuffer.cpp
	framework/graphics/framebuffermanager.cpp
	framework/graphics/graphics.cpp
	framework/graphics/graph.cpp
	framework/graphics/hardwarebuffer.cpp
	framework/graphics/image.cpp
	framework/graphics/painter.cpp
	framework/graphics/paintershaderprogram.cpp
	framework/graphics/shader.cpp
	framework/graphics/shaderprogram.cpp
	framework/graphics/shadermanager.cpp
	framework/graphics/textrender.cpp
	framework/graphics/texture.cpp
	framework/graphics/texturemanager.cpp
	# ui
	framework/ui/uianchorlayout.cpp
	framework/ui/uiboxlayout.cpp
	framework/ui/uigridlayout.cpp
	framework/ui/uihorizontallayout.cpp
	framework/ui/uilayout.cpp
	framework/ui/uimanager.cpp
	framework/ui/uitextedit.cpp
	framework/ui/uitranslator.cpp
	framework/ui/uiverticallayout.cpp
	framework/ui/uiwidget.cpp
	framework/ui/uiwidgetbasestyle.cpp
	framework/ui/uiwidgetimage.cpp
	framework/ui/uiwidgettext.cpp

	# platform window
	framework/platform/platformwindow.cpp
	framework/platform/win32window.cpp
	framework/platform/x11window.cpp
	framework/platform/sdlwindow.cpp

	# window input
	framework/input/mouse.cpp

	framework/core/graphicalapplication.cpp

	framework/sound/combinedsoundsource.cpp
	framework/sound/oggsoundfile.cpp
	framework/sound/soundbuffer.cpp
	framework/sound/soundchannel.cpp
	framework/sound/soundfile.cpp
	framework/sound/soundmanager.cpp
	framework/sound/soundsource.cpp
	framework/sound/streamsoundsource.cpp

	framework/net/connection.cpp
	framework/net/inputmessage.cpp
	framework/net/outputmessage.cpp
	framework/net/protocol.cpp
	framework/net/server.cpp
	framework/net/packet_player.cpp
	framework/net/packet_recorder.cpp
	framework/proxy/proxy.cpp
	framework/proxy/proxy_client.cpp
	framework/http/http.cpp

	framework/xml/tinyxml.cpp
	framework/xml/tinystr.cpp
	framework/xml/tinyxmlerror.cpp
	framework/xml/tinyxmlparser.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

# *****************************************************************************
# Includes and librarys
# *****************************************************************************

if(WIN32)
    find_library(IPHLPAPI_LIBRARY iphlpapi)
    find_library(PSAPI_LIBRARY psapi)
    find_library(BCRYPT_LIBRARY bcrypt)
    find_library(DBGHELP_LIBRARY dbghelp)
    find_library(SHLWAPI_LIBRARY shlwapi)
endif()

if(MSVC)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
	elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
	endif()

	if(BUILD_STATIC_LIBRARY)
		log_option_enabled("STATIC_LIBRARY")
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
		find_package(Vorbis REQUIRED)
		set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	else()
		log_option_disabled("STATIC_LIBRARY")
		target_compile_options(${PROJECT_NAME} PUBLIC /MP /FS /Zf /EHsc )
	endif()

	target_include_directories(${PROJECT_NAME}
		PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${LUAJIT_INCLUDE_DIR}
		${VORBISFILE_INCLUDE_DIR}
		${GMP_INCLUDE_DIR}
		${OPENSSL_INCLUDE_DIR}
		${PHYSFS_INCLUDE_DIR}
		${PARALLEL_HASHMAP_INCLUDE_DIRS}
		${NLOHMANN_JSON_INCLUDE_DIR}
	)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE
		${LUAJIT_LIBRARY}
		${CMAKE_THREAD_LIBS_INIT}
		${PHYSFS_LIBRARY}
		${ZLIB_LIBRARY}
		${NLOHMANN_JSON_LIBRARY}
		${OPENGL_LIBRARIES}
		${DirectX_LIBRARY}
		${DirectX_LIBRARIES}
		${VORBISFILE_LIBRARY}
		${VORBIS_LIBRARY}
		${OGG_LIBRARY}
		${GMP_LIBRARY}
		${OPENSSL_LIBRARIES}
		${STDUUID}

		${IPHLPAPI_LIBRARY}
		${PSAPI_LIBRARY}
		${BCRYPT_LIBRARY}
		${DBGHELP_LIBRARY}
		${SHLWAPI_LIBRARY}

		Threads::Threads
		GLEW::GLEW
		asio::asio
		OpenAL::OpenAL
		winmm.lib
		pugixml::pugixml
		libzip::zip
	)
else()
	target_include_directories(${PROJECT_NAME}
		PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${LUAJIT_INCLUDE_DIR}
		${CMAKE_THREAD_LIBS_INIT}
		${GMP_INCLUDE_DIR}
		${OPENSSL_INCLUDE_DIR}
		${PHYSFS_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
		${PARALLEL_HASHMAP_INCLUDE_DIRS}
		${NLOHMANN_JSON_INCLUDE_DIR}
	)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE
		${LUAJIT_LIBRARY}
		${PHYSFS_LIBRARY}
		${ZLIB_LIBRARY}
		${NLOHMANN_JSON_LIBRARY}
		${GLEW_LIBRARY}
		${OPENGL_LIBRARIES}
		${DirectX_LIBRARY}
		${DirectX_LIBRARIES}
		${OGG_LIBRARY}
		${GMP_LIBRARY}
		${OPENSSL_LIBRARIES}
		${STDUUID}
		${FOUNDATION}
		${IOKIT}

		Threads::Threads
		X11::X11
		asio::asio
		OpenAL::OpenAL
		Vorbis::vorbis
		Vorbis::vorbisfile
		pugixml::pugixml
		libzip::zip
	)
endif()

# *****************************************************************************
# Enable otclient console only for debug build and release deb info
# *****************************************************************************
if(WIN32)
	set_target_properties(${PROJECT_NAME}
	PROPERTIES
		LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_RELEASE "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
		LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
	)
endif(WIN32)

# *****************************************************************************
# Link compilation files folder
# *****************************************************************************

## Link compilation files to build/bin folder, else link to the main dir
if (TOGGLE_BIN_FOLDER)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)
else()
	set_target_properties(${PROJECT_NAME}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
	)
endif()
