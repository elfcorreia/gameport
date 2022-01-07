#!/bin/bash

function compile_linux_static_lib {
	SRC=$1
	DST=$(basename $SRC .cpp).obj

	# Generates object file
	$(CC) -I . -c $SRC -o build/linux_wayland.obj	
	# Glue objs in a single file
	ar ru build/linux_wayland.obj build/easyview.a
	# Creates a fast index to access objs
	ranlib build/easyview.a
}

function make_static_lib {	
}