set origin_dir ".."

open_project $origin_dir/video_design/turretmaster.xpr

launch_sdk -workspace $origin_dir/video_sdk -hwspec $origin_dir/video_sdk/turretmaster_video_bd_wrapper.hdf
