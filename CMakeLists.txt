cmake_minimum_required( VERSION 3.1)
project( libdae C)

set( LIBDAE_SRC "./src/dae.c" "./src/daeu.c")
add_library( dae STATIC ${LIBDAE_SRC})
target_include_directories( dae PUBLIC "./include/")