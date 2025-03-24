#include "can.hpp"

CAN::CAN() {
//	this->canInst.on_reception = &this->CanardOnTransferReception;
//	this->canInst.should_accept = &this->shouldAcceptTransfer;
	static uint8_t canardMemorypool[1024];

	canardInit(&canInst,
			canardMemoryPool,
			sizeof(canardMemoryPool),
			&CAN::CanardOnTransferReception,
			&CAN::CanardShouldAcceptTransfer,
			this
	);
}

CAN::~CAN() {

}
