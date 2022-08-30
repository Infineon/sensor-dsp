/***************************************************************************//**
* \file ifx_doppler_cfft_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_doppler_cfft_f32 function
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

int32_t ifx_doppler_cfft_f32(cfloat32_t* range,
                             cfloat32_t* doppler,
                             bool mean_removal,
                             const float32_t* win,
                             uint16_t num_range_bins,
                             uint16_t num_chirps_per_frame)
{
    assert(range != NULL);
    assert(doppler != NULL);

    static arm_cfft_instance_f32 cfft = { 0 };
    if (cfft.fftLen != num_chirps_per_frame)
    {
        if (arm_cfft_init_f32(&cfft, num_chirps_per_frame) != ARM_MATH_SUCCESS)
        {
            return IFX_SENSOR_DSP_ARGUMENT_ERROR;
        }
    }

    arm_matrix_instance_f32 range_matrix =
    {
        num_chirps_per_frame,
        num_range_bins,
        (float32_t*)range
    };
    arm_matrix_instance_f32 doppler_matrix =
    {
        num_range_bins,
        num_chirps_per_frame,
        (float32_t*)doppler
    };

    (void)arm_mat_cmplx_trans_f32(&range_matrix, &doppler_matrix);

    for (uint32_t range_idx = 0; range_idx < num_range_bins; ++range_idx)
    {
        if (mean_removal)
        {
            ifx_cmplx_mean_removal_f32(doppler, num_chirps_per_frame);
        }

        if (win != NULL)
        {
            arm_cmplx_mult_real_f32((float32_t*)doppler,
                                    win,
                                    (float32_t*)doppler,
                                    num_chirps_per_frame);
        }

        arm_cfft_f32(&cfft, (float32_t*)doppler, 0, 1);

        doppler += num_chirps_per_frame;
    }

    return IFX_SENSOR_DSP_STATUS_OK;
}
