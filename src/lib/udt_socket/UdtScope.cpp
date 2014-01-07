/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "UdtScope.h"

#include "udt4/src/udt.h"

UdtScope::UdtScope()
{
	UDT::startup();
}

UdtScope::~UdtScope()
{
	UDT::cleanup();
}
