/* Copyright 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CRAS_SERVER_METRICS_H_
#define CRAS_SERVER_METRICS_H_

#include <stdbool.h>

#include "cras_iodev.h"
#include "cras_rstream.h"

extern const char kNoCodecsFoundMetric[];

/* Codes for how A2DP exit the audio output list.
 * IDLE - Disconnected while idle. The default disconnec reason without
 *     anything special.
 * WHILE_STREAMING - Disconnected while a2dp is streaming and audio
 *     thread didn't catch any socket error.
 * CONN_RESET - Disconnected while streaming and receiving ECONNRESET code.
 * LONG_TX_FAILURE - CRAS request the disconnection because of longer
 *     than 5 seconds of consecutive packet Tx failure.
 * TX_FATAL_ERROR - CRAS request the disconnection because kernel
 *     socket returns error code that CRAS treats as fatal error.
 */
enum A2DP_EXIT_CODE {
	A2DP_EXIT_IDLE,
	A2DP_EXIT_WHILE_STREAMING,
	A2DP_EXIT_CONN_RESET,
	A2DP_EXIT_LONG_TX_FAILURE,
	A2DP_EXIT_TX_FATAL_ERROR,
};

enum CRAS_METRICS_BT_SCO_ERROR_TYPE {
	CRAS_METRICS_SCO_SKT_SUCCESS = 0,
	CRAS_METRICS_SCO_SKT_CONNECT_ERROR = 1,
	CRAS_METRICS_SCO_SKT_OPEN_ERROR = 2,
	CRAS_METRICS_SCO_SKT_POLL_TIMEOUT = 3,
	CRAS_METRICS_SCO_SKT_POLL_ERR_HUP = 4,
};

/* Logs the error type happens when setting up SCO connection. This is mainly
 * used to track whether the setup of SCO connection succeeds and the frequency
 * of different errors. This will also be used to track if our fixes for these
 * errors address the issues we find.
 */
int cras_server_metrics_hfp_sco_connection_error(
	enum CRAS_METRICS_BT_SCO_ERROR_TYPE type);

/* Logs an enum representing which spec does HFP headset supports battery
 * indicator. Apple, HFP, none or both. */
int cras_server_metrics_hfp_battery_indicator(int battery_indicator_support);

/* Logs an enum representing the spec through which the battery level change
 * event reported. Apple or HFP.*/
int cras_server_metrics_hfp_battery_report(int battery_report);

/* Logs if connected HFP headset supports wideband speech. */
int cras_server_metrics_hfp_wideband_support(bool supported);

/* Logs the selected codec in HFP wideband connection. */
int cras_server_metrics_hfp_wideband_selected_codec(int codec);

/* Logs the number of packet loss per 1000 packets under HFP capture. */
int cras_server_metrics_hfp_packet_loss(float packet_loss_ratio);

/* Logs runtime of webrtc device pairs. */
int cras_server_metrics_webrtc_devs_runtime(const struct cras_iodev *in_dev,
					    const struct cras_iodev *out_dev,
					    const struct timespec *rtc_start_ts);

/* Logs runtime of a device. */
int cras_server_metrics_device_runtime(struct cras_iodev *iodev);

/* Logs the gain of a device. */
int cras_server_metrics_device_gain(struct cras_iodev *iodev);

/* Logs the volume of a device. */
int cras_server_metrics_device_volume(struct cras_iodev *iodev);

/* Logs the status of Noise Cancellation of a supported device. */
int cras_server_metrics_device_noise_cancellation_status(
	struct cras_iodev *iodev, int status);

/* Logs the device type when cras clients request to set aec ref. */
int cras_server_metrics_set_aec_ref_device_type(struct cras_iodev *iodev);

/* Logs the highest delay time of a device. */
int cras_server_metrics_highest_device_delay(
	unsigned int hw_level, unsigned int largest_cb_level,
	enum CRAS_STREAM_DIRECTION direction);

/* Logs the highest hardware level of a device. */
int cras_server_metrics_highest_hw_level(unsigned hw_level,
					 enum CRAS_STREAM_DIRECTION direction);

/* Logs the number of underruns of a device. */
int cras_server_metrics_num_underruns(unsigned num_underruns);

/* Logs the missed callback event. */
int cras_server_metrics_missed_cb_event(struct cras_rstream *stream);

/* Logs information when a stream creates. */
int cras_server_metrics_stream_create(const struct cras_rstream_config *config);

/* Logs information when a stream destroys. */
int cras_server_metrics_stream_destroy(const struct cras_rstream *stream);

/* Logs the number of busyloops for different time periods. */
int cras_server_metrics_busyloop(struct timespec *ts, unsigned count);

/* Logs the length of busyloops. */
int cras_server_metrics_busyloop_length(unsigned length);

/* Logs the code how A2DP exit from the audio output list. Used to
 * track the ratio of normal and abnormal scenarios and break down
 * of individual reasons that causes the exit. */
int cras_server_metrics_a2dp_exit(enum A2DP_EXIT_CODE code);

/* Logs A2dp write failure periods that exceed 20ms all summed up and then
 * divide by the stream time. The final ratio is normalized by multipling
 * 10^9 for metric logging. */
int cras_server_metrics_a2dp_20ms_failure_over_stream(unsigned num);

/* Logs A2dp write failure periods that exceed 100ms all summed up and then
 * divide by the stream time. The final ratio is normalized by multipling
 * 10^9 for metric logging. */
int cras_server_metrics_a2dp_100ms_failure_over_stream(unsigned num);

/* Initialize metrics logging stuff. */
int cras_server_metrics_init();

#endif /* CRAS_SERVER_METRICS_H_ */
