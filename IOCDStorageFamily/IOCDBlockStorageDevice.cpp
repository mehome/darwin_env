/*
 * Copyright (c) 1998-2014 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
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
#include <IOKit/storage/IOCDBlockStorageDevice.h>

#define	super	IOBlockStorageDevice
OSDefineMetaClassAndAbstractStructors(IOCDBlockStorageDevice,IOBlockStorageDevice)

bool
IOCDBlockStorageDevice::init(OSDictionary * properties)
{
    bool result;

    result = super::init(properties);
    if (result) {
        setProperty(kIOBlockStorageDeviceTypeKey,
                    kIOBlockStorageDeviceTypeCDROM);
    }

    return(result);
}

OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  0);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  1);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  2);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  3);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  4);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  5);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  6);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  7);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  8);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice,  9);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 10);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 11);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 12);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 13);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 14);
OSMetaClassDefineReservedUnused(IOCDBlockStorageDevice, 15);
