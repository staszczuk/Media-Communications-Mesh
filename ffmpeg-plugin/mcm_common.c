/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mcm_common.h"
#include "libavutil/opt.h"
#include <bsd/string.h>

/* Parse MCM connection parameters and fill the structure */
int mcm_parse_conn_param(mcm_conn_param *param, transfer_type type,
                         char *ip_addr, char *port, char *protocol_type,
                         char *payload_type, char *socket_name, int interface_id)
{
    param->type = type;

    strlcpy(param->remote_addr.ip, ip_addr, sizeof(param->remote_addr.ip));

    if (type == is_tx)
        strlcpy(param->remote_addr.port, port, sizeof(param->remote_addr.port));
    else
        strlcpy(param->local_addr.port, port, sizeof(param->local_addr.port));

    /* protocol type */
    if (!strcmp(protocol_type, "memif")) {
        param->protocol = PROTO_MEMIF;
        snprintf(param->memif_interface.socket_path, sizeof(param->memif_interface.socket_path),
            "/run/mcm/mcm_memif_%s.sock", socket_name ? socket_name : "0");
        param->memif_interface.interface_id = interface_id;
        param->memif_interface.is_master = (type == is_tx) ? 1 : 0;
    } else if (!strcmp(protocol_type, "udp")) {
        param->protocol = PROTO_UDP;
    } else if (!strcmp(protocol_type, "tcp")) {
        param->protocol = PROTO_TCP;
    } else if (!strcmp(protocol_type, "http")) {
        param->protocol = PROTO_HTTP;
    } else if (!strcmp(protocol_type, "grpc")) {
        param->protocol = PROTO_GRPC;
    } else {
        param->protocol = PROTO_AUTO;
    }

    /* payload type */
    if (!strcmp(payload_type, "st20")) {
        param->payload_type = PAYLOAD_TYPE_ST20_VIDEO;
    } else if (!strcmp(payload_type, "st22")) {
        param->payload_type = PAYLOAD_TYPE_ST22_VIDEO;
        param->payload_codec = PAYLOAD_CODEC_JPEGXS;
    } else if (!strcmp(payload_type, "st30")) {
        param->payload_type = PAYLOAD_TYPE_ST30_AUDIO;
    } else if (!strcmp(payload_type, "st40")) {
        param->payload_type = PAYLOAD_TYPE_ST40_ANCILLARY;
    } else if (!strcmp(payload_type, "rtsp")) {
        param->payload_type = PAYLOAD_TYPE_RTSP_VIDEO;
    } else {
        av_log(NULL, AV_LOG_ERROR, "Unknown payload type\n");
        return AVERROR(EINVAL);
    }

    return 0;
}
