# X initial slice offset
set TDC_OFFSET_X 3
# Y initial slice offset
set TDC_OFFSET_Y 17
# X slice offset increment per tdc, must be multiple of 2
set TDC_INC_X 0
# Y slice offset increment per tdc, must be multiple of 8
set TDC_INC_Y 8

set TDC_COUNT_SENSORS 8
set TDC_DEPTH_SENSORS 8
# Slices name
proc slice {x y} {
	return SLICE_X[expr $x]Y[expr $y]
}
# Name of the top of each tdc
proc tdc {id} {
	return SCAbox_i/tdc_bank_0/U0/tdc_bank_v1_0_S_AXI_inst/top/bank[$id].sensors
}

puts "*** Placing TDC sensors ***"

set x0 $TDC_OFFSET_X
set y0 $TDC_OFFSET_Y
for {set id 0} {$id < $TDC_COUNT_SENSORS} {incr id} {
	puts tdc_$id

	puts "* Coarse *"

	set i 0
	set x $x0
	for {set y $y0} {$y < [expr $y0 + 4]} {incr y} {
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[0].lut [slice $x $y]/A5LUT
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[1].lut [slice $x $y]/B5LUT
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[2].lut [slice $x $y]/C5LUT
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[3].lut [slice $x $y]/D5LUT
        
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[0].latch [slice $x $y]/AFF
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[1].latch [slice $x $y]/BFF
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[2].latch [slice $x $y]/CFF
        place_cell [tdc $id]/coarse/delay_line[0].block_$i/delay_path[3].latch [slice $x $y]/DFF
					
		incr i
	}
	incr x
	set y $y0
	place_cell [tdc $id]/coarse/delay_line[0].mux/delay_path[0].lut [slice $x $y]/A5LUT
	place_cell [tdc $id]/coarse/delay_line[0].mux/delay_path[1].lut [slice $x $y]/B5LUT
	place_cell [tdc $id]/coarse/delay_line[0].mux/delay_path[2].lut [slice $x $y]/C5LUT
	place_cell [tdc $id]/coarse/delay_line[0].mux/delay_path[3].lut [slice $x $y]/D5LUT

	place_cell [tdc $id]/coarse/delay_line[0].mux/middle_mux_1 [slice $x $y]/F7AMUX
	place_cell [tdc $id]/coarse/delay_line[0].mux/middle_mux_0 [slice $x $y]/F7BMUX
	place_cell [tdc $id]/coarse/delay_line[0].mux/out_mux [slice $x $y]/F8MUX

	puts "* Fine *"

	set i 0
	set x $x0
	for {set y [expr $y0 + 4]} {$y < [expr $y0 + 8]} {incr y} {
		place_cell [tdc $id]/fine/delay_line[0].block_$i/delay_path[0].lut [slice $x $y]/A5LUT
		place_cell [tdc $id]/fine/delay_line[0].block_$i/delay_path[1].lut [slice $x $y]/B5LUT
		place_cell [tdc $id]/fine/delay_line[0].block_$i/delay_path[2].lut [slice $x $y]/C5LUT
		place_cell [tdc $id]/fine/delay_line[0].block_$i/delay_path[3].lut [slice $x $y]/D5LUT

		place_cell [tdc $id]/fine/delay_line[0].block_$i/middle_mux_1 [slice $x $y]/F7AMUX
		place_cell [tdc $id]/fine/delay_line[0].block_$i/middle_mux_0 [slice $x $y]/F7BMUX
		place_cell [tdc $id]/fine/delay_line[0].block_$i/out_mux [slice $x $y]/F8MUX
					
		incr i
	}

	incr x
	set y [expr $y0 + 4]
	place_cell [tdc $id]/fine/delay_line[0].mux/delay_path[0].lut [slice $x $y]/A5LUT
	place_cell [tdc $id]/fine/delay_line[0].mux/delay_path[1].lut [slice $x $y]/B5LUT
	place_cell [tdc $id]/fine/delay_line[0].mux/delay_path[2].lut [slice $x $y]/C5LUT
	place_cell [tdc $id]/fine/delay_line[0].mux/delay_path[3].lut [slice $x $y]/D5LUT

	place_cell [tdc $id]/fine/delay_line[0].mux/middle_mux_1 [slice $x $y]/F7AMUX
	place_cell [tdc $id]/fine/delay_line[0].mux/middle_mux_0 [slice $x $y]/F7BMUX
	place_cell [tdc $id]/fine/delay_line[0].mux/out_mux [slice $x $y]/F8MUX

	puts "* Sampling *"
	set i 0
	incr x
	for {set y $y0} {$y < [expr $y0 + $TDC_DEPTH_SENSORS]} {incr y} {
		place_cell [tdc $id]/sampling/delay_line[$i].block_n/delay_path [slice $x $y]/CARRY4
		place_cell [tdc $id]/sampling/delay_line[$i].block_n/sampling_path[0].reg [slice $x $y]/AFF
		place_cell [tdc $id]/sampling/delay_line[$i].block_n/sampling_path[1].reg [slice $x $y]/BFF
		place_cell [tdc $id]/sampling/delay_line[$i].block_n/sampling_path[2].reg [slice $x $y]/CFF
		place_cell [tdc $id]/sampling/delay_line[$i].block_n/sampling_path[3].reg [slice $x $y]/DFF
		
		incr i 
	}    	
	incr x0 $TDC_INC_X
	incr y0 [expr $TDC_INC_Y+1]
}
