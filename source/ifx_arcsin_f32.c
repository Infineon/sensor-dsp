/***************************************************************************//**
* \file ifx_arcsin_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_arcsin_f32 function
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

arm_status ifx_arcsin_f32(float32_t x, float32_t* result)
{
    arm_status status = ARM_MATH_SUCCESS;
    if (x >= 1.0F)
    {
        *result = PI_2_F32;
    }
    else if (x <= -1.0F)
    {
        *result = -PI_2_F32;
    }
    else
    {
        float32_t y = 0.0F;
        status = arm_sqrt_f32(1.0F - (x * x), &y);
        if (ARM_MATH_SUCCESS == status)
        {
            (void)arm_atan2_f32(x, y, result);
        }
        else
        {
            *result = 0.0F;
        }
    }
    return status;
}
