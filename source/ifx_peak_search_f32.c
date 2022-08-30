/***************************************************************************//**
* \file ifx_peak_search_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_peak_search_f32 function
*
*******************************************************************************
* \copyright
* Copyright 2022 Infineon Technologies AG
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "ifx_sensor_dsp.h"

/*
   ==============================================================================
    LOCAL FUNCTION PROTOTYPES
   ==============================================================================
 */

/** @brief Find prominence of a peak within a signasl array
 *
 * The function calculates the topgraphic prominence of a peak point (sample)
 * in a signal vector. A peak is a point that is larger than its left and
 * its right neighbor point.
 *
 * The function searches for the base of the peak determined by parsing the signal in both
 * directions from the peak sample point. the prmonence is then calculated by the magnitude
 * difference from the closer base point.
 *
 * @param [in] x         array for prominence calculation
 * @param [in] length    number of elements in array
 * @param [in] peak_index index of the queried peak sample.
 * @retval               (float32_t) topographic prominence of the input peak sample.
 */
static float32_t get_prominence(const float32_t* x, int32_t length, int32_t peak_index);

/** @brief get width of peak based on prominence
 *
 * The function calculates the width of a peak. the width is caculated in terms of number of
 * samples. The points crossing the peak prominence theshold are marked on both sides of the
 * peak and the difference is returned.
 *
 * @param [in] x         array for peak width calculation
 * @param [in] length    number of elements in array
 * @param [in] peak_index index of the queried peak sample.
 * @param [in] peak_prominence prominence value using which the width is to be calculated.
 * @retval               (int32_t) width of the peak in samples.
 */
static int32_t get_width(const float32_t* x, int32_t length, int32_t peak_index,
                         float32_t peak_prominence);

/*
   ==============================================================================
    LOCAL FUNCTIONS
   ==============================================================================
 */
int32_t ifx_peak_search_f32(const float32_t* x, int32_t length, int32_t* peak_indices,
                            int32_t max_peaks,
                            const ifx_peak_search_opts_f32_t* opts)
{
    float32_t height;
    float32_t threshold;
    int32_t distance;
    int32_t width;
    int32_t peak_count = -1;


    if (opts == NULL)
    { // default options
        height = -INFINITY;
        threshold = FLT_EPSILON;
        distance = 1;
        width = 1;
    }
    else
    {
        height = opts->height;
        threshold = opts->threshold;
        distance = opts->distance;
        width = opts->width;
    }

    int32_t last_peak_index = 0;
    // parse peak candidates from 2nd sample to 2nd last of input
    for (int32_t i = 1; i < (length-1); i++)
    {
        // check height (absolute threshold)
        if (x[i] <= height)
        {
            continue;
        }

        // check prominence threshold wrt neighboring samples
        float32_t sample_thresh = x[i] - threshold;
        if (sample_thresh < x[i-1])
        {
            continue;
        }
        if (sample_thresh < x[i+1])
        {
            continue;
        }

        // check distance with previous peak
        int32_t peak_count_increment = 1;
        if ((peak_count > 0) && (last_peak_index > (i - distance)))
        {
            if (x[last_peak_index] < x[i])
            {
                // potential candidate for replacing last peak
                peak_count_increment = 0;
            }
            else
            {
                continue;
            }
        }

        // check width
        if (width > 1)
        {
            float32_t peak_prominence = get_prominence(x, length, i);
            int32_t peak_width = get_width(x, length, i, peak_prominence * 0.5f);
            if (peak_width < width)
            {
                continue;
            }
        }

        // all checks ok. store peak information
        peak_count += peak_count_increment;
        peak_indices[peak_count] = i;
        last_peak_index = i;

        if ((peak_count+1) >= max_peaks)
        {
            break;
        }
    }
    return (peak_count+1);
}


//--------------------------------------------------------------------------------

static float32_t get_prominence(const float32_t* x, int32_t length, int32_t peak_index)
{
    float32_t peak_val = x[peak_index];
    float32_t min_lhs = peak_val;
    float32_t min_rhs = peak_val;

    // find lhs base of peak
    for (int32_t i = (peak_index-1); i >= 0; i--)
    {
        if (x[i] > peak_val)
        {
            break;
        }
        if (x[i] < min_lhs)
        {
            min_lhs = x[i];
        }
    }
    // find rhs base of peak
    for (int32_t i = (peak_index+1); i < length; i++)
    {
        if (x[i] > peak_val)
        {
            break;
        }
        if (x[i] < min_rhs)
        {
            min_rhs = x[i];
        }
    }
    // return prominence
    return (min_rhs > min_lhs) ? (peak_val-min_rhs) : (peak_val-min_lhs);
}


//---------------------------------------------------------------------------
static int32_t get_width(const float32_t* x, int32_t length, int32_t peak_index,
                         float32_t peak_prominence)
{
    float32_t width_magnitude = x[peak_index] - peak_prominence;
    int32_t width = 0;
    // check rhs crossing
    for (int32_t i = (peak_index+1); i < length; i++)
    {
        if (x[i] < width_magnitude)
        {
            width = i;
            break;
        }
    }

    // subtract lhs crossing
    for (int32_t i = (peak_index-1); i >= 0; i--)
    {
        if (x[i] < width_magnitude)
        {
            width -= i;
            break;
        }
    }
    return width;
}
