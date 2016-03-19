set origin_dir ".."

open_project $origin_dir/video_design/turretmaster.xpr

update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

launch_runs impl_1 -to_step write_bitstream

file copy -force\
    $origin_dir/video_design/turretmaster.runs/impl_1/turretmaster_video_bd_wrapper.sysdef\
    $origin_dir/video_sdk/turretmaster_video_bd_wrapper.hdf
