# Run this script to create the Vivado project files in the WORKING DIRECTORY
# If ::create_path global variable is set, the project is created under that path instead of the working dir

if {[info exists ::create_path]} {
	set dest_dir $::create_path
} else {
	set dest_dir [pwd]
}
puts "INFO: Creating new project in $dest_dir"

# Create project
create_project "turretmaster" $dest_dir -force -part xc7a200tsbg484-1

# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir ".."

# Set the directory path for the original project from where this script was exported
set orig_proj_dir "[file normalize "$origin_dir/proj"]"

set src_dir $origin_dir/src
set repo_dir $origin_dir/ip_repo

# Add our custom IP repo.
set_property  ip_repo_paths  "$origin_dir/ip_repo" [current_project]

# Refresh IP Repositories
update_ip_catalog

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects turretmaster]
set_property "default_lib" "xil_defaultlib" $obj
set_property "board_part" "digilentinc.com:nexys_video:part0:1.1" $obj
set_property "simulator_language" "Mixed" $obj
set_property "target_language" "verilog" $obj

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}

# Create block design
source $origin_dir/bd/video_bd.tcl
puts "INFO: Block design created: $design_name.bd"

# Add the wrapper HDL for the block design.
set obj [get_filesets sources_1]
set file "[file normalize "$origin_dir/hdl/turretmaster_video_bd_wrapper.v"]"
set file_added [add_files -norecurse -fileset $obj $file]
set_property "top" "turretmaster_video_bd_wrapper" $obj

# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}

# Set 'constrs_1' fileset object
set obj [get_filesets constrs_1]

# Add/Import constrs file and set constrs file properties
set file "[file normalize "$origin_dir/constraints/NexysVideo.xdc"]"
set file_added [add_files -norecurse -fileset $obj $file]
set file "$origin_dir/constraints/NexysVideo.xdc"
set file [file normalize $file]
set file_obj [get_files -of_objects [get_filesets constrs_1] [list "*$file"]]
set_property "file_type" "XDC" $file_obj

puts "INFO: Project created"
