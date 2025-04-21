#include "can.hpp"

CAN::CAN(FDCAN_HandleTypeDef *hfdcan) : hfdcan(hfdcan) {
//	this->canard.on_reception = &this->CanardOnTransferReception;
//	this->canard.should_accept = &this->shouldAcceptTransfer;
	static uint8_t canardMemoryPool[1024];

	canardInit(&canard,
			canardMemoryPool,
			sizeof(canardMemoryPool),
			&CAN::CanardOnTransferReception,
			&CAN::CanardShouldAcceptTransfer,
			NULL
	);

	canard.node_id = NODE_ID;
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

	if (canardGetLocalNodeID(ins) == CANARD_BROADCAST_NODE_ID)
    {
        /*
         * If we're in the process of allocation of dynamic node ID, accept only relevant transfers.
         */
        if ((transfer_type == CanardTransferTypeBroadcast) &&
            (data_type_id == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID))
        {
            *out_data_type_signature = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE;
            return true;
        }
        return false;
    }


	if (transfer_type == CanardTransferTypeRequest) {
		// check if we want to handle a specific service request

		
		switch (data_type_id) {
			case UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID:
			case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
				if (transfer_type == CanardTransferTypeResponse || transfer_type == CanardTransferTypeRequest) {
					*out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
					return true;
				}
				else if (transfer_type == CanardTransferTypeBroadcast) {
					*out_data_type_signature = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE;
					return true;
				}
				else {
					return false;
				}
			}
		}
	}
	return false;
}

void CAN::CanardOnTransferReception(CanardInstance *ins, CanardRxTransfer *transfer) 
{
    if (transfer->transfer_type == CanardTransferTypeRequest) {
        // check if we want to handle a specific service request
        switch (transfer->data_type_id) {
			case UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID:
			case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
				if (transfer->transfer_type == CanardTransferTypeResponse) {
					// handle_ReceiveNodeInfo(transfer); // TODO need to implement this function
				}
				else if (transfer->transfer_type == CanardTransferTypeBroadcast) {
					
				}
				break;
			}
        }
    }
}

void CAN::handle_ReceiveNodeInfo(CanardRxTransfer *transfer) {
	// TODO implement this function

	
	
	// Assume the node is a dynamic node
	if (transfer->source_node_id == 0) {
		// TODO handle anonymous node
		handle_NodeAllocation(transfer);
		return;
	}
	

}

void CAN::handle_NodeAllocation(CanardRxTransfer *transfer) {
	// TODO implement this function
	// This function should be called when a node is allocated
	// It should allocate a new node and add it to the canNodes map
	// The node ID should be the next available ID
	// The node should be added to the canNodes map with the ID as the key and the node as the value
	// The next available ID should be incremented by 1
	// The function should return the ID of the allocated node

	// Assume all nodes are dymaic at first




}

/*
Function to convert all canard CAN frames and send them through HAL

Consider removing for loop
*/
void CAN::sendCANTx() {
	bool success = true;
	for (const CanardCANFrame *frame; frame != nullptr; frame = canardPeekTxQueue(&canard)) {
		if (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) > 0) {
			FDCAN_TxHeaderTypeDef txHeader;

			txHeader.Identifier = frame->id;
			txHeader.IdType = FDCAN_EXTENDED_ID;
			txHeader.TxFrameType = FDCAN_DATA_FRAME;
			txHeader.DataLength = FDCAN_DLC_BYTES_8;
			txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
			txHeader.BitRateSwitch = FDCAN_BRS_OFF;
			txHeader.FDFormat = FDCAN_CLASSIC_CAN;
			
			const uint8_t *txData = frame->data;

			bool success = HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &txHeader, txData) == HAL_OK;

			if (success) {
				canardPopTxQueue(&canard);
			}
		}
	}
}

bool CAN::routineTasks() {
	sendCANTx();
}


/*
Wrapper function with mutex
*/
int16_t CAN::broadcastObj(CanardTxTransfer* transfer) {
	osStatus_t status = osMutexAcquire(canBroadcastMutex, CAN_BROADCAST_MUTEX_TIMEOUT);

	if (status != osOK){
		return -1; // handle failure
	} 

	int16_t res = canardBroadcastObj(&canard, transfer);
	osMutexRelease(canBroadcastMutex);

	return res;
}

int16_t CAN::broadcast(
	uint64_t data_type_signature,
	uint16_t data_type_id,
	uint8_t* inout_transfer_id,
	uint8_t priority,
	const void* payload,
	uint16_t payload_len
)
	{
		CanardTxTransfer transfer_object = {
			.data_type_signature = data_type_signature,
			.data_type_id = data_type_id,
			.inout_transfer_id = inout_transfer_id,
			.priority = priority,
			.payload = (uint8_t*)payload,
			.payload_len = payload_len,
			#if CANARD_ENABLE_DEADLINE
					.deadline_usec = tx_deadline,
			#endif
			#if CANARD_MULTI_IFACE
					.iface_mask = iface_mask,
			#endif
			#if CANARD_ENABLE_CANFD
					.canfd = canfd,
			#endif
		};

		return broadcastObj(&transfer_object);
	}