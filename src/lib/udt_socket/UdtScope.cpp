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
