/***************************************************************************//**
* \file ifx_sensor_dsp.h
*
* \brief
* The SensorDSP library provides signal processing functions to support the
* implementation of sensor applications.
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

/**
 * \addtogroup group_sensor_dsp Sensor-DSP
 * \{
 *
 * The Sensor-DSP library provides signal processing functions required to support the
 * implementation of different sensor applications, i.e. radar gesture recognition, vital sensing
 * and presence detection.
 *
 * <b>Features:</b>
 * * Range and Doppler FFT
 * * FFT windowing related functions
 * * Detection related functions
 * * Angle estimation related functions
 *
 * The Sensor-DSP library builds on top of the standard ARM CMSIS-DSP library.
 * \image html software-architecture-overview.png
 *
 * \section subsection_sensor_dsp_quickstart Quick Start Guide
 *
 * The below steps describe the simplest way of adding the Sensor-DSP
 * library into an application.
 *
 * 1. Open/Create an application where to add the Sensor-DSP library.
 *
 * 2. Add the Sensor-DSP library to your project.
 * Launch ModusToolbox Library Manager and enable the Sensor-DSP library.
 * This step is required only if the ModusToolbox IDE is used. Otherwise, ensure
 * the Sensor-DSP library is included in your project as ModusToolbox dependency.
 *
 * 3. Include Sensor-DSP in the main.c file:
 * \snippet snippet/main.c snippet_required_includes
 *
 * 4. The following code shows a simple example calculating the Doppler FFT on radar data frame:
 * \snippet snippet/main.c snippet_sensor_dsp
 */

#ifndef IFX_SENSOR_DSP_H_
#define IFX_SENSOR_DSP_H_

#include <assert.h>
#include <complex.h>
#include <stdbool.h>

#include "arm_math.h"

/************************************** Macros *******************************************/
/** Result code indicating successful operation. */
#define IFX_SENSOR_DSP_STATUS_OK                      (0)
/** Result code indicating a not supported argument value */
#define IFX_SENSOR_DSP_ARGUMENT_ERROR                 (1)

/** The speed of light in vacuum, in meters per second */
#define IFX_LIGHT_SPEED_M_S      (299792458.0F)

/** SensorDSP major version */
#define IFX_SENSOR_DSP_VERSION_MAJOR        (0)

/** SensorDSP minor version */
#define IFX_SENSOR_DSP_VERSION_MINOR        (5)

/** SensorDSP version */
#define IFX_SENSOR_DSP_VERSION_REVERSION    (0)

/** Positive infinity. */
#ifndef POS_INF_F32
#define POS_INF_F32 (1.0F/0.0F)
#endif

/** Negative infinity. */
#ifndef NEG_INF_F32
#define NEG_INF_F32 (-1.0F/0.0F)
#endif

/** PI/2 */
#ifndef PI_2_F32
#define PI_2_F32 (1.570796370506F)
#endif

/** PI/4 */
#ifndef PI_4_F32
#define PI_4_F32 (0.78539816339F)
#endif

/** Macro used to assign real part of a complex floating number */
#define CREAL_F32(x)   (((float32_t *)       &(x))[0])

/** Macro used to assign imaginary part of a complex floating number */
#define CIMAG_F32(x)   (((float32_t *)       &(x))[1])

/**********************************  Type definitions ************************************/

/** Complex float number type */
typedef _Complex float cfloat32_t;

/**
 * @brief Instance structure for the MTI calculation.
 */
typedef struct
{
    /**
     *  Size of in_data, out_data, historical_data
     */
    uint32_t len;

    /**
     * Exponential moving average parameter (0 highest historical influence,
     * 1 lowest)
     */
    float32_t alpha;

    /**
     * Pointer to historical data (size must be equal to len).
     * Initiate with first data or zeros
     */
    float32_t* historical_data;
} ifx_mti_inst_f32;

/**
 * @brief Peak search options.
 */
typedef struct
{
    float32_t height; /**< Required height of peaks */
    float32_t threshold; /**< Required threshold of peaks, the vertical distance to its neighboring
                            samples */
    int32_t distance; /**< Required minimal horizontal distance (>= 1) in samples between
                         neighbouring peaks */
    int32_t width; /**< Required width of peaks in samples */
} ifx_peak_search_opts_f32_t;

/******************************* Function prototypes *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Substracts the calculated array mean from each element of the floating point array.
 *
 * @param[in] v Pointer to input array
 * @param[in] len Number of elements in array
 * @return none
 */
void ifx_mean_removal_f32(float32_t* v, uint32_t len);


/**
 * @brief Substracts the calculated array mean from each element of the complex floating point
 * array.
 *
 * @param[in] v Pointer to input array
 * @param[in] len Number of elements in array
 * @return none
 */
void ifx_cmplx_mean_removal_f32(cfloat32_t* v, uint32_t len);


/**
 * @brief Calculate range FFT from real floating point raw radar data.
 * Perform optional mean removal and windowing on the raw radar data prior to 1D FFT.
 * The caller must allocate the memory for the frame and range arrays.
 *
 * @param[inout] frame Pointer to raw radar real data of shape
 * [num_chirps_per_frame][num_adc_samples]
 * @note frame is modified by this function if mean_removal is true and/or win != NULL
 * @param[out] range Pointer to transformed range complex data of shape
 * [num_chirps_per_frame][num_adc_samples/2]
 * @param[in] mean_removal If true, remove mean along samples before 1D FFT
 * @param[in] win Window to be applied to the raw radar data prior 1D FFT
 * @note Can be NULL if not windowing is desired
 * @param[in] num_samples_per_chirp Number of samples per radar chirp
 * @param[in] num_chirps_per_frame Number of chirps per radar frame
 * @return - \ref IFX_SENSOR_DSP_STATUS_OK : Operation successful
 *         - \ref IFX_SENSOR_DSP_ARGUMENT_ERROR : Not supported FFT length (num_samples_per_chirp)
 */
int32_t ifx_range_fft_f32(float32_t* frame,
                          cfloat32_t* range,
                          bool mean_removal,
                          const float32_t* win,
                          uint16_t num_samples_per_chirp,
                          uint16_t num_chirps_per_frame);


/**
 * @brief Calculate range FFT from complex floating point raw radar data.
 * Perform optional mean removal and windowing on the ADC data prior to 1D FFT
 * The caller must allocate the memory for the frame and range arrays.
 *
 * @param[inout] frame Pointer to raw radar complex data of shape
 * [num_chirps_per_frame][num_adc_samples]
 * @note Processing by this function occurs in-place. The raw radar complex data is replaced by the
 * calculated range FFT
 * @param[in] mean_removal If true, remove mean along samples before 1D FFT
 * @param[in] win Pointer to window to be applied to the raw radar data prior 1D FFT
 * @note Can be NULL if not windowing is desired
 * @param[in] num_samples_per_chirp Number of samples per radar chirp
 * @param[in] num_chirps_per_frame Number of chirps per radar frame
 * @return - \ref IFX_SENSOR_DSP_STATUS_OK : Operation successful
 *         - \ref IFX_SENSOR_DSP_ARGUMENT_ERROR : Not supported FFT length (num_samples_per_chirp)
 */
int32_t ifx_range_cfft_f32(cfloat32_t* frame,
                           bool mean_removal,
                           const float32_t* win,
                           uint16_t num_samples_per_chirp,
                           uint16_t num_chirps_per_frame);


/**
 * @brief Calculate doppler FFT from range data.
 * Perform optional mean removal and windowing on the range data prior to 1D FFT.
 * The caller must allocate the memory for the range and doppler arrays.
 *
 * @param[in] range Pointer to range complex data of shape
 * [num_chirps_per_frame][num_range_bins]
 * @param[out] doppler Pointer to transformed range doppler complex data of shape
 * [num_range_bins][num_doppler_bins]
 * @param[in] mean_removal If true, remove mean along samples before 1D FFT
 * @param[in] win Pointer to window to be applied to the range data prior 1D FFT
 * @note Can be NULL if not windowing is desired
 * @param[in] num_range_bins Number of range bins per chirp
 * @param[in] num_chirps_per_frame Number of chirps per radar frame
 * @return - \ref IFX_SENSOR_DSP_STATUS_OK : Operation successful
 *         - \ref IFX_SENSOR_DSP_ARGUMENT_ERROR : Not supported FFT length (num_chirps_per_frame)
 */
int32_t ifx_doppler_cfft_f32(cfloat32_t* range,
                             cfloat32_t* doppler,
                             bool mean_removal,
                             const float32_t* win,
                             uint16_t num_range_bins,
                             uint16_t num_chirps_per_frame);


/**
 * @brief Generate a symmetric Blackman window.
 *
 * The function generates a symmetric Blackman window \f$w\f$ of length \f$N\f$
 * using the following formula
   \f[
   w_n = \mathrm{0.42} - 0.5 \cos\left( \frac{2\pi n}{N-1} \right) +
   0.08 \cos\left( \frac{4\pi n}{N-1} \right),\qquad \mathrm{0} \le\ n < \mathrm{N}
   \f]
 * The values are populated in the output array 'window' whose pointer is passed as a parameter.
 * The caller is responsible to allocate memory for output. For
 * a window of length \f$ N \f$ a memory of \f$ N*sizeof(float32_t) \f$ should be allocated.
 *
 * @param[inout] win Starting pointer to float32_t array to be populated with output
 * @param[in] len Length of window to be generated
 * @return None
 */
void ifx_window_blackman_f32(float32_t* win, uint32_t len);


/**
 * @brief Generate a symmetric four-term Blackman-Harris window.
 *
 * The function generates a symmetric four-term Blackman-Harris window \f$w\f$ of length \f$N\f$
 * using the following formula
   \f[
   w_n = \mathrm{a_0} - \mathrm{a_1} \cos\left( \frac{2\pi n}{N-1} \right) +
   \mathrm{a_2} \cos\left( \frac{4\pi n}{N-1} \right) -
   \mathrm{a_3} \cos\left( \frac{6\pi n}{N-1} \right),\qquad \mathrm{0} \le\ n < \mathrm{N}
   \f]
 *
 * where the coefficient values are
 * Coefficient          |  Value
 * ---------------------|----------------
 * \f$\mathrm{a}_0\f$   | 0.35875
 * \f$\mathrm{a}_1\f$   | 0.48829
 * \f$\mathrm{a}_2\f$   | 0.14128
 * \f$\mathrm{a}_3\f$   | 0.01168
 *
 * The window values are populated in the output array 'window' whose pointer is passed as a
 * parameter.
 * The caller is responsible to allocate memory for output. For
 * a window of length \f$ N \f$ a memory of \f$ N*sizeof(float32_t) \f$ should be allocated.
 *
 * @param[inout] win Starting pointer to float32_t array to be populated with output
 * @param[in] len Length of window to be generated
 * @return None
 */
void ifx_window_blackmanharris_f32(float32_t* win, uint32_t len);


/**
 * @brief Generate a symmetric Hamming window.
 *
 * The function generates a symmetric Hamming window \f$w\f$ of length \f$N\f$
 * using the following formula
   \f[
   w_n = \mathrm{0.54} - \mathrm{0.46} \cos\left( \frac{2\pi n}{N-1}\right),
   \qquad \mathrm{0} \le\ n < \mathrm{N}
   \f]
 *
 * The values are populated in the output array 'window' whose pointer is passed as a parameter.
 * The caller is responsible to allocate memory for output. For
 * a window of length \f$ N \f$ a memory of \f$ N*sizeof(float32_t) \f$ should be allocated.
 *
 * @param[inout] win Starting pointer to float32_t array to be populated with output
 * @param[in] len Length of window to be generated
 * @return None
 */
void ifx_window_hamming_f32(float32_t* win, uint32_t len);


/**
 * @brief Generate a symmetric Hann window.
 *
 * The function generates a symmetric Hann window \f$w\f$ of length \f$N\f$
 * using the following formula
   \f[
   w_n = \mathrm{0.5} \left(\mathrm{1} - \cos\left( \frac{2\pi n}{N-1}
   \right)\right),\qquad \mathrm{0} \le\ n < \mathrm{N}
   \f]
 * The values are populated in the output array 'window' whose pointer is passed as a parameter.
 * The caller is responsible to allocate memory for output. For
 * a window of length \f$ N \f$ a memory of \f$ N*sizeof(float32_t) \f$ should be allocated.
 *
 * @param[inout] win Starting pointer to float32_t array to be populated with output
 * @param[in] len Length of window to be generated
 * @return None
 */
void ifx_window_hann_f32(float32_t* win, uint32_t len);


/**
 * @brief Generate a steering vector for AOA estimation given the theta range, number of beams, and
 * number of antennas
 * Defines a method for generating steering vector data input
 * The generated steering vector will span from -angEstRange to angEstRange with increments of
 * ang_est_resolution
 * The generated steering vector is already transposed and can be used directly for all further AOA
 * estimations (bartlett/capon)
 *
 * @param[in] ang_est_range Single side angle of view in radians. zero is directly
 * perpendicular to antenna plane
 * @param[in] num_angles Number of beams
 * @param[in] antenna_spacing_mm Radar antenna spacing in mm
 * @param[in] lambda_mm Wavelength corresponding to Radar operating frequency
 * @param[in] num_ant The number of receiving antennas
 * @param[inout] pDst Pointer to output steering matrix array allocated by the user
 * [ rows, columns ] = [ num_angles, num_antennas ]
 * @return none
 */
void ifx_gen_steering_matrix_f32(float32_t ang_est_range, uint32_t num_angles,
                                 float32_t antenna_spacing_mm, float32_t lambda_mm,
                                 uint32_t num_ant, const arm_matrix_instance_f32* pDst);


/**
 * @brief Generate a phase shift beam forming matrix
 * Defines a method for generating a phase shift beam forming matrix by multiplying the steering
 * matrix by the input.
 *
 * @param[in] pInput Pointer to input matrix
 * @param[in] pSteering Pointer to steering matrix
 * @param[inout] pOutput Pointer to beamforming matrix
 * @return status flag
 */
arm_status ifx_angle_dbf_f32(const arm_matrix_instance_f32* pInput,
                             const arm_matrix_instance_f32* pSteering,
                             arm_matrix_instance_f32* pOutput);


/**
 * @brief Shift the array of complex numbers
 *
 * @param[inout] v Pointer to input array
 * @param[in] len Number of elements in array
 * @param[in] dim Number of matrix dimensions
 * @return None
 */
void ifx_shift_cfft_f32(cfloat32_t* v, uint32_t len, uint32_t dim);


/**
 * @brief Initializes MTI control structure
 *
 * @param[in] inst Pointer to configuration and history data previouly allocated
 * @param[in] alpha Exponential moving average parameter (0 highest historical influence,
 * 1 lowest)
 * @param[in] len Size of historical_data
 * @param[in] historical_data Pointer to historical data
 */
void ifx_mti_init_f32(ifx_mti_inst_f32* inst,
                      float32_t alpha,
                      uint32_t len,
                      float32_t* historical_data);


/**
 * @brief (M)oving (T)arget (I)ndication
 *
 * This algorithm work on exponential moving average to compute MTI.
 * <BR>
 * As an input can be 1D/2D/complex number and computation will retain same
 * only data size must be adequad for given type
 * @code
 * for(index i = 0; i < len; i++)
 * {
 *    h = historical_data[i]
 *    x = in_data[i]
 *    out_data[i] = x-h
 *    historical_data[i] = (1 - alpha) * h + alpha * x
 * }
 * @endcode
 * @param[in,out] inst Pointer to configuration and history data
 * @param[in] in_data Pointer to current data
 * @param[out] out_data Pointer to output data with movement indication (size must be identical as
 * in_data)
 */
void ifx_mti_f32(const ifx_mti_inst_f32* inst,
                 const float32_t* in_data,
                 float32_t* out_data);


/** @brief Find peaks in array
 *
 * The function searches for peaks in an array. A peak is a point that is larger   than its left and
 * its right neighbor point. The pointer array together with the
 * length len denote the left and right end of the search range.
 *
 * The function searches for the max_peaks highest peaks and returns the   number of peaks found.
 * The function may find 0 peaks up to max_peaks peaks.
 * The indices of the peaks are written to the array indices.
 *
 * @param [in] x             Pointer to array for peak search
 * @param [in] length        Number of elements in array
 * @param [out] peak_indices Pointer to array that holds the indices of the peaks found. The array
 * must hold at
 * least max_peaks elements.
 * @param [in] max_peaks Maximum number of peaks
 * @param [in] opts      Pointer to conditions structure for peaks
 * @retval               Number of peaks found. The corresponding indices are written to peaks
 */
int32_t ifx_peak_search_f32(const float32_t* x, int32_t length, int32_t* peak_indices,
                            int32_t max_peaks,
                            const ifx_peak_search_opts_f32_t* opts);


/**
 * @brief Flip the array of float numbers
 *
 * @param[inout] v Pointer to input array
 * @param[in] len Number of elements in array
 * @return none
 */
void ifx_flip_f32(float32_t* v, uint32_t len);


/**
 * @brief Rotate the array of float numbers
 *
 * @param[inout] v Pointer to input array
 * @param[in] len Number of elements in array
 * @param[in] k Number of positions to move
 * @return none
 */
void ifx_rotate_f32(float32_t* v, uint32_t len, uint32_t k);


/**
 * @brief Calculate range resolution
 *
 * @param[in] bandwidth_hz Bandwidth in Hz
 * @return Range resolution
 */
static inline float32_t ifx_range_resolution(float32_t bandwidth_hz)
{
    return (IFX_LIGHT_SPEED_M_S / (2.0F * bandwidth_hz));
}


/**
 * @brief Calculate range resolution including number of samples, sample rate and frequency slope
 *
 * @param[in] num_samples Number of samples
 * @param[in] sample_rate_hz Sample rate in Hz
 * @param[in] freq_slope_hz_s Frequency slope in Hz/s
 * @return Range resolution
 */
static inline float32_t ifx_range_resolution_ex(uint32_t num_samples, float32_t sample_rate_hz,
                                                float32_t freq_slope_hz_s)
{
    const float32_t bandwidth = (freq_slope_hz_s / sample_rate_hz) *
                                (float32_t)num_samples;
    return ifx_range_resolution(bandwidth);
}


/**
 * @brief Calculating max range
 *
 * @param[in] sample_rate_hz Sample rate in Hz
 * @param[in] freq_slope_hz_s Frequency slope in Hz/s
 * @return Max range
 */
static inline float32_t ifx_max_range(uint32_t sample_rate_hz, float32_t freq_slope_hz_s)
{
    return (((float32_t)sample_rate_hz * IFX_LIGHT_SPEED_M_S) / (2.0F * freq_slope_hz_s));
}


/**
 * @brief Calculating Doppler resolution
 *
 * @param[in] bandwidth_hz Bandwidth in Hz
 * @param[in] frame_time_s Frame time in seconds
 * @return Doppler resolution
 */
static inline float32_t ifx_doppler_resolution(float32_t bandwidth_hz, float32_t frame_time_s)
{
    return (IFX_LIGHT_SPEED_M_S / (bandwidth_hz * frame_time_s));
}


/**
 * @brief Calculating Max Doppler
 *
 * @param[in] bandwidth_hz Bandwidth in Hz
 * @param[in] chirp_time_s Chirp time in seconds
 * @return Max Doppler
 */
static inline float32_t ifx_max_doppler(float32_t bandwidth_hz, float32_t chirp_time_s)
{
    return (IFX_LIGHT_SPEED_M_S / (2.0f * bandwidth_hz * chirp_time_s));
}


/**
 * @brief Calculates and returns arcus sinus of the given input, in radians.
 * The Implementation uses an identity and above arcus tangent.
 *
 * @param[in] x input value
 * @param[out] result output value in radians
 * @return status flag - ARM_MATH_SUCCESS on success and
 *                       ARM_MATH_ARGUMENT_ERROR on error
 */
arm_status ifx_arcsin_f32(float32_t x, float32_t* result);


/**
 * @brief Calculates the angle of the direction of arrival of a monopulse
 * Defines a method for calculating the angle of monopulse with given
 * wavelength, spacing between two antennas and phase shifts.
 *
 * @param[in] rx1 Pointer to input vector of antenna 1
 * @param[in] rx2 Pointer to input vector of antenna 2
 * @param[in] size Length of the input vector
 * @param[in] wavelength Length of the wave (in units of meters)
 * @param[in] antenna_spacing Distance between antennas (in units of meters)
 * @param[out] angle Pointer to output angle of the direction of monopulse
 * @return Status flag - ARM_MATH_SUCCESS on success and
 *                       ARM_MATH_ARGUMENT_ERROR on error
 */
arm_status ifx_angle_monopulse_f32(const cfloat32_t* rx1,
                                   const cfloat32_t* rx2,
                                   uint32_t size,
                                   float32_t wavelength,
                                   float32_t antenna_spacing,
                                   float32_t* angle);

/** \} group_sensor_dsp */

#ifdef __cplusplus
}
#endif

#endif // ifndef IFX_SENSOR_DSP_H_
