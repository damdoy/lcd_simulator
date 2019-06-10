#!/bin/bash

#ensure parammeter
if [ "$#" -ne 1 ]; then
    echo "should pass folder name as argument"
    exit 1
fi

folder_name="$1"
json_filename="segments.json"
png_filename="segments.png"
fpga_filename="segments.v"

if [ ! -d "${folder_name}" ]; then
  echo "folder ${folder_name} doesn't exist"
  exit 1
fi

if [ ! -f "${folder_name}/${json_filename}" ]; then
  echo "file "${folder_name}/${json_filename}" doesn't exist"
  exit 1
fi

if [ ! -f "${folder_name}/${png_filename}" ]; then
  echo "file "${folder_name}/${png_filename}" doesn't exist"
  exit 1
fi

if [ ! -f "${folder_name}/${fpga_filename}" ]; then
  echo "file "${folder_name}/${fpga_filename}" doesn't exist"
  exit 1
fi

echo "running verilator"
verilator -Wall --cc src/top.v "${folder_name}/${fpga_filename}" --Mdir "verilator_output"

mkdir -p build
pushd build
cmake ..
make
popd
cp "build/lcd_simulator" .
