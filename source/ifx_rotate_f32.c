/***************************************************************************//**
* \file ifx_rotate_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_rotate_f32 function
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

void ifx_rotate_f32(float32_t* v, uint32_t len, uint32_t k)
{
    assert(v != NULL);

    while (k > 0U)
    {
        const float32_t temp = v[0];
        for (uint32_t j = 0; j < (len - 1U); ++j)
        {
            v[j] = v[j + 1U];
        }
        v[len - 1U] = temp;
        --k;
    }
}
