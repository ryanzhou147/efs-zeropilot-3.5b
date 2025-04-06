
#pragma once

#include <can_iface.hpp>
#include "can_datatypes.hpp"
#include <canard.h>
#include "stm32l5xx_hal.h"
#include <dronecan_msgs.h>
#include "can_defines.hpp"
#include "cmsis_os2.h"
#include "museq.hpp"

class CAN : ICAN {

private:
	Node_t canNodes[CANARD_MAX_NODE_ID + 1];
	uint8_t nextAvailableID = CANARD_MIN_NODE_ID + 1;
	FDCAN_HandleTypeDef *hfdcan;
	
	CanardInstance canard;

	static bool CanardShouldAcceptTransfer(const CanardInstance* ins,          ///< Library instance
	                                            uint64_t* out_data_type_signature,  ///< Must be set by the application!
	                                            uint16_t data_type_id,              ///< Refer to the specification
	                                            CanardTransferType transfer_type,   ///< Refer to CanardTransferType
	                                            uint8_t source_node_id);

	void CanardOnTransferReception(CanardInstance* ins,                 ///< Library instance
	                                           CanardRxTransfer* transfer);

	void sendCANTx();

	void handleNodeAllocation(CanardInstance* ins, CanardRxTransfer* transfer);

	int8_t allocateNode();  


public:
	CAN(FDCAN_HandleTypeDef *hfdcan);

	virtual ~CAN();

	bool routineTasks();

	int16_t canardSTM32Receive(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation, CanardCANFrame *const rx_frame);
	
	
	int16_t broadcastObj(
		CanardTxTransfer* transfer
	);

	int16_t broadcast(
		uint64_t data_type_signature,
		uint16_t data_type_id,
		uint8_t* inout_transfer_id,
		uint8_t priority,
		const void* payload,
		uint16_t payload_len
	);

};
