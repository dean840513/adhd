/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "cras_device_blacklist.h"
#include "iniparser_wrapper.h"
#include "utlist.h"

struct cras_device_blacklist {
	dictionary *ini;
};

/*
 * Exported Interface
 */

struct cras_device_blacklist *
cras_device_blacklist_create(const char *config_path)
{
	struct cras_device_blacklist *blacklist;
	char ini_name[MAX_INI_NAME_LENGTH + 1];

	blacklist = calloc(1, sizeof(*blacklist));
	if (!blacklist)
		return NULL;

	snprintf(ini_name, MAX_INI_NAME_LENGTH, "%s/%s", config_path,
		 "device_blacklist");
	ini_name[MAX_INI_NAME_LENGTH] = '\0';
	blacklist->ini = iniparser_load_wrapper(ini_name);

	return blacklist;
}

void cras_device_blacklist_destroy(struct cras_device_blacklist *blacklist)
{
	if (blacklist && blacklist->ini)
		iniparser_freedict(blacklist->ini);
	free(blacklist);
}

int cras_device_blacklist_check(struct cras_device_blacklist *blacklist,
				unsigned vendor_id, unsigned product_id,
				unsigned desc_checksum, unsigned device_index)
{
	char ini_key[MAX_INI_KEY_LENGTH + 1];

	if (!blacklist)
		return 0;

	snprintf(ini_key, MAX_INI_KEY_LENGTH, "USB_Outputs:%04x_%04x_%08x_%u",
		 vendor_id, product_id, desc_checksum, device_index);
	ini_key[MAX_INI_KEY_LENGTH] = 0;
	return iniparser_getboolean(blacklist->ini, ini_key, 0);
}
