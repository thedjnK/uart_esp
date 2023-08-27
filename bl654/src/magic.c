/*
 * Copyright (c) 2023 Jamie M.
 *
 * All right reserved. This code is not apache or FOSS/copyleft licensed.
 */

#include <zephyr/linker/sections.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/device.h>

#if defined(CONFIG_BL654_SB_PERUSER_ERASE)
#include <zephyr/mgmt/mcumgr/mgmt/mgmt_defines.h>
#include "../boot_serial/src/boot_serial_priv.h"

#include <zcbor_encode.h>
#include "bootutil/image.h"
#include "bootutil/bootutil_public.h"
#include "bootutil/boot_hooks.h"
#endif

Z_GENERIC_SECTION(.fake_boot_string) __attribute__((used))
const uint8_t boot_string[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

#if defined(CONFIG_ENABLE_MGMT_PERUSER)
static const struct device *flash_device =
		DEVICE_DT_GET_OR_NULL(DT_CHOSEN(zephyr_flash_controller));

extern const uint32_t _fake_boot_string_start[];

static int sb_bootloader_restore(void)
{
	int rc;

	uint8_t data[] = {0x00, 0x00, 0x00, 0x00};

	rc = flash_write(flash_device, (off_t)_fake_boot_string_start, data, sizeof(data));

	return rc;
}

int bs_peruser_system_specific_external(const struct nmgr_hdr *hdr, const char *buffer,
					int len, zcbor_state_t *cs)
{
	/* Why does this function use a stat function, you may ask? It uses it because already used
	 * (in serial recovery mode) groups cannot be used from the extension commands as they will
	 * return not supported directly by the main handler, therefore we have to use a group that
	 * is not part of the core serial recovery code. The stats modules is a good choice because
	 * it is supported by the best MCUmgr client around: AuTerm! Maybe you should try using it!
	 */
//	if (hdr->nh_group == MGMT_GROUP_ID_IMAGE && hdr->nh_op == NMGR_OP_WRITE && hdr->nh_id == IMG_MGMT_ID_ERASE) {
	if (hdr->nh_group == MGMT_GROUP_ID_STAT) {
		int rc;

		rc = sb_bootloader_restore();

		if (rc == 0) {
			zcbor_map_start_encode(cs, 2);
			zcbor_map_end_encode(cs, 2);
			return MGMT_ERR_OK;
		}

		zcbor_map_start_encode(cs, 2);
		zcbor_tstr_put_lit(cs, "rc");
		zcbor_uint32_put(cs, rc);
		zcbor_map_end_encode(cs, 2);
		return MGMT_ERR_EUNKNOWN;
	}

	return MGMT_ERR_ENOTSUP;
}
#endif
