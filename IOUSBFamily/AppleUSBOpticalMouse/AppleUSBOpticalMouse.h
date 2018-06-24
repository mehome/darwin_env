/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1998-2007 Apple Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _IOKIT_APPLEUSBOPTICALMOUSE_H
#define _IOKIT_APPLEUSBOPTICALMOUSE_H

#include "IOUSBHIDDriver.h"

#define kMouseRetryCount	3

class AppleUSBOpticalMouse : public IOUSBHIDDriver
{
    OSDeclareDefaultStructors(AppleUSBOpticalMouse)

private:
    bool				_switchTo800dpiFlag;
    bool				_switchTo2000fpsFlag;
	bool				_switchBackOnRestart;
    
    // IOKit methods
	virtual IOReturn	setPowerState ( unsigned long powerStateOrdinal, IOService* whatDevice );
    virtual bool		willTerminate(IOService * provider, IOOptionBits options);
    
    // IOUSBHIDDriver methods
    virtual IOReturn	StartFinalProcessing(void);
    
};

#endif _IOKIT_APPLEUSBOPTICALMOUSE_H