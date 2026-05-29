// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2023 MediaTek Inc.
 */

#include "uci_config.h"
#include <stdbool.h>

typedef struct {
    bool enable;
    bool hqos;
    int txq_num;
    char scheduling[8];
    int sch0_bw;
    int sch1_bw;
    int qos_toggle;
} global_param;

typedef struct {
    int id;
    int minrate;
    int maxrate;
    int weight;
    int resv;
} queue_param;


