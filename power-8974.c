/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Copyright (C) 2018 The LineageOS Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * *    * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define LOG_NIDEBUG 0

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>

#define LOG_TAG "QCOM PowerHAL"
#include <log/log.h>
#include <hardware/hardware.h>
#include <hardware/power.h>

#include "utils.h"
#include "metadata-defs.h"
#include "hint-data.h"
#include "performance.h"
#include "power-common.h"

static int display_hint_sent;
static int display_hint2_sent;
static int first_display_off_hint;
extern int display_boost;

static int current_power_profile = PROFILE_BALANCED;

static int profile_high_performance[] = {
    CPUS_ONLINE_MIN_4,
    0x0901,
    CPU0_MIN_FREQ_TURBO_MAX,
    CPU1_MIN_FREQ_TURBO_MAX,
    CPU2_MIN_FREQ_TURBO_MAX,
    CPU3_MIN_FREQ_TURBO_MAX
};

static int profile_power_save[] = {
    0x0A03,
    CPUS_ONLINE_MAX_LIMIT_2,
    CPU0_MAX_FREQ_NONTURBO_MAX,
    CPU1_MAX_FREQ_NONTURBO_MAX,
    CPU2_MAX_FREQ_NONTURBO_MAX,
    CPU3_MAX_FREQ_NONTURBO_MAX
};

static int profile_bias_power[] = {
    0x0A03,
    CPU0_MAX_FREQ_NONTURBO_MAX,
    CPU1_MAX_FREQ_NONTURBO_MAX,
    CPU1_MAX_FREQ_NONTURBO_MAX,
    CPU2_MAX_FREQ_NONTURBO_MAX
};

static int profile_bias_performance[] = {
    CPU0_MIN_FREQ_NONTURBO_MAX + 1,
    CPU1_MIN_FREQ_NONTURBO_MAX + 1,
    CPU2_MIN_FREQ_NONTURBO_MAX + 1,
    CPU2_MIN_FREQ_NONTURBO_MAX + 1
};

#ifdef INTERACTION_BOOST
int get_number_of_profiles() {
    return 5;
}
#endif

static void set_power_profile(int profile) {

    if (profile == current_power_profile)
        return;

    ALOGV("%s: Profile=%d", __func__, profile);

    if (current_power_profile != PROFILE_BALANCED) {
        undo_hint_action(DEFAULT_PROFILE_HINT_ID);
        ALOGV("%s: Hint undone", __func__);
    }

    if (profile == PROFILE_POWER_SAVE) {
        perform_hint_action(DEFAULT_PROFILE_HINT_ID, profile_power_save,
                ARRAY_SIZE(profile_power_save));
        ALOGD("%s: Set powersave mode", __func__);

    } else if (profile == PROFILE_HIGH_PERFORMANCE) {
        perform_hint_action(DEFAULT_PROFILE_HINT_ID, profile_high_performance,
                ARRAY_SIZE(profile_high_performance));
        ALOGD("%s: Set performance mode", __func__);

    } else if (profile == PROFILE_BIAS_POWER) {
        perform_hint_action(DEFAULT_PROFILE_HINT_ID, profile_bias_power,
                ARRAY_SIZE(profile_bias_power));
        ALOGD("%s: Set bias power mode", __func__);

    } else if (profile == PROFILE_BIAS_PERFORMANCE) {
        perform_hint_action(DEFAULT_PROFILE_HINT_ID, profile_bias_performance,
                ARRAY_SIZE(profile_bias_performance));
        ALOGD("%s: Set bias perf mode", __func__);

    }

    current_power_profile = profile;
}

void interaction(int duration, int num_args, int opt_list[]);

int power_hint_override(power_hint_t hint, void *data)
{
    if (hint == POWER_HINT_SET_PROFILE) {
        set_power_profile(*(int32_t *)data);
        return HINT_HANDLED;
    }

    // Skip other hints in high/low power modes
    if (current_power_profile == PROFILE_POWER_SAVE ||
            current_power_profile == PROFILE_HIGH_PERFORMANCE) {
        return HINT_HANDLED;
    }

    if (hint == POWER_HINT_LAUNCH) {
        int duration = 2000;
        int resources[] = { CPUS_ONLINE_MIN_3,
            CPU0_MIN_FREQ_TURBO_MAX, CPU1_MIN_FREQ_TURBO_MAX,
            CPU2_MIN_FREQ_TURBO_MAX, CPU3_MIN_FREQ_TURBO_MAX };

        interaction(duration, ARRAY_SIZE(resources), resources);

        return HINT_HANDLED;
    }

    if (hint == POWER_HINT_INTERACTION) {
        int duration = 500, duration_hint = 0;
        static struct timespec s_previous_boost_timespec;
        struct timespec cur_boost_timespec;
        long long elapsed_time;

        if (data) {
            duration_hint = *((int *)data);
        }

        duration = duration_hint > 0 ? duration_hint : 500;

        clock_gettime(CLOCK_MONOTONIC, &cur_boost_timespec);
        elapsed_time = calc_timespan_us(s_previous_boost_timespec, cur_boost_timespec);
        if (elapsed_time > 750000)
            elapsed_time = 750000;
        // don't hint if it's been less than 250ms since last boost
        // also detect if we're doing anything resembling a fling
        // support additional boosting in case of flings
        else if (elapsed_time < 250000 && duration <= 750)
            return HINT_HANDLED;

        s_previous_boost_timespec = cur_boost_timespec;

        int resources[] = { (duration >= 2000 ? CPUS_ONLINE_MIN_3 : CPUS_ONLINE_MIN_2),
            0x20F, 0x30F, 0x40F, 0x50F };

        if (duration)
            interaction(duration, ARRAY_SIZE(resources), resources);

        return HINT_HANDLED;
    }

    return HINT_NONE;
}

int set_interactive_override(int on)
{
    char governor[80];

    if (get_scaling_governor(governor, sizeof(governor)) == -1) {
        ALOGE("Can't obtain scaling governor.");

        return HINT_NONE;
    }

    if (!on) {
        /* Display off. */
        /*
         * We need to be able to identify the first display off hint
         * and release the current lock holder
         */
        if (display_boost) {
            if (!first_display_off_hint) {
                undo_initial_hint_action();
                first_display_off_hint = 1;
            }
            /* used for all subsequent toggles to the display */
            if (!display_hint2_sent) {
                undo_hint_action(DISPLAY_STATE_HINT_ID_2);
                display_hint2_sent = 1;
            }
        }

        if ((strncmp(governor, ONDEMAND_GOVERNOR, strlen(ONDEMAND_GOVERNOR)) == 0) &&
                (strlen(governor) == strlen(ONDEMAND_GOVERNOR))) {
            int resource_values[] = {MS_500, SYNC_FREQ_600, OPTIMAL_FREQ_600, THREAD_MIGRATION_SYNC_OFF};

            if (!display_hint_sent) {
                perform_hint_action(DISPLAY_STATE_HINT_ID,
                        resource_values, sizeof(resource_values)/sizeof(resource_values[0]));
                display_hint_sent = 1;
            }

            return HINT_HANDLED;
        }
    } else {
        /* Display on */
        if (display_boost && display_hint2_sent) {
            int resource_values2[] = {CPUS_ONLINE_MIN_2};
            perform_hint_action(DISPLAY_STATE_HINT_ID_2,
                    resource_values2, sizeof(resource_values2)/sizeof(resource_values2[0]));
            display_hint2_sent = 0;
        }

        if ((strncmp(governor, ONDEMAND_GOVERNOR, strlen(ONDEMAND_GOVERNOR)) == 0) &&
                (strlen(governor) == strlen(ONDEMAND_GOVERNOR))) {
            undo_hint_action(DISPLAY_STATE_HINT_ID);
            display_hint_sent = 0;

            return HINT_HANDLED;
        }
    }

    return HINT_NONE;
}
