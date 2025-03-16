
#pragma once

#include <can_iface.hpp>
#include "can_datatypes.hpp"

#include <map>

class CAN : ICAN {

private:
	map<uint8_t, Node_t> canNodes;

public:
	CAN();
	virtual ~CAN();

	bool heartBeat() override;
};