TEMPLATE=lib
CONFIG += staticlib
#QMAKE_LFLAGS_WINDOWS += /NODEFAULTLIB:lib
#CONFIG += dynamiclib

#MODULES_COMMON_FLAGS += /MDd

#CONFIG(debug) {
#    MODULES_COMMON_FLAGS += /MDd
#}
#else {
#    MODULES_COMMON_FLAGS += /MD
#}

#debug{
#    MODULES_COMMON_FLAGS += /MDd
#}
#!debug{
    MODULES_COMMON_FLAGS += /MD
#}

MODULES_COMMON_FLAGS += /std:c++20 /EHsc /DUNICODE /D_UNICODE /DWIN32 /DWIN64 /D_ENABLE_EXTENDED_ALIGNED_STORAGE

include(stdheaders.pro)
for(hdr, STD_HEADERS) {
    STD_HEADER_TARGETS += mod_$${hdr}
    mod_$${hdr}.target = $${hdr}.ifc
    mod_$${hdr}.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS /exportHeader /headerName:angle $$hdr
    QMAKE_EXTRA_TARGETS += mod_$${hdr}
}

FINESTREAM_MODULES = bitremedy finestream
for(mod, FINESTREAM_MODULES) {
    mod_$${mod}.target = $${mod}.ifc
    mod_$${mod}.commands = $$QMAKE_CXX $$MODULES_COMMON_FLAGS $$STD_HEADER_UNITS /c $$PWD/archiver/finestream/modules/$${mod}.ixx
    mod_$${mod}.depends = $$STD_HEADER_TARGETS
    QMAKE_EXTRA_TARGETS += mod_$${mod}
    PRE_TARGETDEPS += $${mod}.ifc
}

mod_finestream.depends += mod_bitremedy
