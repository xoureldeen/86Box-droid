#
# 86Box    A hypervisor and IBM PC system emulator that specializes in
#          running old operating systems and software designed for IBM
#          PC systems and compatibles from 1981 through fairly recent
#          system designs based on the PCI bus.
#
#          This file is part of the 86Box distribution.
#
#          CMake build script.
#
# Authors: David Hrdlička, <hrdlickadavid@outlook.com>
#          dob205
#
#          Copyright 2020-2022 David Hrdlička.
#          Copyright 2021      dob205.
#          Copyright 2024      Jasmine Iwanek.
#

add_library(86Box SHARED
        86box.c
        config.c
        log.c
        random.c
        timer.c
        io.c
        acpi.c
        apm.c
        dma.c
        ddma.c
        nmi.c
        pic.c
        pit.c
        pit_fast.c
        port_6x.c
        port_92.c
        ppi.c
        pci.c
        mca.c
        usb.c
        fifo.c
        fifo8.c
        device.c
        nvr.c
        nvr_at.c
        nvr_ps2.c
        machine_status.c
        ini.c
        cJSON.c
        )

if(CPPTHREADS)
    target_sources(86Box PRIVATE thread.cpp)
endif()

if(GDBSTUB)
    add_compile_definitions(USE_GDBSTUB)
    target_sources(86Box PRIVATE gdbstub.c)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_compile_definitions(_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE=1 _LARGEFILE64_SOURCE=1)
endif()

if(NEW_DYNAREC)
    add_compile_definitions(USE_NEW_DYNAREC)
endif()

if(RELEASE)
    add_compile_definitions(RELEASE_BUILD)
endif()

if(DYNAREC)
    add_compile_definitions(USE_DYNAREC)
endif()

if(DISCORD)
    add_compile_definitions(DISCORD)
    target_sources(86Box PRIVATE discord.c)
endif()

if(DEBUGREGS486)
    add_compile_definitions(USE_DEBUG_REGS_486)
endif()

if(VNC)
    find_package(LibVNCServer)
    if(LibVNCServer_FOUND)
        add_compile_definitions(USE_VNC)
        add_library(vnc OBJECT
                vnc.c
                vnc_keymap.c
                )
        target_link_libraries(86Box vnc LibVNCServer::vncserver)
        if(WIN32)
            target_link_libraries(86Box ws2_32)
        endif()
    endif()
endif()

if(INSTRUMENT)
    add_compile_definitions(USE_INSTRUMENT)
endif()

target_link_libraries(86Box cpu chipset mch dev mem fdd game cdrom zip mo hdd
        net print scsi sio snd vid voodoo plat ui)

if(STATIC_BUILD)
    if(MINGW OR UNIX)
        target_link_options(86Box PRIVATE "-static")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()
endif()

include_directories(${FREETYPE_INCLUDE_DIR_ft2build})

configure_file(include/86box/version.h.in include/86box/version.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(include)
if(NEW_DYNAREC)
    include_directories(cpu codegen_new)
elseif(DYNAREC)
    include_directories(cpu codegen)
else()
    include_directories(cpu)
endif()

add_subdirectory(cdrom)
add_subdirectory(chipset)

add_subdirectory(cpu)
if(NEW_DYNAREC)
    add_subdirectory(codegen_new)
elseif(DYNAREC)
    add_subdirectory(codegen)
endif()

add_subdirectory(device)
add_subdirectory(disk)
add_subdirectory(floppy)
add_subdirectory(game)
add_subdirectory(machine)
add_subdirectory(mem)
add_subdirectory(network)
add_subdirectory(printer)
add_subdirectory(sio)
add_subdirectory(scsi)
add_subdirectory(sound)
add_subdirectory(video)

add_compile_definitions(USE_SDL_UI)
add_subdirectory(unix)
