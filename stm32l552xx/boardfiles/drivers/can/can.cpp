#include "can.hpp"

CAN::CAN(FDCAN_HandleTypeDef hfdcan) : hfdcan(hfdcan) {
//	this->canInst.on_reception = &this->CanardOnTransferReception;
//	this->canInst.should_accept = &this->shouldAcceptTransfer;
	static uint8_t canardMemoryPool[1024];

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

bool CAN::CanardShouldAcceptTransfer(
	const CanardInstance *ins,
	uint64_t *out_data_type_signature,
	uint16_t data_type_id,
	CanardTransferType transfer_type,
	uint8_t source_node_id)
{
	if (transfer_type == CanardTransferTypeRequest) {
		// check if we want to handle a specific service request
		switch (data_type_id) {
			case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
				*out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
				return true;
			}
		}
	}
	return false;
}

void CAN::CanardOnTransferReception(
	CanardInstance *ins,
	CanardRxTransfer *transfer)
{
    if (transfer->transfer_type == CanardTransferTypeRequest) {
        // check if we want to handle a specific service request
        switch (transfer->data_type_id) {
            case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
                handle_GetNodeInfo(ins, transfer); // TODO need to implement this function
                break;
            }
        }
    }
}
