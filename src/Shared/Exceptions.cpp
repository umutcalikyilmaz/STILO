#include "STILO/Shared/Exceptions.h"

namespace STILO
{
    InvalidParameterException::InvalidParameterException(char* msg) : errmsg_(msg) { }

	const char* InvalidParameterException::what()
	{
		return errmsg_;
	}   
}
