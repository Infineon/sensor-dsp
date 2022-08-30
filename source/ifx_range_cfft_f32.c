/***************************************************************************//**
* \file ifx_range_cfft_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_range_cfft_f32 function
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

int32_t ifx_range_cfft_f32(cfloat32_t* frame,
                           bool mean_removal,
                           const float32_t* win,
                           uint16_t num_samples_per_chirp,
                           uint16_t num_chirps_per_frame)
{
    assert(frame != NULL);

    static arm_cfft_instance_f32 cfft = { 0 };
    if (cfft.fftLen != num_samples_per_chirp)
    {
        if (arm_cfft_init_f32(&cfft, num_samples_per_chirp) != ARM_MATH_SUCCESS)
        {
            return IFX_SENSOR_DSP_ARGUMENT_ERROR;
        }
    }

    for (uint32_t chirp_idx = 0; chirp_idx < num_chirps_per_frame; ++chirp_idx)
    {
        if (mean_removal)
        {
            ifx_cmplx_mean_removal_f32(frame, num_samples_per_chirp);
        }

        if (win != NULL)
        {
            arm_cmplx_mult_real_f32((float32_t*)frame, win, (float32_t*)frame,
                                    num_samples_per_chirp);
        }

        arm_cfft_f32(&cfft, (float32_t*)frame, 0, 1);

        frame += num_samples_per_chirp;
    }

    return IFX_SENSOR_DSP_STATUS_OK;
}
