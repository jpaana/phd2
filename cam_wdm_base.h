/*
 *  cam_wdm_base.h
 *  PHD Guiding
 *
 *  Created by Craig Stark.
 *  Copyright (c) 2006-2010 Craig Stark.
 *  All rights reserved.
 *
 *  Refactored by Bret McKee
 *  Copyright (c) 2013 Dad Dog Development Ltd.
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Craig Stark, Stark Labs nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef CAM_WDM_BASE_INCLUDED
#define CAM_WDM_BASE_INCLUDED

#include "camera.h"

#define CVRES_VIDCAP_OFFSET wxID_HIGHEST + 1
#include "CVVidCaptureDSWin32.h"

struct cbdata
{
    CVRES status;
    bool raw;
    union
    {
        CVImage *cvimage;
        struct
        {
            const VIDEOINFOHEADER *hdr;
            const unsigned char *buf;
        };
    };
};

class CameraWDM : public GuideCamera
{
    volatile int m_nFrames;
    volatile int m_nAttempts;
    usImage *m_stack;
    volatile enum E_CAPTURE_MODE
    {
        NOT_CAPTURING = 0,
        STOP_CAPTURING,
        CAPTURE_ONE_FRAME,
        CAPTURE_STACKING,
        CAPTURE_STACK_FRAMES
    } m_captureMode;

protected:
    int m_deviceNumber;
    int m_deviceMode;
    bool m_rawYUY2;
    CVVidCaptureDSWin32 *m_pVidCap;

public:
    CameraWDM();

    bool CanSelectCamera() const override { return true; }
    bool HandleSelectCameraButtonClick(wxCommandEvent& evt);
    bool Capture(int duration, usImage& img, int options, const wxRect& subframe) override;
    bool CaptureOneFrame(usImage& img, int options, const wxRect& subframe);
    bool Connect(const wxString& camId) override;
    bool Disconnect() override;
    void ShowPropertyDialog() override;
    bool HasNonGuiCapture() override { return true; }
    wxByte BitsPerPixel() override;

    bool OnCapture(const cbdata& p);

protected:
    enum SelectionContext {
        CTX_SELECT,  // selecting device and mode in response to camera selection button
        CTX_CONNECT, // selecting device and mode in response to camera connect button
    };
    virtual bool SelectDeviceAndMode(SelectionContext ctx);
    bool BeginCapture(usImage& img, E_CAPTURE_MODE captureMode);
    void EndCapture();
};

class CameraLEWebcam : public CameraWDM
{
protected:
    enum LECAMERA_ACTIONS
    {
        LECAMERA_LED_OFF = 1,
        LECAMERA_LED_RED = 2,
        LECAMERA_LED_GREEN = 4,
        LECAMERA_AMP_OFF = 8,
        LECAMERA_AMP_ON = 16,
        LECAMERA_SHUTTER_CLOSED = 32,
        LECAMERA_SHUTTER_OPEN = 64,
        LECAMERA_EXPOSURE_FIELD_NONE = 128,
        LECAMERA_EXPOSURE_FIELD_A = 256,
        LECAMERA_EXPOSURE_FIELD_B = 512,
    };

public:

    CameraLEWebcam();
    ~CameraLEWebcam();

    bool Capture(int duration, usImage& img, int options, const wxRect& subframe) override;
    bool Connect(const wxString& camId) override;
    bool Disconnect() override;
    bool HasNonGuiCapture() override { return true; }

private:
    virtual bool LEControl(int actions) = 0;
};

#endif // CAM_WDM_BASE_INCLUDED
