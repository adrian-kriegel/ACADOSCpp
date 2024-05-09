
get_filename_component(ACADOS_DIR $ENV{ACADOS_SOURCE_DIR} REALPATH)

if(EXISTS ${ACADOS_DIR})
    message(STATUS "Found ACADOS.")
else()
    message(FATAL_ERROR "Could not find ACADOS. ACADOS_SOURCE_DIR=${ACADOS_SOURCE_DIR} does not exist.")
endif()

set(ACADOS_LIBS acados hpipm blasfeo m)

set(
    ACADOS_INCLUDE_DIRECTORIES 
    ${ACADOS_DIR}/include 
    ${ACADOS_DIR}/include/acados
    ${ACADOS_DIR}/include/blasfeo/include
    ${ACADOS_DIR}/include/hpipm/include
)

set(ACADOS_LINK_DIRECTORIES ${ACADOS_DIR}/lib)


