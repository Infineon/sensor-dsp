/***************************************************************************//**
* \file ifx_window_hamming_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_window_hamming_f32 function
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

void ifx_window_hamming_f32(float32_t* win, uint32_t len)
{
    assert(win != NULL);
    assert(len > 1);

    const float32_t M = 1.0F / ((float32_t)len - 1.0F);

    for (uint32_t n = 0; n < len; ++n)
    {
        win[n] = 0.54F - (0.46F * arm_cos_f32(2.0F * PI * (float32_t)n * M));
    }
}
