divert(-1)
#
# Copyright (c) 1998-1999, 2001 Sendmail, Inc. and its suppliers.
#	All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

divert(0)
VERSIONID(`$Id: promiscuous_relay.m4,v 1.1.1.4 2002/03/12 17:59:49 zarzycki Exp $')
divert(-1)

define(`_PROMISCUOUS_RELAY_', 1)
errprint(`*** WARNING: FEATURE(`promiscuous_relay') configures your system as open
	relay.  Do NOT use it on a server that is connected to the Internet!
')
