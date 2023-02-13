file(GLOB MSPACK_SRC_FILES
	mspack/mspack.h 
    mspack/system.h 
    mspack/system.c 
    mspack/cab.h 
    mspack/cabc.c 
    mspack/cabd.c 
    mspack/chm.h 
    mspack/chmc.c 
    mspack/chmd.c 
    mspack/hlp.h 
    mspack/hlpc.c 
    mspack/hlpd.c 
    mspack/lit.h 
    mspack/litc.c 
    mspack/litd.c 
    mspack/kwaj.h 
    mspack/kwajc.c 
    mspack/kwajd.c 
    mspack/szdd.h 
    mspack/szddc.c 
    mspack/szddd.c 
    mspack/oab.h 
    mspack/oabc.c 
    mspack/oabd.c 
    mspack/lzx.h 
    mspack/lzxc.c 
    mspack/lzxd.c 
    mspack/mszip.h 
    mspack/mszipc.c 
    mspack/mszipd.c 
    mspack/qtm.h 
    mspack/qtmd.c 
    mspack/readbits.h 
    mspack/readhuff.h 
    mspack/lzss.h 
    mspack/lzssd.c 
    mspack/des.h 
    mspack/sha.h 
    mspack/crc32.c 
    mspack/crc32.h
)

add_library("mspack" SHARED ${MSPACK_SRC_FILES})
target_include_directories("mspack" PUBLIC "mspack")
