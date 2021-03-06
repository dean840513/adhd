/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CRAS_BT_DEVICE_H_
#define CRAS_BT_DEVICE_H_

#include <dbus/dbus.h>

struct cras_bt_adapter;
struct cras_bt_device;
struct cras_iodev;
struct cras_timer;

/* All the reasons for when CRAS schedule a suspend to BT device. */
enum cras_bt_device_suspend_reason {
	A2DP_LONG_TX_FAILURE,
	A2DP_TX_FATAL_ERROR,
	CONN_WATCH_TIME_OUT,
	HFP_SCO_SOCKET_ERROR,
	HFP_AG_START_FAILURE,
	UNEXPECTED_PROFILE_DROP,
};

enum cras_bt_device_profile {
	CRAS_BT_DEVICE_PROFILE_A2DP_SOURCE = (1 << 0),
	CRAS_BT_DEVICE_PROFILE_A2DP_SINK = (1 << 1),
	CRAS_BT_DEVICE_PROFILE_AVRCP_REMOTE = (1 << 2),
	CRAS_BT_DEVICE_PROFILE_AVRCP_TARGET = (1 << 3),
	CRAS_BT_DEVICE_PROFILE_HFP_HANDSFREE = (1 << 4),
	CRAS_BT_DEVICE_PROFILE_HFP_AUDIOGATEWAY = (1 << 5),
	CRAS_BT_DEVICE_PROFILE_HSP_HEADSET = (1 << 6),
	CRAS_BT_DEVICE_PROFILE_HSP_AUDIOGATEWAY = (1 << 7)
};

enum cras_bt_device_profile cras_bt_device_profile_from_uuid(const char *uuid);

struct cras_bt_device *cras_bt_device_create(DBusConnection *conn,
					     const char *object_path);

/*
 * Removes a BT device from record. If this device is connected state,
 * ensure the associated A2DP and HFP AG be removed cleanly.
 */
void cras_bt_device_remove(struct cras_bt_device *device);

void cras_bt_device_reset();

struct cras_bt_device *cras_bt_device_get(const char *object_path);

const char *cras_bt_device_object_path(const struct cras_bt_device *device);
struct cras_bt_adapter *
cras_bt_device_adapter(const struct cras_bt_device *device);
const char *cras_bt_device_address(const struct cras_bt_device *device);
const char *cras_bt_device_name(const struct cras_bt_device *device);
int cras_bt_device_paired(const struct cras_bt_device *device);
int cras_bt_device_trusted(const struct cras_bt_device *device);
int cras_bt_device_connected(const struct cras_bt_device *device);

void cras_bt_device_update_properties(struct cras_bt_device *device,
				      DBusMessageIter *properties_array_iter,
				      DBusMessageIter *invalidated_array_iter);

/* Updates the supported profiles on dev. Expose for unit test. */
int cras_bt_device_set_supported_profiles(struct cras_bt_device *device,
					  unsigned int profiles);

/* Checks if profile is claimed supported by the device. */
int cras_bt_device_supports_profile(const struct cras_bt_device *device,
				    enum cras_bt_device_profile profile);

/* Sets if the BT audio device should use hardware volume.
 * Args:
 *    device - The remote bluetooth audio device.
 *    use_hardware_volume - Set to true to indicate hardware volume
 *        is preferred over software volume.
 */
void cras_bt_device_set_use_hardware_volume(struct cras_bt_device *device,
					    int use_hardware_volume);

/* Gets if the BT audio device should use hardware volume. */
int cras_bt_device_get_use_hardware_volume(struct cras_bt_device *device);

/* Sets device connected state. Expose for unit test. */
void cras_bt_device_set_connected(struct cras_bt_device *device, int value);

/* Forces disconnect the bt device. Used when handling audio error
 * that we want to make the device be completely disconnected from
 * host to reflect the state that an error has occurred.
 * Args:
 *    conn - The dbus connection.
 *    device - The bt device to disconnect.
 */
int cras_bt_device_disconnect(DBusConnection *conn,
			      struct cras_bt_device *device);

/* Gets the SCO socket for the device.
 * Args:
 *     device - The device object to get SCO socket for.
 *     codec - 1 for CVSD, 2 for mSBC
 */
int cras_bt_device_sco_connect(struct cras_bt_device *device, int codec);

/* Gets the SCO packet size in bytes, used by HFP iodev for audio I/O.
 * The logic is built base on experience: for USB bus, respect BT Core spec
 * that has clear recommendation of packet size of codecs (CVSD, mSBC).
 * As for other buses, use the MTU value of SCO socket filled by driver.
 * Args:
 *    device - The bt device to query mtu.
 *    sco_socket - The SCO socket.
 *    codec - 1 for CVSD, 2 for mSBC per HFP 1.7 specification.
 */
int cras_bt_device_sco_packet_size(struct cras_bt_device *device,
				   int sco_socket, int codec);

/* Appends an iodev to bt device.
 * Args:
 *    device - The device to append iodev to.
 *    iodev - The iodev to add.
 *    profile - The profile of the iodev about to add.
 */
void cras_bt_device_append_iodev(struct cras_bt_device *device,
				 struct cras_iodev *iodev,
				 enum cras_bt_device_profile profile);

/* Removes an iodev from bt device.
 * Args:
 *    device - The device to remove iodev from.
 *    iodev - The iodev to remove.
 */
void cras_bt_device_rm_iodev(struct cras_bt_device *device,
			     struct cras_iodev *iodev);

/* Gets the active profile of the bt device. */
unsigned int
cras_bt_device_get_active_profile(const struct cras_bt_device *device);

/* Sets the active profile of the bt device. */
void cras_bt_device_set_active_profile(struct cras_bt_device *device,
				       unsigned int profile);

/* Switches profile after the active profile of bt device has changed and
 * enables bt iodev immediately. This function is used for profile switching
 * at iodev open.
 * Args:
 *    device - The bluetooth device.
 *    bt_iodev - The iodev triggers the reactivaion.
 */
int cras_bt_device_switch_profile_enable_dev(struct cras_bt_device *device,
					     struct cras_iodev *bt_iodev);

/* Switches profile after the active profile of bt device has changed. This
 * function is used when we want to switch profile without changing the
 * iodev's status.
 * Args:
 *    device - The bluetooth device.
 *    bt_iodev - The iodev triggers the reactivaion.
 */
int cras_bt_device_switch_profile(struct cras_bt_device *device,
				  struct cras_iodev *bt_iodev);

void cras_bt_device_start_monitor();

/* Checks if the device has an iodev for A2DP. */
int cras_bt_device_has_a2dp(struct cras_bt_device *device);

/* Returns true if and only if device has an iodev for A2DP and the bt device
 * is not opening for audio capture.
 */
int cras_bt_device_can_switch_to_a2dp(struct cras_bt_device *device);

/* Updates the volume to bt_device when a volume change event is reported. */
void cras_bt_device_update_hardware_volume(struct cras_bt_device *device,
					   int volume);

/* Notifies bt_device that a2dp connection is configured. */
void cras_bt_device_a2dp_configured(struct cras_bt_device *device);

/* Cancels any scheduled suspension of device. */
int cras_bt_device_cancel_suspend(struct cras_bt_device *device);

/* Schedules device to suspend after given delay. */
int cras_bt_device_schedule_suspend(
	struct cras_bt_device *device, unsigned int msec,
	enum cras_bt_device_suspend_reason suspend_reason);

/* Notifies bt device that audio gateway is initialized.
 * Args:
 *   device - The bluetooth device.
 * Returns:
 *   0 on success, error code otherwise.
 */
int cras_bt_device_audio_gateway_initialized(struct cras_bt_device *device);

/*
 * Notifies bt device about a profile no longer works. It could be caused
 * by initialize failure or fatal error has occurred.
 * Args:
 *    device - The bluetooth audio device.
 *    profile - The BT audio profile that has dropped.
 */
void cras_bt_device_notify_profile_dropped(struct cras_bt_device *device,
					   enum cras_bt_device_profile profile);

/*
 * Establishes SCO connection if it has not been established on the BT device.
 * Note: this function should be only used for hfp_alsa_io.
 * Args:
 *    device - The bluetooth device.
 *    codec - 1 for CVSD, 2 for mSBC
 * Returns:
 *   0 on success, error code otherwise.
 */
int cras_bt_device_get_sco(struct cras_bt_device *device, int codec);

/*
 * Closes SCO connection if the caller is the last user for the connection on
 * the BT device.
 * Note: this function should be only used for hfp_alsa_io.
 * Args:
 *   device - The bluetooth device.
 */
void cras_bt_device_put_sco(struct cras_bt_device *device);

#endif /* CRAS_BT_DEVICE_H_ */
