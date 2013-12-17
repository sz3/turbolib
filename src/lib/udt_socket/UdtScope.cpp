#include "UdtScope.h"

#include "udt/udt.h"

UdtScope::UdtScope()
{
	UDT::startup();
}

UdtScope::~UdtScope()
{
	UDT::cleanup();
}
