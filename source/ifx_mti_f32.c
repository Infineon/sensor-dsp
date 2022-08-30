/***************************************************************************//**
* \file ifx_mti_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_mti_f32 function
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

void ifx_mti_init_f32(ifx_mti_inst_f32* inst,
                      float32_t alpha,
                      uint32_t len,
                      float32_t* historical_data)
{
    assert(inst);
    assert(historical_data);
    assert(inst->historical_data);
    assert(inst->alpha >= 0);
    assert(inst->alpha <= 1);

    inst->alpha = alpha;
    inst->len = len;
    inst->historical_data = historical_data;
    for (uint32_t i = 0; i < len; ++i)
    {
        historical_data[i] = 0.0f;
    }
}


void ifx_mti_f32(const ifx_mti_inst_f32* inst,
                 const float32_t* in_data,
                 float32_t* out_data)
{
    assert(in_data);
    assert(out_data);
    assert(inst->historical_data);
    assert(inst->alpha >= 0);
    assert(inst->alpha <= 1);

    const float32_t alpha = inst->alpha;
    const uint32_t len = inst->len;

    /* The out_data is updated using y(t) = x(t) - h(t-1) */
    arm_sub_f32(in_data, inst->historical_data, out_data, len);

    for (uint32_t j = 0; j < len; ++j)
    {
        /* The historical data is updated using h(t) = alpha * y(t) + h(t-1) */
        inst->historical_data[j] += alpha * out_data[j];
    }
}
