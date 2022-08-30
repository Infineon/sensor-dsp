/***************************************************************************//**
* \file ifx_angle_dbf_f32.c
*
* \brief
* This file contains the implementation for the
* ifx_angle_dbf_f32 function
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

arm_status ifx_angle_dbf_f32(const arm_matrix_instance_f32* pInput,
                             const arm_matrix_instance_f32* pSteering,
                             arm_matrix_instance_f32* pOutput)
{
    // corresponds to number of antennas
    assert(pSteering->numCols == pInput->numRows);

    // corresponds to number of samples
    assert(pInput->numCols == pOutput->numCols);

    // corresponds to number of angles
    assert(pSteering->numRows == pOutput->numRows);

    return arm_mat_cmplx_mult_f32(pSteering, pInput, pOutput);
}
