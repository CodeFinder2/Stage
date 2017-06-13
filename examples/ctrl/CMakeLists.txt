SET( PLUGINS
  fasr
  sink
  source
  wander
  pioneer_flocking
  rasterize
  dynamic
)

# create a library module for each plugin and link libstage to each
foreach( PLUGIN ${PLUGINS} )
  add_library( ${PLUGIN} MODULE ${PLUGIN}.cc )
endforeach()

# activate the lasernoise plugin if we've a recent CMake:
if (NOT CMAKE_VERSION VERSION_LESS 3.1)
  add_library( lasernoise MODULE lasernoise.cc )
  set_property( TARGET lasernoise PROPERTY CXX_STANDARD 11 )
  set_property( TARGET lasernoise PROPERTY CXX_STANDARD_REQUIRED ON )
  set( PLUGINS ${PLUGINS} lasernoise )
else()
  message(WARNING "Your CMake version is too old, cannot build lasernoise controller plugin.")
endif()

# add extras to the list of plugins
add_library( fasr2 MODULE fasr2.cc astar/findpath.cpp )
set( PLUGINS ${PLUGINS} fasr2 )

set_source_files_properties( ${PLUGINS} PROPERTIES COMPILE_FLAGS "${FLTK_CFLAGS}" )

foreach( PLUGIN ${PLUGINS} )
  target_link_libraries( ${PLUGIN} stage ${OPENGL_LIBRARIES} )
endforeach()

# delete the "lib" prefix from the plugin libraries
set_target_properties( ${PLUGINS} PROPERTIES PREFIX "" )

# install in <prefix>/lib
install( TARGETS ${PLUGINS} DESTINATION ${PROJECT_PLUGIN_DIR} )
