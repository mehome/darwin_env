/*
 * Copyright (c) 2013-2014 Apple Inc. All Rights Reserved.
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


#include <CoreFoundation/CoreFoundation.h>
#include <Security/SecCertificate.h>
#include <Security/SecCertificatePriv.h>
#include <Security/SecItem.h>
#include <Security/SecItemPriv.h>
#include <Security/SecIdentityPriv.h>
#include <Security/SecIdentity.h>
#include <Security/SecPolicy.h>
#include <Security/SecPolicyPriv.h>
#include <Security/SecTrust.h>
#include <Security/SecTrustPriv.h>
#include <utilities/SecCFRelease.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared_regressions.h"

//#if defined(NO_SERVER) && NO_SERVER == 1

__unused static const uint8_t kDemoContentSigningCert[] = {
	0x30, 0x82, 0x04, 0x1a, 0x30, 0x82, 0x03, 0x02, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x47,
	0x62, 0x58, 0xaa, 0x2c, 0xdd, 0x90, 0xc1, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x81, 0x87, 0x31, 0x3b, 0x30, 0x39, 0x06, 0x03, 0x55,
	0x04, 0x03, 0x0c, 0x32, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6d,
	0x20, 0x49, 0x6e, 0x74, 0x65, 0x67, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x32, 0x20, 0x43,
	0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74,
	0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c,
	0x1d, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61,
	0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x13,
	0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0a, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x49,
	0x6e, 0x63, 0x2e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
	0x30, 0x1e, 0x17, 0x0d, 0x31, 0x33, 0x30, 0x36, 0x30, 0x34, 0x32, 0x31, 0x34, 0x37, 0x31, 0x37,
	0x5a, 0x17, 0x0d, 0x31, 0x38, 0x30, 0x36, 0x30, 0x33, 0x32, 0x31, 0x34, 0x37, 0x31, 0x37, 0x5a,
	0x30, 0x53, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x14, 0x44, 0x65, 0x6d,
	0x6f, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x20, 0x53, 0x69, 0x67, 0x6e, 0x69, 0x6e,
	0x67, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x07, 0x43, 0x6f, 0x72, 0x65,
	0x20, 0x4f, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0a, 0x41, 0x70,
	0x70, 0x6c, 0x65, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
	0x06, 0x13, 0x02, 0x55, 0x53, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
	0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01,
	0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xaf, 0x2a, 0xac, 0x4b, 0x91, 0x1e, 0x4d, 0x9f, 0x1d, 0x23,
	0xf3, 0xff, 0x1c, 0xbe, 0x87, 0xbe, 0x3e, 0x2a, 0x0f, 0x35, 0xf4, 0xad, 0x80, 0x01, 0xc3, 0xf6,
	0x6b, 0x12, 0xc4, 0xfa, 0x0f, 0xf5, 0xbc, 0xc0, 0x79, 0x76, 0xad, 0x77, 0xb1, 0x8d, 0xdc, 0xe7,
	0xa0, 0x8a, 0xe0, 0x6a, 0xa3, 0xdb, 0x24, 0xb6, 0x0b, 0xa5, 0x29, 0x13, 0xe0, 0xd4, 0x96, 0xc7,
	0x1e, 0xee, 0x9b, 0x56, 0x63, 0xdd, 0xab, 0x9b, 0x4e, 0x7d, 0x09, 0xa7, 0x3f, 0xf9, 0xec, 0x96,
	0xcb, 0x88, 0x62, 0x91, 0xfe, 0xb8, 0x10, 0xe4, 0x40, 0x10, 0xb5, 0x34, 0x8a, 0x8e, 0x9a, 0x28,
	0x16, 0xc3, 0x8e, 0x06, 0xda, 0x32, 0x04, 0x64, 0x60, 0x3b, 0xae, 0x70, 0x24, 0xb0, 0xcc, 0x6c,
	0x76, 0x69, 0xb3, 0x52, 0x8f, 0x9c, 0x7a, 0xfb, 0x68, 0x55, 0x88, 0x2f, 0xd8, 0x18, 0xe0, 0x51,
	0x43, 0xdb, 0x91, 0x9f, 0x10, 0xcf, 0xd8, 0x93, 0x6e, 0x5d, 0xef, 0x90, 0xac, 0x47, 0x56, 0xf7,
	0x32, 0xd3, 0xf0, 0xb4, 0x9d, 0x68, 0x94, 0xdd, 0x4f, 0xf9, 0x5a, 0xb4, 0x9a, 0x81, 0xf8, 0xef,
	0xae, 0xd8, 0x27, 0x4d, 0xec, 0xc5, 0x7d, 0xc2, 0xfe, 0x80, 0x23, 0x4d, 0x13, 0xc9, 0x90, 0x5a,
	0x91, 0x86, 0x30, 0x42, 0x7d, 0x38, 0xda, 0x17, 0xa8, 0x46, 0x3e, 0x76, 0x8c, 0xca, 0x5e, 0x20,
	0x1d, 0x3b, 0x85, 0xda, 0x87, 0x9d, 0xf1, 0xc7, 0x1d, 0x3c, 0x13, 0x4a, 0x56, 0xd4, 0x23, 0x52,
	0x96, 0x6b, 0xb8, 0x99, 0x79, 0x03, 0x88, 0xbd, 0xf0, 0xf2, 0x28, 0x79, 0x1b, 0xe7, 0x8e, 0xfc,
	0xd0, 0x30, 0xf3, 0x0b, 0x3e, 0x15, 0xff, 0x20, 0x5c, 0x42, 0x67, 0x7e, 0x0e, 0x52, 0x86, 0x50,
	0xa0, 0x36, 0xb8, 0xc3, 0xee, 0x3d, 0xd8, 0xd6, 0x77, 0x15, 0xb7, 0x8f, 0xed, 0xf6, 0x42, 0xe4,
	0xfc, 0x52, 0xf6, 0x45, 0x67, 0x41, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x81, 0xbc, 0x30, 0x81,
	0xb9, 0x30, 0x3e, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x32, 0x30,
	0x30, 0x30, 0x2e, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x22, 0x68,
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x2e, 0x61, 0x70, 0x70, 0x6c, 0x65,
	0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x30, 0x34, 0x2d, 0x61, 0x73, 0x69, 0x30,
	0x32, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x55, 0xac, 0xda, 0x4b,
	0xad, 0xf1, 0xe8, 0xe4, 0x2f, 0x72, 0xc2, 0x02, 0x2b, 0xaa, 0x01, 0x23, 0x9c, 0x96, 0x86, 0xdb,
	0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1f,
	0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x12, 0x75, 0x7c, 0x47, 0x92,
	0xfd, 0x83, 0xe3, 0xbe, 0x2e, 0xfc, 0x59, 0x7f, 0x67, 0x43, 0x81, 0x35, 0x10, 0x45, 0xec, 0x30,
	0x19, 0x06, 0x03, 0x55, 0x1d, 0x20, 0x01, 0x01, 0xff, 0x04, 0x0f, 0x30, 0x0d, 0x30, 0x0b, 0x06,
	0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x05, 0x0c, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d,
	0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
	0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x27, 0x50,
	0xf8, 0x85, 0xe0, 0x6a, 0xf6, 0x74, 0x26, 0xcb, 0x5a, 0x23, 0xef, 0xd8, 0x32, 0xa0, 0x87, 0xf0,
	0x01, 0x29, 0x1c, 0x64, 0x84, 0x9e, 0xc6, 0x3b, 0xd7, 0x9e, 0x40, 0x8b, 0x0a, 0x8c, 0x9d, 0xd5,
	0x9b, 0x77, 0xc6, 0x5c, 0xe6, 0x52, 0xfb, 0x5b, 0x41, 0x2b, 0xbb, 0x8c, 0x24, 0xd5, 0xaf, 0xa1,
	0xab, 0x2b, 0xda, 0x3e, 0x11, 0x6b, 0x51, 0xe5, 0xc8, 0xb9, 0xf7, 0x1b, 0x0c, 0x89, 0x6e, 0x29,
	0x1a, 0x88, 0x24, 0xf9, 0x03, 0x2d, 0x46, 0x14, 0x07, 0xff, 0x3f, 0xd2, 0x2b, 0x87, 0x9b, 0xdb,
	0x5c, 0x06, 0x4a, 0x20, 0x4d, 0xf6, 0x33, 0xb1, 0x06, 0x93, 0x87, 0x27, 0x5c, 0x93, 0xca, 0xf6,
	0x81, 0x79, 0x71, 0x82, 0x1b, 0x88, 0x1d, 0x5a, 0x67, 0xc6, 0x2c, 0xd7, 0x68, 0x69, 0xbb, 0x07,
	0x1d, 0x12, 0xae, 0x78, 0x4c, 0xfa, 0x4e, 0xc5, 0x80, 0x5c, 0x2d, 0xd4, 0xb0, 0xcf, 0x71, 0x64,
	0x9e, 0xf8, 0xf3, 0x99, 0xd4, 0xd9, 0xdb, 0xbf, 0xd7, 0xb8, 0x97, 0x24, 0x3f, 0x34, 0x19, 0x52,
	0x73, 0xac, 0xca, 0x31, 0xac, 0x0d, 0xb5, 0x72, 0x11, 0x2d, 0x03, 0x7b, 0xce, 0xf6, 0x1e, 0x84,
	0x09, 0x3b, 0x47, 0xa0, 0x6a, 0xd6, 0x54, 0x60, 0x6d, 0x76, 0x6d, 0x02, 0xbb, 0xa8, 0x69, 0xa8,
	0x58, 0xb5, 0xe2, 0x34, 0xdb, 0x8d, 0xb2, 0x96, 0x17, 0x47, 0x6d, 0xbc, 0xe9, 0xc6, 0xc9, 0xae,
	0xc0, 0x54, 0xcf, 0x98, 0xc9, 0xea, 0x92, 0x69, 0x7f, 0x92, 0x82, 0x19, 0x63, 0x45, 0x5b, 0xeb,
	0x85, 0x0b, 0xaa, 0x7e, 0x4a, 0x77, 0xb4, 0xf9, 0x45, 0xe8, 0x0e, 0x1e, 0x1f, 0xab, 0x1c, 0x86,
	0x59, 0xa8, 0x82, 0x1e, 0x73, 0x86, 0x44, 0xfc, 0x67, 0x2a, 0x71, 0xeb, 0xb9, 0x9e, 0xd0, 0x28,
	0xd7, 0xb1, 0xef, 0xa3, 0x8c, 0x0d, 0x18, 0xd4, 0x8c, 0x1c, 0xb3, 0x0e, 0xba, 0x30
};

__unused static const uint8_t kTestDemoContentSigningCert[] = {
	0x30, 0x82, 0x04, 0x20, 0x30, 0x82, 0x03, 0x08, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x75,
	0x93, 0x8a, 0xaa, 0xe6, 0x6e, 0x3c, 0x5a, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x81, 0x87, 0x31, 0x3b, 0x30, 0x39, 0x06, 0x03, 0x55,
	0x04, 0x03, 0x0c, 0x32, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6d,
	0x20, 0x49, 0x6e, 0x74, 0x65, 0x67, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x32, 0x20, 0x43,
	0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74,
	0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c,
	0x1d, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61,
	0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x13,
	0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0a, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x49,
	0x6e, 0x63, 0x2e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
	0x30, 0x1e, 0x17, 0x0d, 0x31, 0x33, 0x30, 0x36, 0x30, 0x34, 0x30, 0x30, 0x30, 0x32, 0x35, 0x32,
	0x5a, 0x17, 0x0d, 0x31, 0x38, 0x30, 0x36, 0x30, 0x33, 0x30, 0x30, 0x30, 0x32, 0x35, 0x32, 0x5a,
	0x30, 0x58, 0x31, 0x22, 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x19, 0x54, 0x65, 0x73,
	0x74, 0x20, 0x44, 0x65, 0x6d, 0x6f, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x20, 0x53,
	0x69, 0x67, 0x6e, 0x69, 0x6e, 0x67, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c,
	0x07, 0x43, 0x6f, 0x72, 0x65, 0x20, 0x4f, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04,
	0x0a, 0x0c, 0x0a, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x0b, 0x30,
	0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d,
	0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01,
	0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xac, 0xc5, 0x87, 0x0d, 0x3b,
	0x14, 0xba, 0x87, 0x63, 0x3d, 0xc9, 0x8d, 0xc1, 0x3a, 0xa9, 0x71, 0x14, 0x30, 0x6b, 0x64, 0x58,
	0x35, 0xb2, 0xff, 0xcf, 0xe1, 0xe1, 0x5c, 0xa8, 0x5d, 0x99, 0xed, 0x11, 0x53, 0xf5, 0x6f, 0x92,
	0x39, 0x28, 0x5a, 0x0f, 0x76, 0x86, 0x40, 0x44, 0x6a, 0x7b, 0x23, 0x05, 0x50, 0xfd, 0x1b, 0x29,
	0xd9, 0x83, 0xfe, 0x0b, 0x65, 0xc6, 0x35, 0x8c, 0x69, 0x4e, 0x93, 0xa3, 0x32, 0x74, 0x18, 0xd4,
	0x06, 0xaf, 0xe5, 0x74, 0xb9, 0x63, 0x89, 0x41, 0x25, 0x2f, 0x15, 0xb6, 0x71, 0x45, 0x02, 0x5e,
	0x71, 0x11, 0xec, 0xea, 0xb8, 0x3a, 0x3d, 0xf2, 0x87, 0x2b, 0x71, 0xb8, 0x7e, 0xc8, 0xc7, 0x06,
	0x12, 0x87, 0x21, 0x61, 0x2f, 0xeb, 0x33, 0x44, 0xe1, 0x3e, 0xa7, 0x77, 0x6a, 0xfe, 0x5d, 0xee,
	0x4c, 0x77, 0x57, 0x74, 0xf7, 0x64, 0x2b, 0x6f, 0x12, 0x1b, 0x0e, 0xec, 0x49, 0x41, 0x81, 0x93,
	0x6b, 0xf9, 0x3b, 0x4e, 0x83, 0x53, 0xd7, 0x05, 0xe0, 0x29, 0x78, 0xf2, 0xd3, 0xe5, 0x81, 0x6f,
	0x47, 0xd1, 0xc8, 0xae, 0xb3, 0x22, 0xa9, 0x29, 0x61, 0xe2, 0x36, 0x26, 0xb9, 0x0d, 0x9d, 0x23,
	0x90, 0x45, 0x8b, 0x42, 0xbc, 0x8b, 0x60, 0xd5, 0xd2, 0x56, 0xc6, 0x09, 0xfd, 0x45, 0xff, 0x8c,
	0x54, 0x69, 0x74, 0x81, 0x90, 0x7a, 0x31, 0x6f, 0xf0, 0x16, 0x4c, 0xef, 0x1c, 0xa9, 0x97, 0xc9,
	0x4e, 0xfe, 0x7d, 0x87, 0x7a, 0x47, 0x3f, 0xa5, 0xc8, 0x20, 0xd3, 0x4a, 0xe2, 0xf3, 0xf6, 0x3a,
	0xf1, 0xef, 0x24, 0x5e, 0x35, 0xc8, 0xdf, 0x7b, 0x12, 0xda, 0xcc, 0xef, 0xbb, 0xf2, 0xfa, 0x28,
	0xf6, 0x38, 0x57, 0xcf, 0x41, 0xd6, 0x43, 0x21, 0x62, 0xc6, 0xa3, 0x3f, 0xf5, 0xd1, 0x86, 0x41,
	0x42, 0xb6, 0x69, 0xca, 0xb0, 0x8a, 0xba, 0xba, 0xbe, 0x97, 0x85, 0x02, 0x03, 0x01, 0x00, 0x01,
	0xa3, 0x81, 0xbd, 0x30, 0x81, 0xba, 0x30, 0x3e, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07,
	0x01, 0x01, 0x04, 0x32, 0x30, 0x30, 0x30, 0x2e, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07,
	0x30, 0x01, 0x86, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x2e,
	0x61, 0x70, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x6f, 0x63, 0x73, 0x70, 0x30, 0x34,
	0x2d, 0x61, 0x73, 0x69, 0x30, 0x33, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04,
	0x14, 0x0a, 0x8f, 0x13, 0x24, 0x0d, 0xc4, 0xe4, 0xe7, 0x97, 0x65, 0x35, 0xc8, 0x19, 0x91, 0x38,
	0x28, 0xce, 0x9b, 0x50, 0xdb, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04,
	0x02, 0x30, 0x00, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14,
	0x12, 0x75, 0x7c, 0x47, 0x92, 0xfd, 0x83, 0xe3, 0xbe, 0x2e, 0xfc, 0x59, 0x7f, 0x67, 0x43, 0x81,
	0x35, 0x10, 0x45, 0xec, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x1d, 0x20, 0x01, 0x01, 0xff, 0x04, 0x10,
	0x30, 0x0e, 0x30, 0x0c, 0x06, 0x0a, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x05, 0x0c, 0x01,
	0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03,
	0x82, 0x01, 0x01, 0x00, 0x59, 0x19, 0xdd, 0x6c, 0xf5, 0xb6, 0xf9, 0x93, 0x69, 0xe5, 0xcd, 0x64,
	0x5c, 0x4c, 0xaa, 0xd8, 0x86, 0x44, 0x6b, 0xca, 0x48, 0x6f, 0x2b, 0x15, 0x8f, 0xef, 0x1a, 0x5c,
	0xc9, 0xb2, 0x5b, 0x51, 0xee, 0xae, 0xf8, 0xeb, 0x7e, 0x05, 0x4a, 0xa1, 0x27, 0xd3, 0x15, 0x41,
	0xa1, 0x2c, 0x7e, 0x81, 0x96, 0xa8, 0xba, 0x8d, 0x1c, 0xc0, 0x0a, 0xce, 0xfd, 0x4b, 0xbc, 0x56,
	0x80, 0xe8, 0xf1, 0x04, 0x05, 0xd9, 0x17, 0x3d, 0x29, 0x6d, 0x99, 0xf5, 0xcc, 0x3a, 0x80, 0x7c,
	0x03, 0xb0, 0x8a, 0x4e, 0x2c, 0xdb, 0x24, 0x42, 0xa7, 0xa6, 0x8b, 0x2e, 0xff, 0x9c, 0xec, 0xce,
	0xe0, 0x0f, 0xd3, 0x5c, 0x0a, 0xed, 0x35, 0x14, 0xd0, 0x33, 0x76, 0xe0, 0x46, 0x09, 0x35, 0x54,
	0x40, 0x9b, 0x56, 0x8a, 0x45, 0x17, 0xa7, 0x50, 0x95, 0x5c, 0x9f, 0x5e, 0x95, 0x26, 0x89, 0xfe,
	0x79, 0xc6, 0x01, 0x3b, 0xf9, 0x64, 0x83, 0x28, 0x2b, 0x2d, 0x24, 0xbd, 0xb8, 0x4a, 0xca, 0x9a,
	0xba, 0x59, 0x5f, 0x72, 0xdd, 0x90, 0xcc, 0x4c, 0xed, 0x49, 0xf6, 0x11, 0x10, 0x7c, 0x34, 0x86,
	0x3f, 0x3e, 0x9a, 0xc0, 0xe6, 0xda, 0xf0, 0x6d, 0xb0, 0xd2, 0xf0, 0x46, 0x1e, 0x33, 0x53, 0x4a,
	0x5c, 0xd3, 0xdc, 0x9b, 0x74, 0x3d, 0x3e, 0x85, 0x9c, 0x19, 0xca, 0xb5, 0x26, 0xd5, 0xda, 0x22,
	0x7d, 0x52, 0xe6, 0x8c, 0x95, 0xd8, 0x30, 0x61, 0xf5, 0x3e, 0x05, 0x40, 0xe5, 0xd0, 0xcb, 0x91,
	0x28, 0x17, 0x4c, 0xf3, 0xf8, 0x06, 0x4c, 0x71, 0x80, 0xe0, 0x7c, 0x11, 0x7b, 0x00, 0xed, 0xe6,
	0x93, 0x8c, 0x12, 0x9e, 0xcc, 0x65, 0x5a, 0xb6, 0x61, 0x21, 0xe0, 0x0f, 0x3a, 0x8e, 0xcf, 0x12,
	0xea, 0x16, 0xbb, 0x91, 0x7e, 0x6c, 0x11, 0xb2, 0x29, 0x8a, 0x41, 0x46, 0xc3, 0xae, 0xd9, 0xf7,
	0x15, 0xe8, 0xaa, 0x86
};

__unused static const uint8_t kMobileIntermediate[] = {
  	0x30, 0x82, 0x04, 0x13, 0x30, 0x82, 0x02, 0xfb, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x60,
	0x25, 0x4e, 0x56, 0x81, 0x16, 0xcf, 0xf1, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x62, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
	0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0a,
	0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03,
	0x55, 0x04, 0x0b, 0x13, 0x1d, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69,
	0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69,
	0x74, 0x79, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0d, 0x41, 0x70, 0x70,
	0x6c, 0x65, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x32,
	0x30, 0x39, 0x31, 0x33, 0x32, 0x32, 0x33, 0x35, 0x33, 0x37, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30,
	0x39, 0x31, 0x33, 0x32, 0x32, 0x33, 0x35, 0x33, 0x37, 0x5a, 0x30, 0x81, 0x87, 0x31, 0x3b, 0x30,
	0x39, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x32, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x53, 0x79,
	0x73, 0x74, 0x65, 0x6d, 0x20, 0x49, 0x6e, 0x74, 0x65, 0x67, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x20, 0x32, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03,
	0x55, 0x04, 0x0b, 0x0c, 0x1d, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69,
	0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69,
	0x74, 0x79, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0a, 0x41, 0x70, 0x70,
	0x6c, 0x65, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
	0x13, 0x02, 0x55, 0x53, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
	0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a,
	0x02, 0x82, 0x01, 0x01, 0x00, 0x9e, 0x48, 0xf6, 0x8f, 0xab, 0x1b, 0x89, 0xa5, 0x9f, 0x34, 0x47,
	0xb6, 0x38, 0x30, 0xa7, 0xb0, 0x62, 0xa4, 0x99, 0x84, 0x2a, 0x74, 0x4b, 0x47, 0x5b, 0x66, 0x35,
	0x69, 0xfe, 0x97, 0x01, 0x64, 0x18, 0x9d, 0x0a, 0xf8, 0xd7, 0x33, 0x2a, 0x64, 0xd8, 0xbc, 0x03,
	0x4f, 0xcd, 0x4d, 0x80, 0x90, 0xd2, 0xc3, 0xc5, 0xd1, 0x8c, 0xc1, 0x73, 0xf6, 0x5a, 0x50, 0x70,
	0xcd, 0xe0, 0x94, 0x35, 0x82, 0xc4, 0x59, 0xca, 0xba, 0xa9, 0x94, 0xbd, 0x88, 0x36, 0x7a, 0x32,
	0xdf, 0x08, 0x29, 0xe0, 0x9c, 0x19, 0x1c, 0x68, 0x02, 0x60, 0x87, 0xfc, 0x2e, 0x8a, 0x3c, 0x37,
	0x4d, 0x7c, 0x81, 0xbb, 0x6c, 0x6f, 0x72, 0x45, 0xf5, 0xb3, 0x96, 0x66, 0x5e, 0xab, 0x97, 0x54,
	0x50, 0x03, 0xc0, 0x41, 0xfc, 0xb5, 0xa8, 0xf7, 0x78, 0xde, 0x4c, 0x9a, 0x67, 0xf4, 0x4d, 0x27,
	0x54, 0x9e, 0xc8, 0x75, 0xea, 0x8c, 0x76, 0xc1, 0xb5, 0x79, 0xe3, 0x60, 0xce, 0x98, 0x21, 0x38,
	0x7b, 0x9a, 0x53, 0xe8, 0x57, 0x69, 0x13, 0xf0, 0xf0, 0x86, 0x91, 0x2e, 0xad, 0x75, 0xb9, 0x53,
	0x4a, 0x30, 0xf6, 0x6d, 0x96, 0x8e, 0x5e, 0xe7, 0xe9, 0xa5, 0x16, 0xe7, 0xb4, 0x5b, 0xe7, 0xe9,
	0x6e, 0x4f, 0xe7, 0x38, 0xda, 0x53, 0x39, 0x60, 0x0e, 0xfd, 0xd0, 0x0b, 0x19, 0x52, 0x5d, 0x79,
	0x3a, 0x5d, 0x7b, 0x0d, 0x19, 0x91, 0x72, 0x94, 0x0e, 0x03, 0xa6, 0x14, 0xca, 0xdd, 0x7a, 0x27,
	0xed, 0x22, 0x21, 0xeb, 0x67, 0x01, 0xc0, 0x98, 0x4f, 0xf7, 0x78, 0x81, 0x34, 0xfb, 0x08, 0x48,
	0xb6, 0xec, 0xa4, 0xdb, 0x60, 0xd3, 0x9d, 0xa7, 0x46, 0x6c, 0x50, 0xa6, 0x14, 0x4a, 0x30, 0xee,
	0x77, 0x87, 0x64, 0xb6, 0xb2, 0xb4, 0x92, 0xfc, 0x45, 0xb3, 0x80, 0x59, 0xbd, 0x85, 0x9c, 0x2f,
	0xd1, 0xd7, 0x59, 0xe3, 0x0f, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x81, 0xa6, 0x30, 0x81, 0xa3,
	0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x12, 0x75, 0x7c, 0x47, 0x92,
	0xfd, 0x83, 0xe3, 0xbe, 0x2e, 0xfc, 0x59, 0x7f, 0x67, 0x43, 0x81, 0x35, 0x10, 0x45, 0xec, 0x30,
	0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff,
	0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x2b, 0xd0, 0x69,
	0x47, 0x94, 0x76, 0x09, 0xfe, 0xf4, 0x6b, 0x8d, 0x2e, 0x40, 0xa6, 0xf7, 0x47, 0x4d, 0x7f, 0x08,
	0x5e, 0x30, 0x2e, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x27, 0x30, 0x25, 0x30, 0x23, 0xa0, 0x21,
	0xa0, 0x1f, 0x86, 0x1d, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x63, 0x72, 0x6c, 0x2e, 0x61,
	0x70, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x72, 0x6f, 0x6f, 0x74, 0x2e, 0x63, 0x72,
	0x6c, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01,
	0x06, 0x30, 0x10, 0x06, 0x0a, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x06, 0x02, 0x0a, 0x04,
	0x02, 0x05, 0x00, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
	0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0xaa, 0x3d, 0x38, 0x8a, 0x96, 0x9b, 0xc7, 0x94, 0x97,
	0x3d, 0x3e, 0x5b, 0x1f, 0x09, 0x43, 0xe1, 0x1a, 0x6e, 0x6a, 0x4d, 0xd6, 0x38, 0xbb, 0x8a, 0x6d,
	0x00, 0xc9, 0x93, 0xdc, 0x3e, 0xa2, 0xc3, 0x2a, 0x97, 0xc1, 0x4f, 0x07, 0xa0, 0xa3, 0x14, 0x6c,
	0xd9, 0x0b, 0x9a, 0xcd, 0xb1, 0xcf, 0x7c, 0xc0, 0x67, 0x75, 0x12, 0xee, 0x2e, 0xb0, 0x28, 0x17,
	0x18, 0x7e, 0x73, 0x3d, 0xb4, 0x04, 0x91, 0x39, 0x7c, 0x37, 0xfb, 0xbd, 0x34, 0x01, 0xa0, 0xde,
	0x77, 0xa7, 0x5b, 0xb6, 0x47, 0x8e, 0x73, 0x3a, 0x8f, 0x7b, 0x63, 0x58, 0xc4, 0xa3, 0x6d, 0x19,
	0x4d, 0x9d, 0x9e, 0xd6, 0x3e, 0xbe, 0x18, 0x53, 0xe9, 0x30, 0x4d, 0xd2, 0xad, 0xef, 0x4b, 0xf6,
	0xa4, 0x84, 0xcf, 0x25, 0xfc, 0xea, 0x0d, 0x33, 0x58, 0x51, 0x00, 0xe2, 0x4b, 0xfe, 0x74, 0xd9,
	0x47, 0x13, 0x9e, 0xa7, 0xd7, 0x37, 0x6a, 0xea, 0x27, 0x0c, 0x6c, 0x7e, 0x8a, 0x93, 0xee, 0x60,
	0x10, 0x30, 0x1b, 0xf5, 0x59, 0x2a, 0x14, 0x4f, 0xd8, 0x7f, 0xe7, 0xb0, 0xe6, 0x3d, 0xfe, 0x72,
	0xe0, 0xc8, 0x52, 0x21, 0x15, 0x37, 0x18, 0x59, 0xd4, 0x7c, 0x5c, 0x8d, 0x55, 0x0b, 0x9b, 0xfb,
	0x3a, 0x75, 0x0f, 0xf6, 0x06, 0x29, 0xb6, 0xc2, 0x7f, 0x8d, 0x95, 0xce, 0x68, 0x77, 0x73, 0xae,
	0xde, 0x81, 0xfa, 0xce, 0x09, 0x72, 0xb1, 0x6c, 0xce, 0xe4, 0x94, 0x9e, 0x85, 0xf8, 0xdc, 0xba,
	0xc4, 0x5f, 0x77, 0xe9, 0x9e, 0x03, 0x99, 0x34, 0x8d, 0xcc, 0xdc, 0x9f, 0x56, 0x98, 0xed, 0x28,
	0xed, 0x06, 0x56, 0x80, 0xea, 0x52, 0x3a, 0x95, 0xc4, 0x64, 0x8f, 0x0c, 0xd9, 0x57, 0xbd, 0xc2,
	0xa9, 0x8d, 0xa0, 0x3a, 0xe2, 0x6c, 0x5a, 0x2a, 0x7b, 0xda, 0xf6, 0x81, 0x1a, 0x03, 0x17, 0x6b,
	0x0c, 0x0c, 0xda, 0xbd, 0x8e, 0x23, 0x06

};

// Escrow Service Key F98EB04C6AA62F4022709406305387C61415DDD770A24039C1415F24BF918803
static const uint8_t kEscrowLeafCert[] = {
 	0x30, 0x82, 0x04, 0x07, 0x30, 0x82, 0x02, 0xef, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x05, 0x00,
	0x9b, 0x6e, 0xef, 0x8d, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
	0x0b, 0x05, 0x00, 0x30, 0x79, 0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x05, 0x13, 0x03,
	0x31, 0x30, 0x30, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
	0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0a, 0x41, 0x70, 0x70, 0x6c, 0x65,
	0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x26, 0x30, 0x24, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x1d,
	0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74,
	0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x31, 0x1f, 0x30,
	0x1d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x16, 0x45, 0x73, 0x63, 0x72, 0x6f, 0x77, 0x20, 0x53,
	0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1e,
	0x17, 0x0d, 0x31, 0x33, 0x30, 0x38, 0x30, 0x32, 0x32, 0x33, 0x32, 0x34, 0x34, 0x38, 0x5a, 0x17,
	0x0d, 0x31, 0x35, 0x30, 0x38, 0x30, 0x32, 0x32, 0x33, 0x32, 0x34, 0x34, 0x38, 0x5a, 0x30, 0x81,
	0x9b, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b,
	0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x02, 0x43, 0x41, 0x31, 0x13, 0x30, 0x11, 0x06,
	0x03, 0x55, 0x04, 0x0a, 0x13, 0x0a, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x20, 0x49, 0x6e, 0x63, 0x2e,
	0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x03, 0x45, 0x54, 0x53, 0x31, 0x5c,
	0x30, 0x5a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x53, 0x45, 0x73, 0x63, 0x72, 0x6f, 0x77, 0x20,
	0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x20, 0x4b, 0x65, 0x79, 0x20, 0x39, 0x42, 0x36, 0x45,
	0x45, 0x46, 0x38, 0x44, 0x37, 0x42, 0x33, 0x43, 0x34, 0x39, 0x31, 0x32, 0x46, 0x35, 0x35, 0x35,
	0x32, 0x32, 0x32, 0x32, 0x42, 0x42, 0x34, 0x31, 0x44, 0x38, 0x34, 0x43, 0x43, 0x31, 0x39, 0x43,
	0x33, 0x35, 0x37, 0x37, 0x32, 0x35, 0x41, 0x36, 0x43, 0x35, 0x46, 0x34, 0x45, 0x35, 0x43, 0x43,
	0x36, 0x30, 0x37, 0x41, 0x32, 0x44, 0x37, 0x32, 0x31, 0x35, 0x41, 0x43, 0x30, 0x82, 0x01, 0x22,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03,
	0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xc5, 0xc7, 0x51,
	0xf1, 0x3f, 0x9a, 0x5d, 0xd7, 0xc1, 0x03, 0x42, 0x30, 0x84, 0xdb, 0xbc, 0x6c, 0x00, 0x6c, 0xcc,
	0x57, 0x97, 0xaf, 0xfe, 0x9e, 0xa0, 0x06, 0x9f, 0xe8, 0xa9, 0x59, 0xe1, 0xf8, 0xed, 0x23, 0x05,
	0x57, 0xe3, 0xd8, 0xdf, 0xf8, 0x31, 0x80, 0x8b, 0x31, 0x08, 0x2a, 0xc3, 0x7b, 0x16, 0xba, 0x27,
	0x84, 0x4e, 0xbb, 0x55, 0x5b, 0xd2, 0xda, 0x2d, 0xea, 0xda, 0x5c, 0xf0, 0x21, 0x58, 0x63, 0x74,
	0xa9, 0x90, 0x99, 0xbe, 0x87, 0x19, 0x7d, 0x87, 0xfc, 0xcb, 0xb6, 0xc9, 0x39, 0x51, 0x6a, 0xa7,
	0x81, 0x05, 0x50, 0x2d, 0xa2, 0x10, 0x6d, 0x58, 0xa5, 0x62, 0x29, 0x53, 0xce, 0xa6, 0x53, 0xad,
	0x3f, 0x50, 0xda, 0x1a, 0x1e, 0x2c, 0xe3, 0xae, 0x24, 0x88, 0xa5, 0x4c, 0xa3, 0x3a, 0xe1, 0xc6,
	0xa9, 0xcf, 0xb5, 0x53, 0x30, 0xbf, 0x7b, 0xea, 0x77, 0x21, 0x24, 0xfd, 0x91, 0x4c, 0x6f, 0x60,
	0x9f, 0x78, 0xf9, 0xed, 0x84, 0xe5, 0xee, 0xab, 0x07, 0xc5, 0x34, 0xa9, 0xe2, 0x0a, 0xf5, 0xf5,
	0xfa, 0x66, 0x75, 0xc8, 0x3e, 0x9c, 0xdd, 0xea, 0x60, 0xf0, 0x83, 0x03, 0x19, 0x08, 0xa4, 0x85,
	0xb0, 0xf3, 0xb1, 0xf1, 0x7a, 0x3d, 0xb4, 0xc8, 0xdd, 0x25, 0x5a, 0x1b, 0xf5, 0xa0, 0x78, 0xf9,
	0xbb, 0x08, 0x27, 0x6f, 0xa9, 0xf9, 0x17, 0xe8, 0xcb, 0x01, 0xa3, 0x5a, 0xd0, 0x67, 0xfb, 0xb7,
	0xef, 0xb7, 0x5c, 0x20, 0x94, 0x17, 0x5d, 0x46, 0xbd, 0xd2, 0xfe, 0xb6, 0x68, 0x88, 0x9f, 0xa6,
	0x0b, 0x97, 0x0f, 0x2e, 0x10, 0x23, 0x52, 0x9e, 0x69, 0x8b, 0xf4, 0x80, 0x83, 0x0e, 0x5b, 0x04,
	0xfc, 0x4e, 0xa1, 0x32, 0x44, 0x8a, 0x63, 0x3e, 0x3b, 0x0b, 0x70, 0x15, 0xd4, 0x17, 0xc1, 0xbe,
	0xbb, 0x01, 0x37, 0xe8, 0xfb, 0x58, 0x8b, 0x3d, 0xec, 0xc4, 0x47, 0x82, 0xe5, 0x02, 0x03, 0x01,
	0x00, 0x01, 0xa3, 0x73, 0x30, 0x71, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff,
	0x04, 0x02, 0x30, 0x00, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04,
	0x03, 0x02, 0x05, 0x20, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xb7,
	0x34, 0x54, 0x7b, 0x16, 0x2a, 0x38, 0x22, 0xd5, 0x79, 0x7b, 0xbf, 0x5c, 0x62, 0x16, 0x59, 0xc5,
	0x53, 0x9f, 0xac, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14,
	0xfd, 0x78, 0x96, 0x53, 0x80, 0xd6, 0xf6, 0xdc, 0xa6, 0xc3, 0x59, 0x06, 0x38, 0xed, 0x79, 0x3e,
	0x8f, 0x50, 0x1b, 0x50, 0x30, 0x11, 0x06, 0x0a, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x06,
	0x17, 0x01, 0x04, 0x03, 0x02, 0x01, 0x0a, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x01, 0x41, 0x68, 0xd9, 0xd5,
	0xb5, 0xbe, 0x2d, 0xa1, 0xe7, 0x52, 0x28, 0x2c, 0x4f, 0xe7, 0x43, 0x39, 0x4d, 0x1f, 0xeb, 0x09,
	0xd9, 0xc7, 0xca, 0x6f, 0x63, 0x60, 0x7f, 0x7c, 0xb5, 0x7c, 0x5e, 0x4b, 0xab, 0xd5, 0x8b, 0x34,
	0x5b, 0x50, 0xda, 0x3e, 0x37, 0xa8, 0x26, 0xf5, 0xdb, 0x76, 0xc4, 0x4a, 0x48, 0x09, 0xcf, 0x04,
	0x0a, 0x17, 0x6c, 0x8d, 0x3c, 0x6e, 0x1e, 0x41, 0xfc, 0xef, 0x45, 0xbd, 0x72, 0x59, 0x5e, 0x10,
	0x61, 0x4e, 0xad, 0x0e, 0xe4, 0x76, 0x3c, 0xf7, 0x87, 0xef, 0x54, 0xdd, 0x61, 0xe9, 0x91, 0x0f,
	0x7e, 0x52, 0xd6, 0x9e, 0x02, 0xd7, 0xb6, 0xcc, 0xa4, 0x9e, 0x7d, 0xba, 0x5f, 0xb4, 0x40, 0xc9,
	0xe6, 0x95, 0x61, 0xae, 0xb9, 0x89, 0xba, 0x25, 0x1b, 0xb6, 0xde, 0x08, 0x7f, 0x88, 0xef, 0x7c,
	0x59, 0x4d, 0x73, 0xc7, 0xf5, 0x07, 0x94, 0x13, 0x7b, 0xfc, 0x9f, 0x75, 0x0d, 0x1a, 0x69, 0xf0,
	0x51, 0x36, 0x1e, 0x46, 0x76, 0xc8, 0x27, 0x4e, 0x65, 0x58, 0x66, 0x41, 0x5e, 0x9d, 0xfe, 0xf1,
	0x10, 0xd5, 0x3c, 0x5b, 0xea, 0xcd, 0x96, 0x37, 0x4d, 0x76, 0x88, 0x60, 0xfb, 0x3f, 0xb2, 0x7a,
	0x00, 0xb4, 0xe0, 0xb6, 0xb9, 0x76, 0x6e, 0x02, 0xb6, 0xf7, 0x8d, 0x8b, 0x3a, 0x5c, 0xde, 0x4e,
	0xb9, 0xa4, 0x05, 0xc6, 0x14, 0xa6, 0x3f, 0x6c, 0xbd, 0xfd, 0xee, 0x0b, 0xf5, 0x5c, 0x27, 0x56,
	0xc5, 0x48, 0x55, 0x78, 0x72, 0xdc, 0xca, 0x95, 0xb7, 0x02, 0xb2, 0xdc, 0x4e, 0xbd, 0xe2, 0x78,
	0x87, 0xcc, 0xb5, 0xb0, 0x7c, 0x22, 0x52, 0xc1, 0xb0, 0x5a, 0x09, 0x9d, 0xb6, 0xbe, 0xe7, 0x4b,
	0xa2, 0x0e, 0x20, 0x43, 0x28, 0x77, 0x88, 0x1f, 0xd6, 0xa4, 0xb9, 0x56, 0xd0, 0xd0, 0xa6, 0x0f,
	0xa3, 0xef, 0x2e, 0xb7, 0x1d, 0x40, 0x61, 0xf8, 0xb9, 0x17, 0x4c,
};

static const uint8_t kPCSEscrowLeafCert[] = {
	0x30,0x82,0x04,0x0E,0x30,0x82,0x02,0xF6,0xA0,0x03,0x02,0x01,0x02,0x02,0x04,0x5B,
	0x57,0x9E,0xA2,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,
	0x05,0x00,0x30,0x7D,0x31,0x0C,0x30,0x0A,0x06,0x03,0x55,0x04,0x05,0x13,0x03,0x31,
	0x30,0x30,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,
	0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x13,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,
	0x49,0x6E,0x63,0x2E,0x31,0x26,0x30,0x24,0x06,0x03,0x55,0x04,0x0B,0x13,0x1D,0x41,
	0x70,0x70,0x6C,0x65,0x20,0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,
	0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x31,0x23,0x30,0x21,
	0x06,0x03,0x55,0x04,0x03,0x13,0x1A,0x45,0x66,0x66,0x61,0x63,0x65,0x61,0x62,0x6C,
	0x65,0x20,0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,
	0x41,0x30,0x1E,0x17,0x0D,0x31,0x34,0x30,0x37,0x32,0x39,0x30,0x30,0x31,0x31,0x34,
	0x33,0x5A,0x17,0x0D,0x31,0x36,0x30,0x37,0x32,0x39,0x30,0x30,0x31,0x31,0x34,0x33,
	0x5A,0x30,0x81,0x9F,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,
	0x53,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x08,0x13,0x02,0x43,0x41,0x31,0x13,
	0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x13,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,0x49,
	0x6E,0x63,0x2E,0x31,0x0C,0x30,0x0A,0x06,0x03,0x55,0x04,0x0B,0x13,0x03,0x45,0x54,
	0x53,0x31,0x60,0x30,0x5E,0x06,0x03,0x55,0x04,0x03,0x13,0x57,0x45,0x66,0x66,0x61,
	0x63,0x65,0x61,0x62,0x6C,0x65,0x20,0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x20,0x4B,
	0x65,0x79,0x20,0x35,0x42,0x35,0x37,0x39,0x45,0x41,0x32,0x36,0x34,0x41,0x39,0x36,
	0x36,0x39,0x37,0x42,0x41,0x38,0x31,0x33,0x34,0x35,0x34,0x44,0x32,0x45,0x38,0x43,
	0x46,0x37,0x44,0x42,0x31,0x34,0x30,0x33,0x37,0x43,0x30,0x43,0x45,0x44,0x32,0x34,
	0x33,0x43,0x42,0x37,0x46,0x45,0x33,0x31,0x33,0x38,0x33,0x42,0x34,0x35,0x38,0x34,
	0x33,0x30,0x42,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
	0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,
	0x82,0x01,0x01,0x00,0xAD,0xAD,0x92,0x86,0x08,0xFF,0x27,0xF1,0xCC,0xB6,0x57,0x41,
	0xC9,0x1F,0x05,0xC2,0xC7,0xC5,0xFB,0x8C,0xE3,0xEE,0xBC,0xE0,0x07,0xA4,0x58,0xE0,
	0x9E,0x81,0x19,0xF9,0x81,0xDF,0xD6,0x1B,0x65,0x23,0x0E,0xAC,0x2A,0xB6,0x3F,0x32,
	0x66,0xBF,0x9E,0xF4,0x0D,0xEA,0xE8,0xE9,0x5E,0xCF,0xB6,0x84,0x61,0x03,0x26,0x92,
	0xC5,0xAC,0xB2,0x54,0xE0,0x5B,0x22,0x88,0xFB,0x9B,0x6F,0xEF,0xFE,0xC1,0x40,0x27,
	0xF3,0x35,0xF2,0xC4,0x4C,0xB1,0xB4,0x57,0x2D,0xE7,0xF0,0x26,0xC0,0xE8,0x5F,0x02,
	0x13,0x33,0x53,0x5F,0xC8,0x5B,0x6B,0x14,0x5B,0x37,0x2F,0x24,0xD6,0x39,0x0C,0x7C,
	0x0D,0x8E,0x4A,0x73,0x33,0xAF,0xA1,0x78,0x6F,0xE0,0xD8,0x42,0x40,0x33,0x58,0x12,
	0x76,0xFF,0xF2,0x1E,0xAE,0x0B,0x80,0x3F,0x63,0x52,0xED,0xCA,0x33,0x3A,0x8B,0x19,
	0x37,0xFF,0xAC,0xFF,0x4D,0xBF,0xD1,0x9D,0x55,0xD1,0x2A,0x17,0x28,0x0E,0x6B,0xC8,
	0x12,0xBF,0x79,0x22,0x6B,0x8F,0x3C,0x3B,0x5C,0xC7,0x66,0xA8,0x9C,0x65,0x68,0xB4,
	0x4B,0x37,0xCE,0xC9,0x2A,0x7E,0x36,0x6A,0x7A,0x76,0x5D,0x47,0x26,0xDE,0x70,0xC8,
	0xE5,0x6B,0xA1,0xC5,0xC8,0xBC,0xCF,0xD2,0x78,0xAF,0xDE,0x9A,0xF4,0x49,0xC7,0xFC,
	0x78,0xF2,0x37,0xC1,0xD1,0x40,0xFA,0x30,0x4C,0x96,0xC2,0x16,0xEC,0xEB,0xE0,0x33,
	0x94,0xE1,0x83,0xE8,0xF9,0xD6,0x58,0xEE,0xEE,0x7D,0x49,0x38,0xC7,0xD6,0x15,0xE6,
	0x00,0x7F,0x40,0x37,0x03,0x3F,0xF3,0x5F,0x38,0x63,0x93,0x87,0xF1,0xE2,0xB7,0x8C,
	0xFF,0x21,0x68,0x7B,0x02,0x03,0x01,0x00,0x01,0xA3,0x73,0x30,0x71,0x30,0x0C,0x06,
	0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x02,0x30,0x00,0x30,0x0E,0x06,0x03,0x55,
	0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x05,0x20,0x30,0x1D,0x06,0x03,0x55,
	0x1D,0x0E,0x04,0x16,0x04,0x14,0x3F,0x94,0xA6,0xB9,0xB6,0xAE,0x43,0xDA,0x66,0xF3,
	0xD3,0x38,0xF0,0xD0,0x96,0x59,0x79,0x1B,0x0A,0xC1,0x30,0x1F,0x06,0x03,0x55,0x1D,
	0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x64,0x0B,0xE4,0x72,0x73,0x5C,0x54,0xB2,0x58,
	0x59,0xAE,0x42,0xDF,0x2B,0xB7,0xBA,0xB9,0xEB,0x86,0xAE,0x30,0x11,0x06,0x0A,0x2A,
	0x86,0x48,0x86,0xF7,0x63,0x64,0x06,0x17,0x01,0x04,0x03,0x02,0x01,0x0A,0x30,0x0D,
	0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,
	0x01,0x00,0x83,0x4B,0x3D,0x4E,0xE0,0x17,0xB5,0x95,0xAE,0x90,0xA4,0xE0,0x41,0x01,
	0x5B,0x1A,0x1D,0x69,0x83,0xA9,0xA0,0xD8,0x58,0xCD,0x57,0xB8,0x8B,0x1D,0x00,0xD2,
	0xB4,0x27,0x65,0x34,0x83,0xAD,0x5B,0xCF,0xA8,0x2F,0xFC,0x6C,0x08,0x1B,0x2B,0x12,
	0x93,0xE5,0x68,0xE3,0x85,0xFA,0x87,0x28,0x31,0x7E,0x18,0xA9,0xB9,0xEF,0xF4,0xE3,
	0x1C,0x24,0x83,0xE6,0x01,0xDA,0x56,0x49,0x52,0xA1,0xBC,0x56,0xE4,0xFC,0xA6,0xAF,
	0xC7,0x8D,0xD8,0x41,0xD3,0xA8,0x10,0x99,0x0F,0xEE,0x93,0xF1,0x58,0x4C,0x43,0x8B,
	0x31,0xF8,0xA8,0x04,0xFD,0x88,0x7E,0x0C,0x0C,0xA6,0xB9,0x3C,0xEC,0x9B,0xCD,0x99,
	0xF5,0x38,0x35,0x76,0x63,0xBC,0x23,0x9A,0x8F,0xA3,0x0B,0xE2,0x8E,0x55,0xEF,0x71,
	0xD8,0x87,0xA8,0x62,0x2B,0x35,0x32,0x24,0x6C,0xEE,0x95,0x5C,0x74,0xB2,0x1B,0x8F,
	0xEF,0x4C,0x45,0x03,0x0B,0x35,0x97,0x7D,0x43,0x7F,0x1D,0x3E,0xB9,0xE9,0x9D,0xF1,
	0x96,0x3B,0x91,0xA6,0xDF,0x52,0x00,0xB3,0xC5,0xDC,0xD3,0x29,0xAC,0x17,0xE1,0x73,
	0xA9,0x15,0x14,0xBD,0x1E,0x4F,0x9F,0x09,0xF8,0x84,0xF8,0xB6,0x9C,0xFD,0xCD,0x09,
	0x6D,0xDD,0x39,0xC7,0x15,0x58,0x86,0xE8,0x6B,0x47,0x4F,0x5D,0x84,0x29,0x56,0x81,
	0xF1,0x7C,0x09,0xA5,0x50,0x5C,0x4B,0xD9,0xFF,0xDF,0xA5,0xA9,0x6E,0xFF,0x14,0x9F,
	0xE4,0x4C,0xE7,0xAA,0x83,0xEA,0x30,0xB4,0x0D,0x8D,0xF7,0x36,0x4B,0x8D,0x38,0xDC,
	0xF5,0xA9,0xCF,0x3F,0x85,0x6A,0xC3,0x7A,0x53,0x86,0x18,0x5D,0x4B,0x93,0x1A,0x0E,
	0x50,0x5D,
};


#if 0
static inline void test_mobile(void)
{
	SecCertificateRef aCert = NULL;
	SecCertificateRef intermediateCert = NULL;
	SecCertificateRef rootCert = NULL;
	SecTrustResultType trustResult = kSecTrustResultUnspecified;
	SecPolicyRef policy = NULL;
	CFArrayRef certs = NULL;
	SecTrustRef trust = NULL;

	isnt(aCert = SecCertificateCreateWithBytes(NULL, kDemoContentSigningCert, sizeof(kDemoContentSigningCert)),
	    NULL, "create aCert from kDemoContentSigningCert");

	isnt(intermediateCert = SecCertificateCreateWithBytes(NULL, kMobileIntermediate, sizeof(kMobileIntermediate)),
       NULL, "create intermediate from kMobileIntermediate");

	SecCertificateRef refs[] = {aCert, intermediateCert};
	certs = CFArrayCreate(NULL, (const void **)refs, 2, NULL);

	isnt(policy = SecPolicyCreateMobileStoreSigner(),
		NULL, "create mobile policy");

	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust),
        "create trust for mobile store test cert");

	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate mobile store trust");

	is_status(trustResult, kSecTrustResultUnspecified,
		"trust is kSecTrustResultRecoverableTrustFailure");

	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(aCert);

	isnt(aCert = SecCertificateCreateWithBytes(NULL, kTestDemoContentSigningCert, sizeof(kTestDemoContentSigningCert)),
	    NULL, "create aCert from kDemoContentSigningCert");

	isnt(policy = SecPolicyCreateTestMobileStoreSigner(),
		NULL, "create Test mobile policy");

	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust),
        "create trust for mobile store test cert");

	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate test mobile store trust");

	is_status(trustResult, kSecTrustResultUnspecified,
		"trust is kSecTrustResultRecoverableTrustFailure");

	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(aCert);
	CFReleaseSafe(certs);
	CFReleaseSafe(intermediateCert);
	CFReleaseSafe(rootCert);

}
#endif // 0

static void test_escrow_with_anchor_roots(CFArrayRef anchors)
{
	SecCertificateRef escrowLeafCert = NULL;
	SecTrustResultType trustResult = kSecTrustResultUnspecified;
	SecPolicyRef policy = NULL;
	CFArrayRef certs = NULL;
	SecTrustRef trust = NULL;

	isnt(escrowLeafCert = SecCertificateCreateWithBytes(NULL, kEscrowLeafCert, sizeof(kEscrowLeafCert)),
	    NULL, "could not create aCert from kEscrowLeafCert");

	certs = CFArrayCreate(NULL, (const void **)&escrowLeafCert, 1, NULL);

	isnt(policy = SecPolicyCreateWithProperties(kSecPolicyAppleEscrowService, NULL),
		NULL, "could not create Escrow policy for GM Escrow Leaf cert");

	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust),
        "could not create trust for escrow service test GM Escrow Leaf cert");

	SecTrustSetAnchorCertificates(trust, anchors);

	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate escrow service trust for GM Escrow Leaf cert");

	is_status(trustResult, kSecTrustResultUnspecified,
		"trust is not kSecTrustResultUnspecified for GM Escrow Leaf cert");

	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(certs);
	CFReleaseSafe(escrowLeafCert);

}

static void test_pcs_escrow_with_anchor_roots(CFArrayRef anchors)
{
	SecCertificateRef leafCert = NULL;
	SecTrustResultType trustResult = kSecTrustResultUnspecified;
	SecPolicyRef policy = NULL;
	CFArrayRef certs = NULL;
	CFDateRef date = NULL;
	SecTrustRef trust = NULL;
	OSStatus status;

	isnt(leafCert = SecCertificateCreateWithBytes(NULL, kPCSEscrowLeafCert, sizeof(kPCSEscrowLeafCert)),
	    NULL, "could not create leafCert from kPCSEscrowLeafCert");

	certs = CFArrayCreate(NULL, (const void **)&leafCert, 1, NULL);

	isnt(policy = SecPolicyCreateWithProperties(kSecPolicyApplePCSEscrowService, NULL),
		NULL, "could not create PCS Escrow policy for GM PCS Escrow Leaf cert");

	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust),
		"could not create trust for PCS escrow service test GM PCS Escrow Leaf cert");

	/* Set explicit verify date: Mar 18 2016. */
	isnt(date = CFDateCreate(NULL, 480000000.0), NULL, "create verify date");
	status = (date) ? SecTrustSetVerifyDate(trust, date) : errSecParam;
	ok_status(status, "set date");

	SecTrustSetAnchorCertificates(trust, anchors);

	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate PCS escrow service trust for GM PCS Escrow Leaf cert");

	is_status(trustResult, kSecTrustResultUnspecified,
		"trust is not kSecTrustResultUnspecified for GM PCS Escrow Leaf cert");

	CFReleaseSafe(date);
	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(certs);
	CFReleaseSafe(leafCert);

}
static inline void test_escrow()
{
	CFArrayRef anchors = NULL;
	isnt(anchors = SecCertificateCopyEscrowRoots(kSecCertificateProductionEscrowRoot), NULL, "unable to get production anchors");
	test_escrow_with_anchor_roots(anchors);
	CFReleaseSafe(anchors);
}

static inline void test_pcs_escrow()
{
	CFArrayRef anchors = NULL;
	isnt(anchors = SecCertificateCopyEscrowRoots(kSecCertificateProductionPCSEscrowRoot), NULL, "unable to get production PCS roots");
	test_pcs_escrow_with_anchor_roots(anchors);
	CFReleaseSafe(anchors);
}

static inline void test_escrow_roots()
{
	CFArrayRef baselineRoots = NULL;
	isnt(baselineRoots = SecCertificateCopyEscrowRoots(kSecCertificateBaselineEscrowRoot), NULL, "unable to get baseline roots");
	ok(baselineRoots && CFArrayGetCount(baselineRoots) > 0, "baseline roots array empty");
	CFReleaseSafe(baselineRoots);

	CFArrayRef productionRoots = NULL;
	isnt(productionRoots = SecCertificateCopyEscrowRoots(kSecCertificateProductionEscrowRoot), NULL, "unable to get production roots");
	ok(productionRoots && CFArrayGetCount(productionRoots) > 0, "production roots array empty");
	CFReleaseSafe(productionRoots);

	CFArrayRef baselinePCSRoots = NULL;
	isnt(baselinePCSRoots = SecCertificateCopyEscrowRoots(kSecCertificateBaselinePCSEscrowRoot), NULL, "unable to get baseline PCS roots");
	ok(baselinePCSRoots && CFArrayGetCount(baselinePCSRoots) > 0, "baseline PCS roots array empty");
	CFReleaseSafe(baselinePCSRoots);

	CFArrayRef productionPCSRoots = NULL;
	isnt(productionPCSRoots = SecCertificateCopyEscrowRoots(kSecCertificateProductionPCSEscrowRoot), NULL, "unable to get production PCS roots");
	ok(productionPCSRoots && CFArrayGetCount(productionPCSRoots) > 0, "production PCS roots array empty");
	CFReleaseSafe(productionPCSRoots);
}

static void tests(void)
{
	test_escrow();
	test_pcs_escrow();
	test_escrow_roots();
}

//#endif /* defined(NO_SERVER) && NO_SERVER == 1 */

int si_71_mobile_store_policy(int argc, char *const *argv)
{
//#if defined(NO_SERVER) && NO_SERVER == 1

	plan_tests(22);

	tests();

//#endif

    return 0;
}
