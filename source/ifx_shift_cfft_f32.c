/***************************************************************************//**
* \file ifx_shift_cfft_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_shift_cfft_f32 function
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

void ifx_shift_cfft_f32(cfloat32_t* v, uint32_t len, uint32_t dim)
{
    assert(v != NULL);

    for (uint32_t i = 0; i < dim; ++i)
    {
        ifx_rotate_f32((float32_t*)v, len * 2U, ((len % 2U) == 1U) ? (len + 1U) : len);
        v += len;
    }
}
