/*
 * Copyright (c) 2023 Jamie M.
 *
 * All right reserved. This code is not apache or FOSS/copyleft licensed.
 */

#include <zephyr/linker/sections.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>

Z_GENERIC_SECTION(.fake_boot_string) __attribute__((used))
const uint8_t boot_string[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};