/*
 * Can_GeneralTypes.h
 *
 *  Created on: Apr 14, 2016
 *      Author: Administrator
 */

#ifndef CAN_GENERALTYPES_H_
#define CAN_GENERALTYPES_H_

#define CAN_TYPE_BASIC 0x00u
#define CAN_TYPE_FULL  0x01u

#define   CAN_ID_TYPE_29 0x00u
#define   CAN_ID_TYPE_11 0x01u

#define CAN_TXID  0x18DA2F00u
#define CAN_RXID  0x18DA002Fu

#define CAN_CTRL_A_CANHRH 0x00u
#define CAN_CTRL_C_CANHRH 0x01u
#define CAN_CTRL_A_CCPHRH 0x02u

#define CAN_CTRL_A_CANHTH 0x00u
#define CAN_CTRL_C_CANHTH 0x01u
#define CAN_CTRL_A_CCPHTH 0x02u
#define CAN_CTRL_A_DAQHTH 0x03u

#endif /* CAN_GENERALTYPES_H_ */
