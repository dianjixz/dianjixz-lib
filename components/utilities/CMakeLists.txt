# Config enable component3 or not in Kconfig
if(CONFIG_UTILITIES_ENABLED)

    if(CONFIG_UTILITIES_FMT_ENABLED)

        list(APPEND ADD_INCLUDE "party/fmt/include"
            )
        # append_srcs_dir(ADD_SRCS "party/fmt/src") 
    endif()


    if(CONFIG_UTILITIES_EASYMSGCPP_ENABLED)

        list(APPEND ADD_INCLUDE "party/easymsgcpp/src/include"
            )
        append_srcs_dir(ADD_SRCS "party/easymsgcpp/src")

    endif()

    if(CONFIG_UTILITIES_UART_ENABLED)
        list(APPEND ADD_INCLUDE "party/linux_uart")
        append_srcs_dir(ADD_SRCS "party/linux_uart")
    endif()

    if(CONFIG_UTILITIES_I2C_ENABLED)
        list(APPEND ADD_INCLUDE "party/linux_i2c")
        append_srcs_dir(ADD_SRCS "party/linux_i2c")
    endif()

    if(CONFIG_UTILITIES_FRAMEBUFFER_ENABLED)
        list(APPEND ADD_INCLUDE "party/framebuffer")
        append_srcs_dir(ADD_SRCS "party/framebuffer")
    endif()

    if(CONFIG_UTILITIES_INI_ENABLED)
        list(APPEND ADD_INCLUDE "party/ini")
        append_srcs_dir(ADD_SRCS "party/ini")
    endif()

    if(CONFIG_UTILITIES_SIMPILE_DRAW_ENABLED)
        list(APPEND ADD_INCLUDE "party/simple_draw")
        append_srcs_dir(ADD_SRCS "party/simple_draw")
    endif()
    
    if(CONFIG_UTILITIES_SPI_ENABLED)
        list(APPEND ADD_INCLUDE "party/linux_spi")
        append_srcs_dir(ADD_SRCS "party/linux_spi")
    endif()

    if(CONFIG_UTILITIES_PTMX_ENABLED)
        list(APPEND ADD_INCLUDE "party/ptmx")
        append_srcs_dir(ADD_SRCS "party/ptmx")
        list(APPEND ADD_REQUIREMENTS pthread)
    endif()
    
    # if(CONFIG_RTSP_ENABLED)

    #     list(APPEND ADD_INCLUDE "party/RtspServer"
    #         )
    #     append_srcs_dir(ADD_SRCS "party/RtspServer/RtspServer/src/net"
    #                              "party/RtspServer/RtspServer/src/xop"
    #                              "party/RtspServer"
    #                              "party/RtspServer/rtsp/src"
    #     )
    #     list(APPEND ADD_PRIVATE_INCLUDE "party/RtspServer/rtsp/inc"
    #                                     "party/RtspServer/RtspServer/src"
    #                                     "party/RtspServer/RtspServer/src/3rdpart"
    #     )
    # endif()

    if(CONFIG_UTILITIES_BASE64_ENABLED)

        list(APPEND ADD_INCLUDE "party/base64"
            )
        append_srcs_dir(ADD_SRCS "party/base64"
        )
    endif()
    
    ################# Add include #################
    list(APPEND ADD_INCLUDE "include" 
        )
    # list(APPEND ADD_PRIVATE_INCLUDE "include_private")
    ###############################################

    ############## Add source files ###############
    list(APPEND ADD_SRCS  "test.c"
        )
    # FILE(GLOB_RECURSE EXTRA_SRC  "src/*.c")
    # FILE(GLOB EXTRA_SRC  "src/*.c")
    # list(APPEND ADD_SRCS  ${EXTRA_SRC})
    # aux_source_directory(src ADD_SRCS)  # collect all source file in src dir, will set var ADD_SRCS
    # append_srcs_dir(ADD_SRCS "party/fmt/src")     # append source file in src dir to var ADD_SRCS
    # list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
    # set(ADD_ASM_SRCS "src/asm.S")
    # list(APPEND ADD_SRCS ${ADD_ASM_SRCS})
    # SET_PROPERTY(SOURCE ${ADD_ASM_SRCS} PROPERTY LANGUAGE C) # set .S  ASM file as C language
    # SET_SOURCE_FILES_PROPERTIES(${ADD_ASM_SRCS} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D BBBBB")
    ###############################################


    ###### Add required/dependent components ######
    # list(APPEND ADD_REQUIREMENTS component1)
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
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DAAAAA=1)

    #### Add compile option for this component
    #### and components denpend on this component
    # list(APPEND ADD_DEFINITIONS -DAAAAA222=1
    #                             -DAAAAA333=1)
    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group lib/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_UTILITIES_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

