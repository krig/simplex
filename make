#!/bin/bash
[[ ! -d build ]] && ./configure
cd build
make "$@"
