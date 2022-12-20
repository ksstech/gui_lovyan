# GUI_LOVYAN

set( srcs "gui_lovyan.cpp" )
#set( srcs "gui_lovyan_demo.cpp" )
#set( srcs "gui_lovyan_dk41.cpp" )
#set( srcs "gui_lovyan_mf3p16.cpp" )
set( include_dirs "." )
set( priv_include_dirs )
set( requires )
set( priv_requires
	hal_esp32 irmacos main rtos-support printfx syslog systiming
	lovyan
)

idf_component_register(
	SRCS ${srcs} 
	INCLUDE_DIRS ${include_dirs} 
	PRIV_INCLUDE_DIRS ${priv_include_dirs} 
	REQUIRES ${requires} 
	PRIV_REQUIRES ${priv_requires} 
)
