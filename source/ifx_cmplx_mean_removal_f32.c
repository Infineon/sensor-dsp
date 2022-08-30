/***************************************************************************//**
* \file ifx_cmplx_mean_removal_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_cmplx_mean_removal_f32 function
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

void ifx_cmplx_mean_removal_f32(cfloat32_t* v, uint32_t len)
{
    assert(v != NULL);

    cfloat32_t sum = 0.0f;

    cfloat32_t* pSrc = v;
    uint32_t cnt = len;
    while (cnt > 0U)
    {
        sum += *pSrc;
        pSrc++;

        /* Decrement loop counter */
        cnt--;
    }

    sum = sum / (float32_t)len;

    /* Initialize blkCnt with number of samples */
    cnt = len;
    pSrc = v;
    while (cnt > 0U)
    {
        *pSrc -= sum;
        pSrc++;

        /* Decrement loop counter */
        cnt--;
    }
}
