
#pragma once

#include "can_datatypes.hpp"
#include <cstdint>

class ICAN {

private:
	virtual uint8_t initNode();

public:
	ICAN();
	virtual ~ICAN();

	virtual bool heartBeat();
};
