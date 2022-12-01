#ifndef __INFRA_CANOPEN_SYSTEM_H__
#define __INFRA_CANOPEN_SYSTEM_H__

// system defined addresses and address ranges
#define CAN_ID_BROADCAST        0x00
#define CAN_ID_SEC              0x01    // Reserved
#define CAN_ID_PWB              0x02    // PoWer Bridge
#define CAN_ID_PWB_MAX          0x06
#define CAN_ID_COOLER           0x07    // Cooler Control
#define CAN_ID_COOLER_MAX       0x0A
#define CAN_ID_LCD              0x0B    // Light Control Device
#define CAN_ID_LCD_MAX          0x0E
//                              0x0F    // Illegal
#define CAN_ID_CCB              0x11    // Cabinet Control Board, low-power, single
#define CAN_ID_CCB_POST         0x10    // charge-post
#define CAN_ID_CCB_POST_MAX     0x19
#define CAN_ID_CCB_CABINET      0x1A    // power cabinet
#define CAN_ID_CCB_CABINET_MAX  0x1E
//                              0x1F    // Illegal
#define CAN_ID_IMI              0x20    // Isolation Monitor Interface
#define CAN_ID_IMI_MAX          0x29
//                              0x2F    // Illegal
#define CAN_ID_PWR              0x30    // PoWeR converter
#define CAN_ID_PWR_MAX          0x3E
//                              0x3F    // Illegal
#define CAN_ID_CPI              0x40    // Charge Protocol Interface
#define CAN_ID_CPI_MAX          0x49
//                              0x4F    // Illegal
#define CAN_ID_MRU              0x50    // MRU
#define CAN_ID_MRU_MAX          0x59
//                              0x5F    // Illegal
#define CAN_ID_NONE             0xFF    // illegal

// PDO id's
enum TPDO {
    NMT_NODE_CTRL = 0,
    PDO_1,
    PDO_2,
    PDO_3,
    PDO_4
};

// A NMT node control message is a 3 byte message of <command(1)><payload(2)>
// The message identifier(s) are choosen to not interfere with the standard
// control messages.
//
// NMT_NODE_CTRL_SHUTDOWN <command> is a broadcast processed by all
// dc-converters, stopping delivery immediateley when the <payload> bit for
// the specific dc-converter is set.
#define NMT_NODE_CTRL_SHUTDOWN      0x40
#define NMT_NODE_CTRL_SHUTDOWN_ALL  0xFFFF

// Reserved for boot, software-download, version querying etc
// Both for boot-loader and application which implies that no
// application shall use CANopen PDO4 .
#define CAN_ID_PDO4_REQUEST         0x500   // maps on PDO4 (rx)
#define CAN_ID_PDO4_RESPONSE        0x480   // maps on PDO4 (tx)

// Reserved for CAN tracing
// Both for boot-loader and application.
#define CAN_FUNCTION_CODE_TRACE     0x700   // maps on NMT Error Control

// Extract CAN node id / function code from 2.0a frame
#define CAN_NODE_ID_MASK            0x07f   // 00001111111
#define CAN_FUNCTION_CODE_MASK      0x780   // 11110000000

#endif /* __INFRA_CANOPEN_SYSTEM_H__ */
