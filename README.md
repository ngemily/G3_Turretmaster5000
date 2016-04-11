Turretmaster 5000
=================

About
-----
The aim of this project is to develop a turret based laser-pointer with two
degrees of freedom, that can identify specific objects, target and then "fire"
at them.  This project incorporates a variety of subsystems including: audio,
video, SD card, external memory, and electromechanical components.

Authors:
- Roberto Bortolussi
- Michael Halamicek
- Emily Ng
- Patrick Payne

Quick Start
-----------

*Important:*  This project makes use of git submodules.  In order to acquire all
source files, ensure that you run `git submodule init && git submodule update`
after cloning.

Steps to generate the project:

1. Generate the vivado project by calling the project creation tcl script from
   within GIT_ROOT/video_design (create_project.tcl).

   `vivado -mode batch -source create_project.tcl`
   
2. Compile the project and export the hardware.

   `vivado -mode batch -source compile_project.tcl`
   
3. Launch the SDK.

   `vivado -mode batch -source launch_sdk.tcl`
   
4. Import the projects.

    In the SDK, click on `file>import`. Under the "general" directory, select
    "Existing Projects Into Workspace", then click next.  Set GIT_ROOT/video_sdk
    as the root directory, and import the "NexysVideoSentryTurret" and
    "turretmaster_bsp" projects.
    
5. Build the sources and program the board as per the tutorials.

Directory Structure
-------------------

| Directory        | Description                                                |
|----------------- |------------------------------------------------------------|
| `bd/`            | Tcl scripts for generating block designs.                  |
| `constraints/`   | Pin and clock constraint files.                            |
| `doc/`           | Helpful documents for understanding our code and hardware. |
| `hdl/`           | Misc. other HDL files, usually block design wrappers.      |
| `ip_repo/`       | A managed IP repository for our custom IPs.                |
| `video_design/`  | The scripts for generating the vivado project.             |
| `video_sdk/`     | The microblaze code for running our project.               |
| `nexys4_design/` | *Deprecated* Nexys4 Vivado project scripts.                |
| `sdk/`           | *Deprecated* Turret control software and bsp               |
