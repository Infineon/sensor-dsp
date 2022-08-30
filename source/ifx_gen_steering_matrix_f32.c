/***************************************************************************//**
* \file ifx_gen_steering_matrix_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_gen_steering_matrix_f32 function
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

void ifx_gen_steering_matrix_f32(float32_t ang_est_range, uint32_t num_angles,
                                 float32_t antenna_spacing_mm, float32_t lambda_mm,
                                 uint32_t num_ant, const arm_matrix_instance_f32* pDst)
{
    assert(ang_est_range > 0.0f);
    assert(ang_est_range <= (PI / 2.0f)); // expected in radians
    assert(num_angles > 1);
    assert(num_ant > 1);
    assert(pDst != NULL);

    float32_t* pOut = pDst->pData;
    assert(pOut != NULL);

    float32_t ang_est_resolution = (2.0f * ang_est_range) / ((float32_t)num_angles - 1.0F);

    // verify output matrix dimensions
    assert(pDst->numCols == num_ant);
    assert(pDst->numRows == num_angles);

    // The following steering phase formula is used
    // 2 * PI * d * sin (theta) / wavelength
    // the relationship between 'd', the wavelength 'lambda'
    // is obtained outside the loop and calculated with the
    // remaining constants as 'phase_calc_coefficient'.
    // The -1 multiplier is included to obtain a conjugate result
    float32_t phase_calc_coefficient = -1.0f * PI * ((2.0f * antenna_spacing_mm) / lambda_mm);

    for (uint32_t kk = 0; kk < num_angles; ++kk)
    {
        float32_t angle = -ang_est_range + ((float32_t)kk * ang_est_resolution);
        for (uint32_t jj = 0; jj < num_ant; ++jj)
        {
            float32_t phase = phase_calc_coefficient * (float32_t)jj * arm_sin_f32(angle);
            *pOut = arm_cos_f32(phase);
            pOut++;
            *pOut = arm_sin_f32(phase);
            pOut++;
        }
    }
}
