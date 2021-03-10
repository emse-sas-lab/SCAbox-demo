############################
# On-board LED             #
############################


set_property -dict {PACKAGE_PIN Y16 IOSTANDARD LVCMOS33} [get_ports reset_rtl]

create_pblock pblock_aes_0
add_cells_to_pblock [get_pblocks pblock_aes_0] [get_cells -quiet [list system_i/aes_0]]
resize_pblock [get_pblocks pblock_aes_0] -add {SLICE_X0Y0:SLICE_X43Y34}
resize_pblock [get_pblocks pblock_aes_0] -add {DSP48_X0Y0:DSP48_X1Y13}
resize_pblock [get_pblocks pblock_aes_0] -add {RAMB18_X0Y0:RAMB18_X2Y13}
resize_pblock [get_pblocks pblock_aes_0] -add {RAMB36_X0Y0:RAMB36_X2Y6}
set_property IS_SOFT TRUE [get_pblocks pblock_aes_0]

