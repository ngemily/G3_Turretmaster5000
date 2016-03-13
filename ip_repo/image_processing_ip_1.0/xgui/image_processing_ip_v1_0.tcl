# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  set C_S_AXI_LITE_DATA_WIDTH [ipgui::add_param $IPINST -name "C_S_AXI_LITE_DATA_WIDTH" -parent ${Page_0} -widget comboBox]
  set_property tooltip {Width of S_AXI data bus} ${C_S_AXI_LITE_DATA_WIDTH}
  set C_S_AXI_LITE_ADDR_WIDTH [ipgui::add_param $IPINST -name "C_S_AXI_LITE_ADDR_WIDTH" -parent ${Page_0}]
  set_property tooltip {Width of S_AXI address bus} ${C_S_AXI_LITE_ADDR_WIDTH}
  ipgui::add_param $IPINST -name "C_S_AXI_LITE_BASEADDR" -parent ${Page_0}
  ipgui::add_param $IPINST -name "C_S_AXI_LITE_HIGHADDR" -parent ${Page_0}
  set C_S_AXIS_MM2S_TDATA_WIDTH [ipgui::add_param $IPINST -name "C_S_AXIS_MM2S_TDATA_WIDTH" -parent ${Page_0} -widget comboBox]
  set_property tooltip {AXI4Stream sink: Data Width} ${C_S_AXIS_MM2S_TDATA_WIDTH}
  set C_M_AXIS_S2MM_TDATA_WIDTH [ipgui::add_param $IPINST -name "C_M_AXIS_S2MM_TDATA_WIDTH" -parent ${Page_0} -widget comboBox]
  set_property tooltip {Width of S_AXIS address bus. The slave accepts the read and write addresses of width C_M_AXIS_TDATA_WIDTH.} ${C_M_AXIS_S2MM_TDATA_WIDTH}
  set C_M_AXIS_S2MM_START_COUNT [ipgui::add_param $IPINST -name "C_M_AXIS_S2MM_START_COUNT" -parent ${Page_0}]
  set_property tooltip {Start count is the numeber of clock cycles the master will wait before initiating/issuing any transaction.} ${C_M_AXIS_S2MM_START_COUNT}


}

proc update_PARAM_VALUE.AXIS_TDATA_WIDTH { PARAM_VALUE.AXIS_TDATA_WIDTH } {
	# Procedure called to update AXIS_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXIS_TDATA_WIDTH { PARAM_VALUE.AXIS_TDATA_WIDTH } {
	# Procedure called to validate AXIS_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.FRAME_HEIGHT { PARAM_VALUE.FRAME_HEIGHT } {
	# Procedure called to update FRAME_HEIGHT when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FRAME_HEIGHT { PARAM_VALUE.FRAME_HEIGHT } {
	# Procedure called to validate FRAME_HEIGHT
	return true
}

proc update_PARAM_VALUE.FRAME_WIDTH { PARAM_VALUE.FRAME_WIDTH } {
	# Procedure called to update FRAME_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FRAME_WIDTH { PARAM_VALUE.FRAME_WIDTH } {
	# Procedure called to validate FRAME_WIDTH
	return true
}

proc update_PARAM_VALUE.WORDS_PER_LINE { PARAM_VALUE.WORDS_PER_LINE } {
	# Procedure called to update WORDS_PER_LINE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.WORDS_PER_LINE { PARAM_VALUE.WORDS_PER_LINE } {
	# Procedure called to validate WORDS_PER_LINE
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to update C_S_AXI_LITE_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to validate C_S_AXI_LITE_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to update C_S_AXI_LITE_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to validate C_S_AXI_LITE_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_BASEADDR { PARAM_VALUE.C_S_AXI_LITE_BASEADDR } {
	# Procedure called to update C_S_AXI_LITE_BASEADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_BASEADDR { PARAM_VALUE.C_S_AXI_LITE_BASEADDR } {
	# Procedure called to validate C_S_AXI_LITE_BASEADDR
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_HIGHADDR { PARAM_VALUE.C_S_AXI_LITE_HIGHADDR } {
	# Procedure called to update C_S_AXI_LITE_HIGHADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_HIGHADDR { PARAM_VALUE.C_S_AXI_LITE_HIGHADDR } {
	# Procedure called to validate C_S_AXI_LITE_HIGHADDR
	return true
}

proc update_PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH { PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH } {
	# Procedure called to update C_S_AXIS_MM2S_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH { PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH } {
	# Procedure called to validate C_S_AXIS_MM2S_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH { PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH } {
	# Procedure called to update C_M_AXIS_S2MM_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH { PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH } {
	# Procedure called to validate C_M_AXIS_S2MM_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT { PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT } {
	# Procedure called to update C_M_AXIS_S2MM_START_COUNT when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT { PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT } {
	# Procedure called to validate C_M_AXIS_S2MM_START_COUNT
	return true
}


proc update_MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH { MODELPARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXIS_MM2S_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH { MODELPARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH}] ${MODELPARAM_VALUE.C_M_AXIS_S2MM_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_M_AXIS_S2MM_START_COUNT { MODELPARAM_VALUE.C_M_AXIS_S2MM_START_COUNT PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_M_AXIS_S2MM_START_COUNT}] ${MODELPARAM_VALUE.C_M_AXIS_S2MM_START_COUNT}
}

proc update_MODELPARAM_VALUE.FRAME_WIDTH { MODELPARAM_VALUE.FRAME_WIDTH PARAM_VALUE.FRAME_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FRAME_WIDTH}] ${MODELPARAM_VALUE.FRAME_WIDTH}
}

proc update_MODELPARAM_VALUE.FRAME_HEIGHT { MODELPARAM_VALUE.FRAME_HEIGHT PARAM_VALUE.FRAME_HEIGHT } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FRAME_HEIGHT}] ${MODELPARAM_VALUE.FRAME_HEIGHT}
}

proc update_MODELPARAM_VALUE.AXIS_TDATA_WIDTH { MODELPARAM_VALUE.AXIS_TDATA_WIDTH PARAM_VALUE.AXIS_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXIS_TDATA_WIDTH}] ${MODELPARAM_VALUE.AXIS_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.WORDS_PER_LINE { MODELPARAM_VALUE.WORDS_PER_LINE PARAM_VALUE.WORDS_PER_LINE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.WORDS_PER_LINE}] ${MODELPARAM_VALUE.WORDS_PER_LINE}
}

