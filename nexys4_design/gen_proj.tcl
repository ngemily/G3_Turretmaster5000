# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir ".."

# Create project
create_project turretmaster . -part xc7a100tcsg324-1 -force

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects turretmaster]

# Disable use of board files for now; this gives us total control over the 
# pin assignments. When generating the block design with the board files, there
# were issues where it would randomly assign some ports.
#set_property "board_part" "digilentinc.com:nexys4_ddr:part0:1.1" $obj
#
set_property "default_lib" "xil_defaultlib" $obj
set_property "simulator_language" "Mixed" $obj

# Add our custom IP repo.
set_property  ip_repo_paths  "$origin_dir/ip_repo" [current_project]

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}

# Create block design
 source $origin_dir/bd/nexys4_bd.tcl


 # We no longer autogenerate the wrapper.
 # set design_name [get_bd_designs]
 #make_wrapper -files [get_files $design_name.bd] \
 #             -fileset [get_filesets sources_1] \
 #             -import -top

# Set 'sources_1' fileset properties
set obj [get_filesets sources_1]
set file "[file normalize "$origin_dir/hdl/nexys4_bd_wrapper.v"]"
set file_added [add_files -norecurse -fileset $obj $file]
set_property "top" "nexys4_bd_wrapper" $obj

# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}

# Set 'constrs_1' fileset object
set obj [get_filesets constrs_1]

# Add/Import constrs file and set constrs file properties
set file "[file normalize "$origin_dir/constraints/Nexys4DDR_Master.xdc"]"
set file_added [add_files -norecurse -fileset $obj $file]
set file "$origin_dir/constraints/Nexys4DDR_Master.xdc"
set file [file normalize $file]
set file_obj [get_files -of_objects [get_filesets constrs_1] [list "*$file"]]
set_property "file_type" "XDC" $file_obj

# Set 'constrs_1' fileset properties
set obj [get_filesets constrs_1]

puts "INFO: Project created:turretmaster"
