/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 2016 Apple Computer, Inc.  All Rights Reserved.
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

#ifndef IOHIDUtility_h
#define IOHIDUtility_h

#include <IOKit/hid/IOHIDUsageTables.h>
#include <IOKit/hid/AppleHIDUsageTables.h>

enum {
    kKeyMaskCtrl             = 0x0001,
    kKeyMaskShift            = 0x0002,
    kKeyMaskAlt              = 0x0004,
    kKeyMaskCommand          = 0x0008,
    kKeyMaskPeriod           = 0x0010,
    kKeyMaskComma            = 0x0020,
    kKeyMaskSlash            = 0x0040,
    kKeyMaskEsc              = 0x0080,
    kKeyMaskFn               = 0x0100,
    kKeyMaskUnknown          = 0x80000000
};

struct Key {
    uint64_t _value;
    bool _modified;
    Key ():_value(0), _modified(false){}
    Key (uint32_t usagePage, uint32_t usage):_value(((uint64_t)usagePage << 32) | usage), _modified(false) {}
    Key (uint32_t usagePage, uint32_t usage, bool modified):_value(((uint64_t)usagePage << 32) | usage), _modified(modified) {}
    Key (uint64_t usageAndUsagePage): _value (usageAndUsagePage), _modified(false) {}
    friend bool operator==(const Key &lhs, const Key &rhs) {
        return (lhs._value == rhs._value);
    }
    friend bool operator<(const Key &lhs, const Key &rhs) {
        return (lhs._value < rhs._value);
    }
    uint32_t usage () const {return ((uint32_t*)&_value)[0];}
    uint32_t usagePage () const {return((uint32_t*)&_value)[1];}
    bool isValid () const {return _value != 0;}
    bool isModifier () const {
      bool result = false;
      if ((usagePage() == kHIDPage_KeyboardOrKeypad) &&
          (((usage() >= kHIDUsage_KeyboardLeftControl) && (usage() <= kHIDUsage_KeyboardRightGUI)) || (usage() == kHIDUsage_KeyboardCapsLock))) {
          result = true;
      } else if ((usagePage() == kHIDPage_AppleVendorTopCase) && (usage() == kHIDUsage_AV_TopCase_KeyboardFn)) {
          result = true;
      }
      return result;
    }
};

struct KeyAttribute {
    uint32_t  _flags;
    KeyAttribute (uint32_t  flags = 0):_flags(flags) {};
};

#endif /* IOHIDUtility_h */
