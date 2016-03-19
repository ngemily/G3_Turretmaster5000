Steps to generate the project:
1) Generate the vivado project by calling the project creation tcl script from
   within GIT_ROOT/nexys4_design (gen_proj.tcl). This is done by running:
   vivado -mode batch -source gen_proj.tcl

2) Pull up the project in the vivado GUI, and compile the design.
3) When compilation is complete, export the hardware to GIT_ROOT/sdk
4) click `file>Launch SDK`, with both directory options set to GIT_ROOT/sdk
5) when in the SDK, click on `file>import`. Under the "general" directory, select
   "Existing Projects Into Workspace", then click next.
6) Set GIT_ROOT/sdk as the root directory, and import the "turretcontrol" and
   "turretcontrol_bsp" projects.
7) Generate the linker script for the "turretcontrol" project.
8) Build the sources and program the board as per the tutorials.

Alternate steps to 2-4 above:
2-3) Compile and export hardware
   vivado -mode batch -source compile_project.tcl
4) Launch SDK
   vivado -mode batch -source launch_sdk.tcl


How to generate files that play correctly on the board:
1) Open the source sound file in Wavosaur (mp3 or wav)
2) Crop to the time period you want to save.
3) If necessary, split to stereo.
4) resample to 96000 Hz (96 KHz)
5) adjust the volume as necessary using process > normalize (-15db seems normal)
5) file > export > export raw binary. Select the following options:
    - signed
    - 16 bits per sample
    - Big endian

When read off of the Sd card into memory and played, it shouldn't sound like 
terrible static.


How to update the video processing ip core:
    
    git submodule init
    git submodule update 
