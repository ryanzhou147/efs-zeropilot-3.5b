
#pragma once

#include <can_iface.hpp>
#include "can_datatypes.hpp"
#include <canard.h>
#include "stm32l5xx_hal.h"
#include <dronecan_msgs.h>

#include <map>

class CAN : ICAN {

private:
	std::map<uint8_t, Node_t> canNodes;
	uint8_t nextAvailableID = 2;
	FDCAN_HandleTypeDef *hfdcan;
	
	CanardInstance canInst;

	static bool CanardShouldAcceptTransfer(const CanardInstance* ins,          ///< Library instance
	                                            uint64_t* out_data_type_signature,  ///< Must be set by the application!
	                                            uint16_t data_type_id,              ///< Refer to the specification
	                                            CanardTransferType transfer_type,   ///< Refer to CanardTransferType
	                                            uint8_t source_node_id);

	static void CanardOnTransferReception(CanardInstance* ins,                 ///< Library instance
	                                           CanardRxTransfer* transfer);

	void sendCANTx();

public:
	CAN(FDCAN_HandleTypeDef* hfdcan);
	CAN();
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
