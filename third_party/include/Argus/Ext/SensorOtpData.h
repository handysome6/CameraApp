/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * <b>Libargus Extension: Sensor OTP data API</b>
 *
 * @b Description: This file defines the SensorOtpData extension.
 */

#ifndef _ARGUS_SENSOR_OTP_DATA_H
#define _ARGUS_SENSOR_OTP_DATA_H

namespace Argus
{

/**
 * Adds a OTP interface to get OTP data.
 * It introduces one new interface:
 *   - ISensorOtpData: used to get OTP data
 * @defgroup ArgusExtOtpData Ext::SensorOtpData
 * @ingroup ArgusExtensions
 */
DEFINE_UUID(ExtensionName, EXT_SENSOR_OTP_DATA, 1651af90,19b1,11eb,8b6f,08,00,20,0c,9a,66);

namespace Ext
{

/**
 * @class ISensorOtpData
 *
 * Interface used to get OTP data
 *
 * @ingroup ArgusCameraDevice ArgusExtSensorOtpData
 */
DEFINE_UUID(InterfaceID, IID_SENSOR_OTP_DATA, 3d125710,19b1,11eb,8b6f,08,00,20,0c,9a,66);
class ISensorOtpData : public Interface
{
public:
    static const InterfaceID& id() { return IID_SENSOR_OTP_DATA; }

    /**
     * Returns the size of the OTP data.
     */
    virtual uint32_t getSensorOtpDataSize() const = 0;

    /**
     * Copies back the OTP data to the provided memory location.
     * If the size of @a dst is smaller than the total size of the EEPROM data, only the first
     * bytes up to size are copied.
     * @param [out] dst The pointer to the location where the data will be copied.
     *                     The caller is responsible for allocating and managing the memory.
     * @param [in] size The size of the destination.
     */
    virtual Status getSensorOtpData(void *dst, uint32_t size) const = 0;

protected:
    ~ISensorOtpData() {}
};

} // namespace Ext

} // namespace Argus

#endif


