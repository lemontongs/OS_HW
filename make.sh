#!/bin/bash

gcc -o prog2 main.c prod_cons_MT.c prod_cons_MT.h -pthread && ./prog2 $@

