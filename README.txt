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
