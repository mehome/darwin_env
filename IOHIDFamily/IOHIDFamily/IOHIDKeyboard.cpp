/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
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

/*
 * 13 Aug 2002 		ryepez
 *			This class is based off IOHIKeyboard and handles
 *			USB HID report based keyboard devices
 */

#include <IOKit/IOLib.h>
#include <IOKit/assert.h>
#include <IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include <IOKit/usb/USB.h>

#include "IOHIDKeyboard.h"
#include "IOHIDKeys.h"
#include "IOHIDElement.h"
#include "AppleHIDUsageTables.h"

#define kFnModifierUsagePageKey		"FnModifierUsagePage"
#define kFnModifierUsageKey		"FnModifierUsage"
#define kFnSpecialKeyMapKey		"FnSpecialKeyMap"
#define	kFnNonSpecialUsageMapKey	"FnNonSpecialUsageMap"
#define	kNumPadUsageMapKey		"NumPadUsageMap"

#define super IOHIKeyboard

OSDefineMetaClassAndStructors(IOHIDKeyboard, super)

extern unsigned char hid_usb_2_adb_keymap[];  //In Cosmo_USB2ADB.cpp

IOHIDKeyboard * 
IOHIDKeyboard::Keyboard(UInt32 supportedModifiers, bool isDispatcher) 
{
    IOHIDKeyboard *keyboard = new IOHIDKeyboard;
    
    if ((keyboard == 0) || !keyboard->init())
    {
        if (keyboard) keyboard->release();
        return 0;
    }
    
    keyboard->_containsFKey = ( supportedModifiers & NX_SECONDARYFNMASK );

    keyboard->setProperty( kIOHIDKeyboardSupportedModifiersKey, supportedModifiers, 32 );
    
    keyboard->_isDispatcher = isDispatcher;

    return keyboard;
}

bool
IOHIDKeyboard::init(OSDictionary *properties)
{
  if (!super::init(properties))  return false;
    
    _asyncLEDThread 	= 0;
    _ledState 		= 0;
	_repeat			= true;
	setRepeatMode(_repeat);
                    
    _containsFKey = 0;
    
    //This makes separate copy of ADB translation table.  Needed to allow ISO
    //  keyboards to swap two keys without affecting non-ISO keys that are
    //  also connected, or that will be plugged in later through USB ports
    bcopy(hid_usb_2_adb_keymap, _usb_2_adb_keymap, ADB_CONVERTER_LEN);
      
    return true;
}


bool
IOHIDKeyboard::start(IOService *provider)
{
    OSNumber *xml_swap_CTRL_CAPSLOCK;
    OSNumber *xml_swap_CMD_ALT;
	OSNumber *xml_use_pc_keyboard;
	
    UInt16 productIDVal;
    UInt16 vendorIDVal;

    _provider = OSDynamicCast(IOHIDEventService, provider);
    
    if ( !_provider )
        return false;

    if ( _isDispatcher )
        setProperty(kIOHIDVirtualHIDevice, kOSBooleanTrue);

    productIDVal    = _provider->getProductID();
    vendorIDVal     = _provider->getVendorID();

    setProperty(kIOHIDTransportKey, _provider->getTransport());
    setProperty(kIOHIDVendorIDKey, vendorIDVal, 32);
    setProperty(kIOHIDProductIDKey, productIDVal, 32);
    setProperty(kIOHIDLocationIDKey, _provider->getLocationID(), 32);
    setProperty(kIOHIDCountryCodeKey, _provider->getCountryCode(), 32);

    xml_swap_CTRL_CAPSLOCK = OSDynamicCast( OSNumber, provider->getProperty("Swap control and capslock"));
    if (xml_swap_CTRL_CAPSLOCK)
    {
        if ( xml_swap_CTRL_CAPSLOCK->unsigned32BitValue())
        {
            char temp;
            
            temp = _usb_2_adb_keymap[0x39];  //Caps lock
                _usb_2_adb_keymap[0x39] = _usb_2_adb_keymap[0xe0];  //Left CONTROL modifier
                _usb_2_adb_keymap[0xe0] = temp;
        }
    }

    xml_swap_CMD_ALT = OSDynamicCast( OSNumber, provider->getProperty("Swap command and alt"));
    if (xml_swap_CMD_ALT)
    {
        if ( xml_swap_CMD_ALT->unsigned32BitValue())
        {
            char temp;
            
            temp = _usb_2_adb_keymap[0xe2];  //left alt
                _usb_2_adb_keymap[0xe2] = _usb_2_adb_keymap[0xe3];  //Left command modifier
                _usb_2_adb_keymap[0xe3] = temp;
                
                temp = _usb_2_adb_keymap[0xe6];  //right alt
                _usb_2_adb_keymap[0xe6] = _usb_2_adb_keymap[0xe7];  //right command modifier
                _usb_2_adb_keymap[0xe7] = temp;
    
        }
    }

	xml_use_pc_keyboard = OSDynamicCast( OSNumber, provider->getProperty("Use PC keyboard"));
	if (xml_use_pc_keyboard)
	{
        if ( xml_use_pc_keyboard->unsigned32BitValue())
        {
            char temp;
    
            temp = _usb_2_adb_keymap[0xe0];                       // Save left control
            _usb_2_adb_keymap[0xe0] = _usb_2_adb_keymap[0xe2];    // Left control becomes left alt(option)
            _usb_2_adb_keymap[0xe2] = _usb_2_adb_keymap[0xe3];    // Left alt becomes left flower(start)
            _usb_2_adb_keymap[0xe3] = temp;                      // Left flower becomes left control
    
            temp = _usb_2_adb_keymap[0xe6];                      // Save right alt
            _usb_2_adb_keymap[0xe6] = _usb_2_adb_keymap[0xe7];    // Right alt becomes right flower
            _usb_2_adb_keymap[0xe4] = temp;                      // Right control becomes right alt
        }
	}

    // Need separate thread to handle LED
    _asyncLEDThread = thread_call_allocate((thread_call_func_t)_asyncLED, (thread_call_param_t)this);
    
    if ( _containsFKey )
    {
        setProperty(kIOHIDFKeyModeKey, (unsigned long long)0, (unsigned int)32);
    }

    return super::start(provider);
}

void IOHIDKeyboard::stop(IOService * provider)
{    
    if (_asyncLEDThread)
    {
	thread_call_cancel(_asyncLEDThread);
	thread_call_free(_asyncLEDThread);
	_asyncLEDThread = 0;
    }

    super::stop(provider);
}

void IOHIDKeyboard::free()
{
    super::free();
}

extern "C" { 
	void Debugger( const char * ); 
	void boot(int paniced, int howto, char * command);
#define RB_BOOT		1	/* Causes reboot, not halt.  Is in xnu/bsd/sys/reboot.h */

}

void IOHIDKeyboard::dispatchKeyboardEvent(
                                AbsoluteTime                timeStamp,
                                UInt32                      usagePage,
                                UInt32                      usage,
                                bool                        keyDown,
                                IOOptionBits                options)
{
    UInt32  alpha   = usage;
    bool    repeat  = ((options & kHIDDispatchOptionKeyboardNoRepeat) == 0);
    
	switch (usagePage)
	{
		case kHIDPage_KeyboardOrKeypad:
            if (repeat != _repeat)
            {
                _repeat = repeat;
                setRepeatMode(_repeat);
            }

            super::dispatchKeyboardEvent(_usb_2_adb_keymap[alpha], keyDown, timeStamp);
            break;
        case kHIDPage_AppleVendor:
            if ((usage == kHIDUsage_AppleVendor_KeyboardFn) && (_provider->getVendorID() == kIOUSBVendorIDAppleComputer))
                super::dispatchKeyboardEvent(0x3f, keyDown, timeStamp);
			break;
	}
}

// **************************************************************************
// _asyncLED
//
// Called asynchronously to turn on/off the keyboard LED
//
// **************************************************************************
void 
IOHIDKeyboard::_asyncLED(OSObject *target)
{
    IOHIDKeyboard *me = OSDynamicCast(IOHIDKeyboard, target);

    me->Set_LED_States( me->_ledState ); 
}

void
IOHIDKeyboard::Set_LED_States(UInt8 ledState)
{
    UInt32              value;
    
    for (int i=0; i<2; i++)
    {
        value = (ledState >> i) & 1;
		
		_provider->setElementValue(kHIDPage_LEDs, i + kHIDUsage_LED_NumLock, value);
    }    
}

//******************************************************************************
// COPIED from ADB keyboard driver 3/25/99
// This is usually called on a call-out thread after the caps-lock key is pressed.
// ADB operations to PMU are synchronous, and this is must not be done
// on the call-out thread since that is the PMU driver workloop thread, and
// it will block itself.
//
// Therefore, we schedule the ADB write to disconnect the call-out thread
// and the one that initiates the ADB write.
//
// *******************************************************************************
void
IOHIDKeyboard::setAlphaLockFeedback ( bool LED_state)
{
    //*** TODO *** REVISIT ***
    UInt8	newState = _ledState;

    if (LED_state) //set alpha lock
	newState |= kUSB_CAPSLOCKLED_SET;   //2nd bit is caps lock on USB
    else
	newState &= ~kUSB_CAPSLOCKLED_SET;

    if (newState != _ledState)
    {
        _ledState = newState;
        
        if (_asyncLEDThread) 
            thread_call_enter(_asyncLEDThread);
    }
}



void
IOHIDKeyboard::setNumLockFeedback ( bool LED_state)
{

    //*** TODO *** REVISIT ***
    UInt8	newState = _ledState;

    if (LED_state) 
	newState |= kUSB_NUMLOCKLED_SET;   //1st bit is num lock on USB
    else
	newState &= ~kUSB_NUMLOCKLED_SET;

    if (newState != _ledState)
    {
        _ledState = newState;
        
        if (_asyncLEDThread) 
            thread_call_enter(_asyncLEDThread);
    }
}


//Called from parent classes
unsigned
IOHIDKeyboard::getLEDStatus (void )
{
    unsigned	ledState = 0;
    
    for (int i=0; i<2; i++)
    {
		ledState |= (_provider->getElementValue(kHIDPage_LEDs, i + kHIDUsage_LED_NumLock)) << i;
    }
    return ledState;
}

// *****************************************************************************
// maxKeyCodes
// A.W. copied 3/25/99 from ADB keyboard driver, I don't know what this does
// ***************************************************************************
UInt32 
IOHIDKeyboard::maxKeyCodes (void )
{
    return 0x80;
}


// *************************************************************************
// deviceType
//
// **************************************************************************
UInt32 
IOHIDKeyboard::deviceType ( void )
{
    OSNumber 	*xml_handlerID;
    UInt32      id;	

	// RY: If a _deviceType an IOHIKeyboard protected variable.  If it is 
    // non-zero, there is no need to obtain the handlerID again.  Just return 
    // the already set value.  This should prevent us from mistakenly changing 
    // changing a deviceType(keyboardType) value back to an unknow value after 
    // it has been set via MacBuddy or the keyboardPref.
    if ( _deviceType )
    {
        id = _deviceType;
    }
    //Info.plist key is <integer>, not <string>
    else if ( xml_handlerID = OSDynamicCast( OSNumber, _provider->getProperty("alt_handler_id")) )
    {
        id = xml_handlerID->unsigned32BitValue();
    }
    else
    {
        id = handlerID();
    }
    
    // ISO specific mappign to match ADB keyboards
    // This should really be done in the keymaps.
    switch ( id )
    {
        case kgestUSBCosmoISOKbd: 
        case kgestUSBAndyISOKbd: 
        case kgestQ6ISOKbd: 
        case kgestQ30ISOKbd: 
        case kgestUSBGenericISOkd: 
            _usb_2_adb_keymap[0x35] = 0x0a;
            _usb_2_adb_keymap[0x64] = 0x32;
            break;
        default:
            _usb_2_adb_keymap[0x35] = 0x32;
            _usb_2_adb_keymap[0x64] = 0x0a;
            break;
    }


    return id;

}

// ************************************************************************
// interfaceID.  Fake ADB for now since USB defaultKeymapOfLength is too complex
//
// **************************************************************************
UInt32 
IOHIDKeyboard::interfaceID ( void )
{
    //Return value must match "interface" line in .keyboard file
    return NX_EVS_DEVICE_INTERFACE_ADB;  // 2 This matches contents of AppleExt.keyboard
}


/***********************************************/
//Get handler ID 
//
//  I assume that this method is only called if a valid USB keyboard
//  is found. This method should return a 0 or something if there's
//  no keyboard, but then the USB keyboard driver should never have
//  been probed if there's no keyboard, so for now it won't return 0.
UInt32
IOHIDKeyboard::handlerID ( void )
{
    UInt16 productID    = _provider->getProductID();
    UInt16 vendorID     = _provider->getVendorID();
    UInt32 ret_id       = kgestUSBUnknownANSIkd;  //Default for all unknown USB keyboards is 2

    //New feature for hardware identification using Gestalt.h values
    if (vendorID == kIOUSBVendorIDAppleComputer)
    {
        switch (productID)
        {
            case kprodUSBCosmoANSIKbd:  //Cosmo ANSI is 0x201
                    ret_id = kgestUSBCosmoANSIKbd; //0xc6
                    break;
            case kprodUSBCosmoISOKbd:  //Cosmo ISO
                    ret_id = kgestUSBCosmoISOKbd; //0xc7
                    break;
            case kprodUSBCosmoJISKbd:  //Cosmo JIS
                    ret_id = kgestUSBCosmoJISKbd;  //0xc8
                    break;
            case kprodUSBAndyANSIKbd:  //Andy ANSI is 0x204
                    ret_id = kgestUSBAndyANSIKbd; //0xcc
                    break;
            case kprodUSBAndyISOKbd:  //Andy ISO
                    ret_id = kgestUSBAndyISOKbd; //0xcd
                    break;
            case kprodUSBAndyJISKbd:  //Andy JIS is 0x206
                    ret_id = kgestUSBAndyJISKbd; //0xce
                    break;
            case kprodQ6ANSIKbd:  //Q6 ANSI
                    ret_id = kgestQ6ANSIKbd;
                    break;
            case kprodQ6ISOKbd:  //Q6 ISO
                    ret_id = kgestQ6ISOKbd;
                    break;
            case kprodQ6JISKbd:  //Q6 JIS
                    ret_id = kgestQ6JISKbd;
                    break;
            case kprodQ30ANSIKbd:  //Q30 ANSI
                    ret_id = kgestQ30ANSIKbd;
                    break;
            case kprodQ30ISOKbd:  //Q30 ISO
                    ret_id = kgestQ30ISOKbd;
                    break;
            case kprodQ30JISKbd:  //Q30 JIS
                    ret_id = kgestQ30JISKbd;
                    break;
            case kprodFountainANSIKbd:  //Fountain ANSI
                    ret_id = kgestFountainANSIKbd;
                    break;
            case kprodFountainISOKbd:  //Fountain ISO
                    ret_id = kgestFountainISOKbd;
                    break;
            case kprodFountainJISKbd:  //Fountain JIS
                    ret_id = kgestFountainJISKbd;
                    break;
            case kprodSantaANSIKbd:  //Santa ANSI
                    ret_id = kgestSantaANSIKbd;
                    break;
            case kprodSantaISOKbd:  //Santa ISO
                    ret_id = kgestSantaISOKbd;
                    break;
            case kprodSantaJISKbd:  //Santa JIS
                    ret_id = kgestSantaJISKbd;
                    break;
                    
            
            default:  // No Gestalt.h values, but still is Apple keyboard,
                    //   so return a generic Cosmo ANSI
                    ret_id = kgestUSBCosmoANSIKbd;  
                    break;
        }
    }

    return ret_id;  //non-Apple USB keyboards should all return "2"
}


// *****************************************************************************
// defaultKeymapOfLength
// A.W. copied from ADB keyboard, I don't have time to make custom USB version
// *****************************************************************************
const unsigned char * 
IOHIDKeyboard::defaultKeymapOfLength (UInt32 * length )
{
    if ( _containsFKey )
    {
        // this one defines the FKeyMap
        static const unsigned char appleUSAFKeyMap[] = {
            0x00,0x00,
            
            // Modifier Defs
            0x0b,   //Number of modifier keys.  Was 7
            //0x00,0x01,0x39,  //CAPSLOCK, uses one byte.
            0x01,0x01,0x38,
            0x02,0x01,0x3b,
            0x03,0x01,0x3a,
            0x04,0x01,0x37,
            0x05,0x14,0x52,0x41,0x53,0x54,0x55,0x45,0x58,0x57,0x56,0x5b,0x5c,
            0x43,0x4b,0x51,0x7b,0x7d,0x7e,0x7c,0x4e,0x59,
            0x06,0x01,0x72,
            0x07,0x01,0x3f, //NX_MODIFIERKEY_SECONDARYFN 8th modifier
            0x09,0x01,0x3c, //Right shift
            0x0a,0x01,0x3e, //Right control
            0x0b,0x01,0x3d, //Right Option
            0x0c,0x01,0x36, //Right Command
            
            // key deffs
            0x7f,0x0d,0x00,0x61,
            0x00,0x41,0x00,0x01,0x00,0x01,0x00,0xca,0x00,0xc7,0x00,0x01,0x00,0x01,0x0d,0x00,
            0x73,0x00,0x53,0x00,0x13,0x00,0x13,0x00,0xfb,0x00,0xa7,0x00,0x13,0x00,0x13,0x0d,
            0x00,0x64,0x00,0x44,0x00,0x04,0x00,0x04,0x01,0x44,0x01,0xb6,0x00,0x04,0x00,0x04,
            0x0d,0x00,0x66,0x00,0x46,0x00,0x06,0x00,0x06,0x00,0xa6,0x01,0xac,0x00,0x06,0x00,
                0x06,0x0d,0x00,0x68,0x00,0x48,0x00,0x08,0x00,0x08,0x00,0xe3,0x00,0xeb,0x00,0x00,
                0x18,0x00,0x0d,0x00,0x67,0x00,0x47,0x00,0x07,0x00,0x07,0x00,0xf1,0x00,0xe1,0x00,
                0x07,0x00,0x07,0x0d,0x00,0x7a,0x00,0x5a,0x00,0x1a,0x00,0x1a,0x00,0xcf,0x01,0x57,
                0x00,0x1a,0x00,0x1a,0x0d,0x00,0x78,0x00,0x58,0x00,0x18,0x00,0x18,0x01,0xb4,0x01,
                0xce,0x00,0x18,0x00,0x18,0x0d,0x00,0x63,0x00,0x43,0x00,0x03,0x00,0x03,0x01,0xe3,
                0x01,0xd3,0x00,0x03,0x00,0x03,0x0d,0x00,0x76,0x00,0x56,0x00,0x16,0x00,0x16,0x01,
                0xd6,0x01,0xe0,0x00,0x16,0x00,0x16,0x02,0x00,0x3c,0x00,0x3e,0x0d,0x00,0x62,0x00,
                0x42,0x00,0x02,0x00,0x02,0x01,0xe5,0x01,0xf2,0x00,0x02,0x00,0x02,0x0d,0x00,0x71,
                0x00,0x51,0x00,0x11,0x00,0x11,0x00,0xfa,0x00,0xea,0x00,0x11,0x00,0x11,0x0d,0x00,
                0x77,0x00,0x57,0x00,0x17,0x00,0x17,0x01,0xc8,0x01,0xc7,0x00,0x17,0x00,0x17,0x0d,
                0x00,0x65,0x00,0x45,0x00,0x05,0x00,0x05,0x00,0xc2,0x00,0xc5,0x00,0x05,0x00,0x05,
                0x0d,0x00,0x72,0x00,0x52,0x00,0x12,0x00,0x12,0x01,0xe2,0x01,0xd2,0x00,0x12,0x00,
                0x12,0x0d,0x00,0x79,0x00,0x59,0x00,0x19,0x00,0x19,0x00,0xa5,0x01,0xdb,0x00,0x19,
                0x00,0x19,0x0d,0x00,0x74,0x00,0x54,0x00,0x14,0x00,0x14,0x01,0xe4,0x01,0xd4,0x00,
                0x14,0x00,0x14,0x0a,0x00,0x31,0x00,0x21,0x01,0xad,0x00,0xa1,0x0e,0x00,0x32,0x00,
                0x40,0x00,0x32,0x00,0x00,0x00,0xb2,0x00,0xb3,0x00,0x00,0x00,0x00,0x0a,0x00,0x33,
                0x00,0x23,0x00,0xa3,0x01,0xba,0x0a,0x00,0x34,0x00,0x24,0x00,0xa2,0x00,0xa8,0x0e,
                0x00,0x36,0x00,0x5e,0x00,0x36,0x00,0x1e,0x00,0xb6,0x00,0xc3,0x00,0x1e,0x00,0x1e,
                0x0a,0x00,0x35,0x00,0x25,0x01,0xa5,0x00,0xbd,0x0a,0x00,0x3d,0x00,0x2b,0x01,0xb9,
                0x01,0xb1,0x0a,0x00,0x39,0x00,0x28,0x00,0xac,0x00,0xab,0x0a,0x00,0x37,0x00,0x26,
                0x01,0xb0,0x01,0xab,0x0e,0x00,0x2d,0x00,0x5f,0x00,0x1f,0x00,0x1f,0x00,0xb1,0x00,
                0xd0,0x00,0x1f,0x00,0x1f,0x0a,0x00,0x38,0x00,0x2a,0x00,0xb7,0x00,0xb4,0x0a,0x00,
                0x30,0x00,0x29,0x00,0xad,0x00,0xbb,0x0e,0x00,0x5d,0x00,0x7d,0x00,0x1d,0x00,0x1d,
                0x00,0x27,0x00,0xba,0x00,0x1d,0x00,0x1d,0x0d,0x00,0x6f,0x00,0x4f,0x00,0x0f,0x00,
                0x0f,0x00,0xf9,0x00,0xe9,0x00,0x0f,0x00,0x0f,0x0d,0x00,0x75,0x00,0x55,0x00,0x15,
                0x00,0x15,0x00,0xc8,0x00,0xcd,0x00,0x15,0x00,0x15,0x0e,0x00,0x5b,0x00,0x7b,0x00,
                0x1b,0x00,0x1b,0x00,0x60,0x00,0xaa,0x00,0x1b,0x00,0x1b,0x0d,0x00,0x69,0x00,0x49,
                0x00,0x09,0x00,0x09,0x00,0xc1,0x00,0xf5,0x00,0x09,0x00,0x09,0x0d,0x00,0x70,0x00,
                0x50,0x00,0x10,0x00,0x10,0x01,0x70,0x01,0x50,0x00,0x10,0x00,0x10,0x10,0x00,0x0d,
                0x00,0x03,0x0d,0x00,0x6c,0x00,0x4c,0x00,0x0c,0x00,0x0c,0x00,0xf8,0x00,0xe8,0x00,
                0x0c,0x00,0x0c,0x0d,0x00,0x6a,0x00,0x4a,0x00,0x0a,0x00,0x0a,0x00,0xc6,0x00,0xae,
                0x00,0x0a,0x00,0x0a,0x0a,0x00,0x27,0x00,0x22,0x00,0xa9,0x01,0xae,0x0d,0x00,0x6b,
                0x00,0x4b,0x00,0x0b,0x00,0x0b,0x00,0xce,0x00,0xaf,0x00,0x0b,0x00,0x0b,0x0a,0x00,
                0x3b,0x00,0x3a,0x01,0xb2,0x01,0xa2,0x0e,0x00,0x5c,0x00,0x7c,0x00,0x1c,0x00,0x1c,
                0x00,0xe3,0x00,0xeb,0x00,0x1c,0x00,0x1c,0x0a,0x00,0x2c,0x00,0x3c,0x00,0xcb,0x01,
                0xa3,0x0a,0x00,0x2f,0x00,0x3f,0x01,0xb8,0x00,0xbf,0x0d,0x00,0x6e,0x00,0x4e,0x00,
                0x0e,0x00,0x0e,0x00,0xc4,0x01,0xaf,0x00,0x0e,0x00,0x0e,0x0d,0x00,0x6d,0x00,0x4d,
                0x00,0x0d,0x00,0x0d,0x01,0x6d,0x01,0xd8,0x00,0x0d,0x00,0x0d,0x0a,0x00,0x2e,0x00,
                0x3e,0x00,0xbc,0x01,0xb3,0x02,0x00,0x09,0x00,0x19,0x0c,0x00,0x20,0x00,0x00,0x00,
                0x80,0x00,0x00,0x0a,0x00,0x60,0x00,0x7e,0x00,0x60,0x01,0xbb,0x02,0x00,0x7f,0x00,
                0x08,0xff,0x02,0x00,0x1b,0x00,0x7e,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                0xff,0xff,0x00,0x00,0x2e,0xff,0x00,0x00,
                0x2a,0xff,0x00,0x00,0x2b,0xff,0x00,0x00,0x1b,0xff,0xff,0xff,0x0e,0x00,0x2f,0x00,
                0x5c,0x00,0x2f,0x00,0x1c,0x00,0x2f,0x00,0x5c,0x00,0x00,0x0a,0x00,0x00,0x00,0x0d, //XX03
                0xff,0x00,0x00,0x2d,0xff,0xff,0x0e,0x00,0x3d,0x00,0x7c,0x00,0x3d,0x00,0x1c,0x00,
                0x3d,0x00,0x7c,0x00,0x00,0x18,0x46,0x00,0x00,0x30,0x00,0x00,0x31,0x00,0x00,0x32,
                0x00,0x00,0x33,0x00,0x00,0x34,0x00,0x00,0x35,0x00,0x00,0x36,0x00,0x00,0x37,0xff,
                0x00,0x00,0x38,0x00,0x00,0x39,0xff,0xff,0xff,0x00,0xfe,0x24,0x00,0xfe,0x25,0x00,
                0xfe,0x26,0x00,0xfe,0x22,0x00,0xfe,0x27,0x00,0xfe,0x28,0xff,0x00,0xfe,0x2a,0xff,
                0x00,0xfe,0x32,0x00,0xfe,0x35,0x00,0xfe,0x33,0xff,0x00,0xfe,0x29,0xff,0x00,0xfe,0x2b,0xff,
                0x00,0xfe,0x34,0xff,0x00,0xfe,0x2e,0x00,0xfe,0x30,0x00,0xfe,0x2d,0x00,0xfe,0x23,
                0x00,0xfe,0x2f,0x00,0xfe,0x21,0x00,0xfe,0x31,0x00,0xfe,0x20,
                0x00,0x01,0xac, //ADB=0x7b is left arrow
                0x00,0x01,0xae, //ADB = 0x7c is right arrow
                0x00,0x01,0xaf, //ADB=0x7d is down arrow.  
                0x00,0x01,0xad, //ADB=0x7e is up arrow	 
                0x0f,0x02,0xff,0x04,            
                0x00,0x31,0x02,0xff,0x04,0x00,0x32,0x02,0xff,0x04,0x00,0x33,0x02,0xff,0x04,0x00,
                0x34,0x02,0xff,0x04,0x00,0x35,0x02,0xff,0x04,0x00,0x36,0x02,0xff,0x04,0x00,0x37,
                0x02,0xff,0x04,0x00,0x38,0x02,0xff,0x04,0x00,0x39,0x02,0xff,0x04,0x00,0x30,0x02,
                0xff,0x04,0x00,0x2d,0x02,0xff,0x04,0x00,0x3d,0x02,0xff,0x04,0x00,0x70,0x02,0xff,
                0x04,0x00,0x5d,0x02,0xff,0x04,0x00,0x5b,
                0x07, // following are 7 special keys
                0x04,0x39,  //caps lock
                0x05,0x72,  //NX_KEYTYPE_HELP is 5, ADB code is 0x72
                0x06,0x7f,  //NX_POWER_KEY is 6, ADB code is 0x7f
                0x07,0x4a,  //NX_KEYTYPE_MUTE is 7, ADB code is 0x4a
                0x00,0x48,  //NX_KEYTYPE_SOUND_UP is 0, ADB code is 0x48
                0x01,0x49,  //NX_KEYTYPE_SOUND_DOWN is 1, ADB code is 0x49
                0x0a,0x47   //NX_KEYTYPE_NUM_LOCK is 10, ADB combines with CLEAR key for numlock
        };
        *length = sizeof(appleUSAFKeyMap);
        return appleUSAFKeyMap;
    } 
        
    static const unsigned char appleUSAKeyMap[] = {
        0x00,0x00,
        
        // Modifier Defs
        0x0a,   //Number of modifier keys.  Was 7
        //0x00,0x01,0x39,  //CAPSLOCK, uses one byte.
        0x01,0x01,0x38,
        0x02,0x01,0x3b,
        0x03,0x01,0x3a,
        0x04,0x01,0x37,
        0x05,0x15,0x52,0x41,0x4c,0x53,0x54,0x55,0x45,0x58,0x57,0x56,0x5b,0x5c,
        0x43,0x4b,0x51,0x7b,0x7d,0x7e,0x7c,0x4e,0x59,
        0x06,0x01,0x72,
        0x09,0x01,0x3c, //Right shift
        0x0a,0x01,0x3e, //Right control
        0x0b,0x01,0x3d, //Right Option
        0x0c,0x01,0x36, //Right Command
        
        // key deffs
        0x7f,0x0d,0x00,0x61,
        0x00,0x41,0x00,0x01,0x00,0x01,0x00,0xca,0x00,0xc7,0x00,0x01,0x00,0x01,0x0d,0x00,
        0x73,0x00,0x53,0x00,0x13,0x00,0x13,0x00,0xfb,0x00,0xa7,0x00,0x13,0x00,0x13,0x0d,
        0x00,0x64,0x00,0x44,0x00,0x04,0x00,0x04,0x01,0x44,0x01,0xb6,0x00,0x04,0x00,0x04,
        0x0d,0x00,0x66,0x00,0x46,0x00,0x06,0x00,0x06,0x00,0xa6,0x01,0xac,0x00,0x06,0x00,
            0x06,0x0d,0x00,0x68,0x00,0x48,0x00,0x08,0x00,0x08,0x00,0xe3,0x00,0xeb,0x00,0x00,
            0x18,0x00,0x0d,0x00,0x67,0x00,0x47,0x00,0x07,0x00,0x07,0x00,0xf1,0x00,0xe1,0x00,
            0x07,0x00,0x07,0x0d,0x00,0x7a,0x00,0x5a,0x00,0x1a,0x00,0x1a,0x00,0xcf,0x01,0x57,
            0x00,0x1a,0x00,0x1a,0x0d,0x00,0x78,0x00,0x58,0x00,0x18,0x00,0x18,0x01,0xb4,0x01,
            0xce,0x00,0x18,0x00,0x18,0x0d,0x00,0x63,0x00,0x43,0x00,0x03,0x00,0x03,0x01,0xe3,
            0x01,0xd3,0x00,0x03,0x00,0x03,0x0d,0x00,0x76,0x00,0x56,0x00,0x16,0x00,0x16,0x01,
            0xd6,0x01,0xe0,0x00,0x16,0x00,0x16,0x02,0x00,0x3c,0x00,0x3e,0x0d,0x00,0x62,0x00,
            0x42,0x00,0x02,0x00,0x02,0x01,0xe5,0x01,0xf2,0x00,0x02,0x00,0x02,0x0d,0x00,0x71,
            0x00,0x51,0x00,0x11,0x00,0x11,0x00,0xfa,0x00,0xea,0x00,0x11,0x00,0x11,0x0d,0x00,
            0x77,0x00,0x57,0x00,0x17,0x00,0x17,0x01,0xc8,0x01,0xc7,0x00,0x17,0x00,0x17,0x0d,
            0x00,0x65,0x00,0x45,0x00,0x05,0x00,0x05,0x00,0xc2,0x00,0xc5,0x00,0x05,0x00,0x05,
            0x0d,0x00,0x72,0x00,0x52,0x00,0x12,0x00,0x12,0x01,0xe2,0x01,0xd2,0x00,0x12,0x00,
            0x12,0x0d,0x00,0x79,0x00,0x59,0x00,0x19,0x00,0x19,0x00,0xa5,0x01,0xdb,0x00,0x19,
            0x00,0x19,0x0d,0x00,0x74,0x00,0x54,0x00,0x14,0x00,0x14,0x01,0xe4,0x01,0xd4,0x00,
            0x14,0x00,0x14,0x0a,0x00,0x31,0x00,0x21,0x01,0xad,0x00,0xa1,0x0e,0x00,0x32,0x00,
            0x40,0x00,0x32,0x00,0x00,0x00,0xb2,0x00,0xb3,0x00,0x00,0x00,0x00,0x0a,0x00,0x33,
            0x00,0x23,0x00,0xa3,0x01,0xba,0x0a,0x00,0x34,0x00,0x24,0x00,0xa2,0x00,0xa8,0x0e,
            0x00,0x36,0x00,0x5e,0x00,0x36,0x00,0x1e,0x00,0xb6,0x00,0xc3,0x00,0x1e,0x00,0x1e,
            0x0a,0x00,0x35,0x00,0x25,0x01,0xa5,0x00,0xbd,0x0a,0x00,0x3d,0x00,0x2b,0x01,0xb9,
            0x01,0xb1,0x0a,0x00,0x39,0x00,0x28,0x00,0xac,0x00,0xab,0x0a,0x00,0x37,0x00,0x26,
            0x01,0xb0,0x01,0xab,0x0e,0x00,0x2d,0x00,0x5f,0x00,0x1f,0x00,0x1f,0x00,0xb1,0x00,
            0xd0,0x00,0x1f,0x00,0x1f,0x0a,0x00,0x38,0x00,0x2a,0x00,0xb7,0x00,0xb4,0x0a,0x00,
            0x30,0x00,0x29,0x00,0xad,0x00,0xbb,0x0e,0x00,0x5d,0x00,0x7d,0x00,0x1d,0x00,0x1d,
            0x00,0x27,0x00,0xba,0x00,0x1d,0x00,0x1d,0x0d,0x00,0x6f,0x00,0x4f,0x00,0x0f,0x00,
            0x0f,0x00,0xf9,0x00,0xe9,0x00,0x0f,0x00,0x0f,0x0d,0x00,0x75,0x00,0x55,0x00,0x15,
            0x00,0x15,0x00,0xc8,0x00,0xcd,0x00,0x15,0x00,0x15,0x0e,0x00,0x5b,0x00,0x7b,0x00,
            0x1b,0x00,0x1b,0x00,0x60,0x00,0xaa,0x00,0x1b,0x00,0x1b,0x0d,0x00,0x69,0x00,0x49,
            0x00,0x09,0x00,0x09,0x00,0xc1,0x00,0xf5,0x00,0x09,0x00,0x09,0x0d,0x00,0x70,0x00,
            0x50,0x00,0x10,0x00,0x10,0x01,0x70,0x01,0x50,0x00,0x10,0x00,0x10,0x10,0x00,0x0d,
            0x00,0x03,0x0d,0x00,0x6c,0x00,0x4c,0x00,0x0c,0x00,0x0c,0x00,0xf8,0x00,0xe8,0x00,
            0x0c,0x00,0x0c,0x0d,0x00,0x6a,0x00,0x4a,0x00,0x0a,0x00,0x0a,0x00,0xc6,0x00,0xae,
            0x00,0x0a,0x00,0x0a,0x0a,0x00,0x27,0x00,0x22,0x00,0xa9,0x01,0xae,0x0d,0x00,0x6b,
            0x00,0x4b,0x00,0x0b,0x00,0x0b,0x00,0xce,0x00,0xaf,0x00,0x0b,0x00,0x0b,0x0a,0x00,
            0x3b,0x00,0x3a,0x01,0xb2,0x01,0xa2,0x0e,0x00,0x5c,0x00,0x7c,0x00,0x1c,0x00,0x1c,
            0x00,0xe3,0x00,0xeb,0x00,0x1c,0x00,0x1c,0x0a,0x00,0x2c,0x00,0x3c,0x00,0xcb,0x01,
            0xa3,0x0a,0x00,0x2f,0x00,0x3f,0x01,0xb8,0x00,0xbf,0x0d,0x00,0x6e,0x00,0x4e,0x00,
            0x0e,0x00,0x0e,0x00,0xc4,0x01,0xaf,0x00,0x0e,0x00,0x0e,0x0d,0x00,0x6d,0x00,0x4d,
            0x00,0x0d,0x00,0x0d,0x01,0x6d,0x01,0xd8,0x00,0x0d,0x00,0x0d,0x0a,0x00,0x2e,0x00,
            0x3e,0x00,0xbc,0x01,0xb3,0x02,0x00,0x09,0x00,0x19,0x0c,0x00,0x20,0x00,0x00,0x00,
            0x80,0x00,0x00,0x0a,0x00,0x60,0x00,0x7e,0x00,0x60,0x01,0xbb,0x02,0x00,0x7f,0x00,
            0x08,0xff,0x02,0x00,0x1b,0x00,0x7e,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
            0xff,0xff,0x00,0x00,0x2e,0xff,0x00,0x00,
            0x2a,0xff,0x00,0x00,0x2b,0xff,0x00,0x00,0x1b,0xff,0xff,0xff,0x0e,0x00,0x2f,0x00,
            0x5c,0x00,0x2f,0x00,0x1c,0x00,0x2f,0x00,0x5c,0x00,0x00,0x0a,0x00,0x00,0x00,0x0d, //XX03
            0xff,0x00,0x00,0x2d,0xff,0xff,0x0e,0x00,0x3d,0x00,0x7c,0x00,0x3d,0x00,0x1c,0x00,
            0x3d,0x00,0x7c,0x00,0x00,0x18,0x46,0x00,0x00,0x30,0x00,0x00,0x31,0x00,0x00,0x32,
            0x00,0x00,0x33,0x00,0x00,0x34,0x00,0x00,0x35,0x00,0x00,0x36,0x00,0x00,0x37,0xff,
            0x00,0x00,0x38,0x00,0x00,0x39,0xff,0xff,0xff,0x00,0xfe,0x24,0x00,0xfe,0x25,0x00,
            0xfe,0x26,0x00,0xfe,0x22,0x00,0xfe,0x27,0x00,0xfe,0x28,0xff,0x00,0xfe,0x2a,0xff,
            0x00,0xfe,0x32,0x00,0xfe,0x35,0x00,0xfe,0x33,0xff,0x00,0xfe,0x29,0xff,0x00,0xfe,0x2b,0xff,
            0x00,0xfe,0x34,0xff,0x00,0xfe,0x2e,0x00,0xfe,0x30,0x00,0xfe,0x2d,0x00,0xfe,0x23,
            0x00,0xfe,0x2f,0x00,0xfe,0x21,0x00,0xfe,0x31,0x00,0xfe,0x20,
            0x00,0x01,0xac, //ADB=0x7b is left arrow
            0x00,0x01,0xae, //ADB = 0x7c is right arrow
            0x00,0x01,0xaf, //ADB=0x7d is down arrow.  
            0x00,0x01,0xad, //ADB=0x7e is up arrow	 
            0x0f,0x02,0xff,0x04,            
            0x00,0x31,0x02,0xff,0x04,0x00,0x32,0x02,0xff,0x04,0x00,0x33,0x02,0xff,0x04,0x00,
            0x34,0x02,0xff,0x04,0x00,0x35,0x02,0xff,0x04,0x00,0x36,0x02,0xff,0x04,0x00,0x37,
            0x02,0xff,0x04,0x00,0x38,0x02,0xff,0x04,0x00,0x39,0x02,0xff,0x04,0x00,0x30,0x02,
            0xff,0x04,0x00,0x2d,0x02,0xff,0x04,0x00,0x3d,0x02,0xff,0x04,0x00,0x70,0x02,0xff,
            0x04,0x00,0x5d,0x02,0xff,0x04,0x00,0x5b,
            0x07, // following are 7 special keys
            0x04,0x39,  //caps lock
            0x05,0x72,  //NX_KEYTYPE_HELP is 5, ADB code is 0x72
            0x06,0x7f,  //NX_POWER_KEY is 6, ADB code is 0x7f
            0x07,0x4a,  //NX_KEYTYPE_MUTE is 7, ADB code is 0x4a
            0x00,0x48,  //NX_KEYTYPE_SOUND_UP is 0, ADB code is 0x48
            0x01,0x49,  //NX_KEYTYPE_SOUND_DOWN is 1, ADB code is 0x49
            0x0a,0x47   //NX_KEYTYPE_NUM_LOCK is 10, ADB combines with CLEAR key for numlock
    };
    *length = sizeof(appleUSAKeyMap);
    return appleUSAKeyMap;    

}

//====================================================================================================
// setParamProperties
//====================================================================================================
IOReturn IOHIDKeyboard::setParamProperties( OSDictionary * dict )
{
    if ( _containsFKey )
    {
        setProperty(kIOHIDFKeyModeKey, OSDynamicCast(OSNumber, dict->getObject(kIOHIDFKeyModeKey)));
    }
    
    return super::setParamProperties(dict);
    
}
