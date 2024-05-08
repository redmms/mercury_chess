TEMPLATE=lib
CONFIG += staticlib

DSTDIR_DEBUG=debug
DSTDIR_RELEASE=release
MODULES_DEBUG_FLAGS += /MDd /ifcOutput $$DSTDIR_DEBUG /ifcSearchDir $$DSTDIR_DEBUG
MODULES_RELEASE_FLAGS = /MD /DNDEBUG /ifcOutput $$DSTDIR_RELEASE /ifcSearchDir $$DSTDIR_RELEASE
MODULES_COMMON_FLAGS = /std:c++20 /EHsc /DUNICODE /D_UNICODE /DWIN32 /DWIN64 /D_ENABLE_EXTENDED_ALIGNED_STORAGE

include(stdheaders.pro)
for(hdr, STD_HEADERS) {
    CONFIG(debug) {
        STD_HEADER_TARGETS_DEBUG += mod_$${hdr}_debug
        mod_$${hdr}_debug.target = $${DSTDIR_DEBUG}/$${hdr}.ifc
        mod_$${hdr}_debug.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS $$MODULES_DEBUG_FLAGS /exportHeader /headerName:angle $$hdr
        QMAKE_EXTRA_TARGETS += mod_$${hdr}_debug
    }
    CONFIG(release) {
        STD_HEADER_TARGETS_RELEASE += mod_$${hdr}_release
        mod_$${hdr}_release.target = $${DSTDIR_RELEASE}/$${hdr}.ifc
        mod_$${hdr}_release.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS $$MODULES_RELEASE_FLAGS /exportHeader /headerName:angle $$hdr
        QMAKE_EXTRA_TARGETS += mod_$${hdr}_release
    }
}

FINESTREAM_MODULES = bitremedy finestream
for(mod, FINESTREAM_MODULES) {
    CONFIG(debug) {
        mod_$${mod}_debug.target = $${DSTDIR_DEBUG}/$${mod}.ifc
        mod_$${mod}_debug.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS $$MODULES_DEBUG_FLAGS $$STD_HEADER_UNITS /c $$PWD/archiver/finestream/modules/$${mod}.ixx /Fo$${DSTDIR_DEBUG}/
        mod_$${mod}_debug.depends = $$STD_HEADER_TARGETS_DEBUG
        QMAKE_EXTRA_TARGETS += mod_$${mod}_debug
        PRE_TARGETDEPS += $${DSTDIR_DEBUG}/$${mod}.ifc
    }
    CONFIG(release) {
        mod_$${mod}_release.target = $${DSTDIR_RELEASE}/$${mod}.ifc
        mod_$${mod}_release.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS $$MODULES_RELEASE_FLAGS $$STD_HEADER_UNITS /c $$PWD/archiver/finestream/modules/$${mod}.ixx /Fo$${DSTDIR_RELEASE}/
        mod_$${mod}_release.depends = $$STD_HEADER_TARGETS_RELEASE
        QMAKE_EXTRA_TARGETS += mod_$${mod}_release
        PRE_TARGETDEPS += $${DSTDIR_RELEASE}/$${mod}.ifc
    }
}

mod_finestream_debug.depends += mod_bitremedy_debug
mod_finestream_release.depends += mod_bitremedy_release
