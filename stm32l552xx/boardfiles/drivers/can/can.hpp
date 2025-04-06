
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

	static void CanardOnTransferReception(CanardInstance* ins,                 ///< Library instance
	                                           CanardRxTransfer* transfer);

	void sendCANTx();

	void handle_ReceiveNodeInfo(CanardRxTransfer *transfer);
	void handle_NodeAllocation(CanardRxTransfer *transfer)

public:
	CAN(FDCAN_HandleTypeDef *hfdcan);
	
	virtual ~CAN();

	bool routineTasks();

	int16_t canardSTM32Receive(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation, CanardCANFrame *const rx_frame);
	
	bool routineTasks();

	/**
	 * Wrapper function with mutex
	 * 
	* Sends a broadcast transfer.
	* If the node is in passive mode, only single frame transfers will be allowed (they will be transmitted as anonymous).
	*
	* For anonymous transfers, maximum data type ID (CanardTxTransfer::data_type_id) is limited to 3 (see specification for details).
	*
	* Please refer to the specification for more details about data type signatures (CanardTxTransfer::data_type_signature). Signature for 
	* any data type can be obtained in many ways; for example, using the generated code generated using dronecan_dsdlc (see the repository).
	*
	* Use CanardTxTransfer structure to pass the transfer parameters. The structure is initialized by the
	* canardInitTxTransfer() function.
	* 
	* Pointer to the Transfer ID (CanardTxTransfer::inout_transfer_id) should point to a persistent variable
	* (e.g. static or heap allocated, not on the stack); it will be updated by the library after every transmission. 
	* The Transfer ID value cannot be shared between transfers that have different descriptors!
	* More on this in the transport layer specification.
	*
	* Returns the number of frames enqueued, or negative error code.
	*/
	int16_t canardBroadcastObj(
		CanardTxTransfer* transfer      ///< Transfer object
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
