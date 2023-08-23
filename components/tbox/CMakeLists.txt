# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "../../github_source")
if(CONFIG_TBOX_ENABLED)

    # set(EXCLUDED_FOLDER "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database")

    # file(GLOB_RECURSE TBOX_SOURCE_FILES 
    #     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/*.c"
    # )

    # foreach(file ${TBOX_SOURCE_FILES})
    #     get_filename_component(dirname ${file} DIRECTORY)
    #     if(NOT "${dirname}" STREQUAL "${EXCLUDED_FOLDER}")
    #         list(APPEND INCLUDED_C_FILES ${file})
    #     endif()
    # endforeach()

    # list(APPEND ADD_SRCS  "${INCLUDED_C_FILES}")
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time"
    # )

    # message("${ADD_SRCS}")
    #libc
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/impl"
    )
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/impl"
    )   
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/random"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/impl"
    )   
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/string"
    )   
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl"
    )  
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm"
    )   
    # # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database"
    # #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database/impl"
    # # )  
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/prefix"
    ) 

    list(APPEND ADD_SRCS  "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/bkdr.c"
                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/fnv32.c"
                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/adler32.c"
    )

    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/dns"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/http"
    ) 

    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/zip"
    # ) 
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element"
    ) 
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/object"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/object/impl"
    # ) 
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/filter"
    ) 
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils"
                            #  "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/impl"
    ) 
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/coroutine"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/coroutine/impl"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/coroutine/stackless"
    # ) 
    append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform"
    ) 
    list(APPEND ADD_SRCS     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/platform.c"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/pollerdata.c"
                             "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/dns.c"
    )
    list(APPEND ADD_SRCS  "${PRIVATE_MODULE_DIR}/tbox/src/tbox/tbox.c"
    )
    message("${ADD_SRCS}")
    ################# Add include #################

	list(APPEND ADD_INCLUDE "include")
    ###############################################

    ############## Add source files ###############
	append_srcs_dir(ADD_SRCS "src")
    # list(APPEND ADD_SRCS  "log.c/src/superlog.c"
    #     )
    # FILE(GLOB_RECURSE EXTRA_SRC  "src/*.c")
    # FILE(GLOB EXTRA_SRC  "src/*.c")
    # list(APPEND ADD_SRCS  ${EXTRA_SRC})
    # aux_source_directory(src ADD_SRCS)  # collect all source file in src dir, will set var ADD_SRCS
    # append_srcs_dir(ADD_SRCS "src")     # append source file in src dir to var ADD_SRCS
    # list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
    # set(ADD_ASM_SRCS "src/asm.S")
    # list(APPEND ADD_SRCS ${ADD_ASM_SRCS})
    # SET_PROPERTY(SOURCE ${ADD_ASM_SRCS} PROPERTY LANGUAGE C) # set .S  ASM file as C language
    # SET_SOURCE_FILES_PROPERTIES(${ADD_ASM_SRCS} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D BBBBB")
    ###############################################


    ###### Add required/dependent components ######
    list(APPEND ADD_REQUIREMENTS m)
    ###############################################

    ###### Add link search path for requirements/libs ######
    # list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib")
    # list(APPEND ADD_REQUIREMENTS pthread m)  # add system libs, pthread and math lib for example here
    # set (OpenCV_DIR opencv/lib/cmake/opencv4)
    # find_package(OpenCV REQUIRED)
    ###############################################

    ############ Add static libs ##################
    # list(APPEND ADD_STATIC_LIB "lib/libtest.a")
    ###############################################

    ############ Add dynamic libs ##################
    # list(APPEND ADD_DYNAMIC_LIB "lib/test1.so"
    #                             "lib/test2.so"
    # )
    ###############################################

    #### Add compile option for this component ####
    #### Just for this component, won't affect other 
    #### modules, including component that depend 
    #### on this component
	# list(APPEND ADD_LINKOPTIONS_PRIVATE -w)
    #### Add compile option for this component
    #### and components denpend on this component
    # list(APPEND ADD_DEFINITIONS_PRIVATE -w)
    # list(APPEND ADD_DEFINITIONS -w)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ARCH_UNIX)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ARCH_WIN32)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_LWIP)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_SOCKET)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_MBEDTLS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_OPENSSL)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_IPV6)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_MD5)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_SSI)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_RANDOM)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_TLS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_MILLIS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_PACKED_FS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_FATFS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_LINES)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_IO_SIZE=2048)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_MAX_RECV_SIZE=(3 * 1024 * 1024))
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_MAX_HTTP_HEADERS=40)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_HTTP_INDEX="index.html")
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_FATFS_ROOT="/")

    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group lib/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_TBOX_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

