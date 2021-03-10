
# X initial slice offset
set RO_OFFSET_X 7
# Y initial slice offset
set RO_OFFSET_Y 30
# X slice offset increment per ro, must be multiple of 2
set RO_INC_X 0
# Y slice offset increment per ro, must be multiple of 8
set RO_INC_Y 1

# Please modify accodignly to your block design
set RO_DEPTH_SENSORS 8
set RO_COUNT_SENSORS 16

# Slices name
proc slice {x y} {
	return SLICE_X[expr $x]Y[expr $y]
}
# Name of the top of each ro
proc ro {id} {
	return SCAbox_i/ro_bank_0/U0/ro_bank_v1_0_S_AXI_inst/top/bank[$id].sensors
}

set x0 $RO_OFFSET_X
set y0 $RO_OFFSET_Y

puts "*** Placing RO sensors ***"

for {set id 0} {$id < $RO_COUNT_SENSORS} {incr id} {
	puts ro_$id



	puts "* Extra elements *"

	set x $x0
    set y $y0

    place_cell [ro $id]/oscillator [slice $x $y]/D5LUT
    place_cell [ro $id]/last_inv [slice $x $y]/B5LUT


	puts "* Counter *"

	set x $x0
    set y $y0
	incr x

    place_cell [ro $id]/count_reg0 [slice $x $y]/A5FF
    place_cell [ro $id]/counter[1].count_reg [slice $x $y]/AFF
    place_cell [ro $id]/counter[2].count_reg [slice $x $y]/B5FF
    place_cell [ro $id]/counter[3].count_reg [slice $x $y]/BFF
    place_cell [ro $id]/counter[4].count_reg [slice $x $y]/C5FF
    place_cell [ro $id]/counter[5].count_reg [slice $x $y]/CFF
    place_cell [ro $id]/counter[6].count_reg [slice $x $y]/D5FF
    place_cell [ro $id]/counter[7].count_reg [slice $x $y]/DFF

    incr y 

	for {set k 8} {$k < $RO_DEPTH_SENSORS} {incr k 8} {
		place_cell [ro $id]/counter[$k].count_reg [slice $x $y]/A5FF
		place_cell [ro $id]/counter[[expr $k + 1]].count_reg [slice $x $y]/AFF
		place_cell [ro $id]/counter[[expr $k + 2]].count_reg [slice $x $y]/B5FF
		place_cell [ro $id]/counter[[expr $k + 3]].count_reg [slice $x $y]/BFF
		place_cell [ro $id]/counter[[expr $k + 4]].count_reg [slice $x $y]/C5FF
		place_cell [ro $id]/counter[[expr $k + 5]].count_reg [slice $x $y]/CFF
		place_cell [ro $id]/counter[[expr $k + 6]].count_reg [slice $x $y]/D5FF
		place_cell [ro $id]/counter[[expr $k + 7]].count_reg [slice $x $y]/DFF

        incr y
	}   

	puts "* Sampling *"

	set x $x0
    set y $y0
   	incr x 2

	for {set k 0} {$k < $RO_DEPTH_SENSORS} {incr k 8} {
		place_cell [ro $id]/sampling[$k].sampling_reg [slice $x $y]/A5FF
		place_cell [ro $id]/sampling[[expr $k + 1]].sampling_reg [slice $x $y]/AFF
		place_cell [ro $id]/sampling[[expr $k + 2]].sampling_reg [slice $x $y]/B5FF
		place_cell [ro $id]/sampling[[expr $k + 3]].sampling_reg [slice $x $y]/BFF
		place_cell [ro $id]/sampling[[expr $k + 4]].sampling_reg [slice $x $y]/C5FF
		place_cell [ro $id]/sampling[[expr $k + 5]].sampling_reg [slice $x $y]/CFF
		place_cell [ro $id]/sampling[[expr $k + 6]].sampling_reg [slice $x $y]/D5FF
		place_cell [ro $id]/sampling[[expr $k + 7]].sampling_reg [slice $x $y]/DFF

        incr y
	}    
	
	incr x0 $RO_INC_X
	incr y0 $RO_INC_Y
}
