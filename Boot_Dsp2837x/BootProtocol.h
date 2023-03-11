/*=============================================================================
 * Filename: BootProtocol.h
 *=============================================================================
 * Description:
 * Header for boot protocol defines,
*=============================================================================*/
#ifndef BOOT_PROTOCOL_H
#define BOOT_PROTOCOL_H

#ifndef CFirmwareRunningLayer_APP
#define CFirmwareRunningLayer_APP   0x11
#endif

#ifndef CFirmwareRunningLayer_BOOT
#define CFirmwareRunningLayer_BOOT  0x22
#endif

/* Messages */
/* Format REQuest:  <CMD-REQ> <placeholder> [<PARAM>...] */
/*        ReSPonse: <CMD-RSP> <RESULT> [<PARAM>...]      */
#define BOOTMSG_MAX_SIZE                                    (8)  // Maximum size of boot messages.
#define BOOTMSG_FLASH_REQUEST_SIZE                          (12) // excluding start-of-frame-byte
#define BOOTMSG_FLASH_RESPONSE_SIZE                         (5)
#define BOOTMSG_REQUEST_BUFFER_CMD_POSITION                 (0)

#define BOOTMSG_REQUEST_GET_IDENTIFICATION                  (0x01)
#define BOOTMSG_REQUEST_GET_VERSION                         (0x02)
#define BOOTMSG_REQUEST_BOARDS_RESET                        (0x03)
#define BOOTMSG_REQUEST_TRIGGER_WATCHDOG                    (0x04)
#define BOOTMSG_REQUEST_RESET_BOOTSTATE                     (0x05)

#define BOOTMSG_REQUEST_STAY_IN_BOOT                        (0x11)
#define BOOTMSG_REQUEST_SET_POSITION_TRANSMITTER            (0x12)
#define BOOTMSG_REQUEST_READ_DISCOVERED_POSITION            (0x13)
#define BOOTMSG_REQUEST_WRITE_NODE_ID                       (0x14)
#define BOOTMSG_REQUEST_WRITE_BUS_TERMINATION               (0x15)

#define BOOTMSG_REQUEST_INITIATE_UPGRADE                    (0x21)
#define BOOTMSG_REQUEST_DATA_TRANSFER                       (0x22)
#define BOOTMSG_REQUEST_CABINET_RESET                       (0x23)
#define BOOTMSG_REQUEST_DATA_TRANSFER_FINISH                (0x24)

#define BOOTMSG_DATA_TRANSFER_FINISH_SUCCESS             0
#define BOOTMSG_DATA_TRANSFER_FINISH_IMAGE_ERROR         1
#define BOOTMSG_DATA_TRANSFER_FINISH_CRC_ERROR           2
#define BOOTMSG_DATA_TRANSFER_FINISH_FLASH_WRITE_ERROR   3

#define BOOTMSG_REQUEST_FLASH_WRITE6                        (0x31)
#define BOOTMSG_REQUEST_FLASH_READ                          (0x32)
#define BOOTMSG_REQUEST_FLASH_ERASE                         (0x33)
#define BOOTMSG_REQUEST_FLASH_GET_VERSION                   (0x34)
#define BOOTMSG_REQUEST_FLASH_START_APPLICATION             (0x35)

#define BOOTMSG_REQUEST_USR                                 (0xE0)

#define BOOTMSG_UPGRADE_STAT_OK                          0
#define BOOTMSG_UPGRADE_STAT_TOO_BIG                     1
#define BOOTMSG_UPGRADE_STAT_INVALID                     2
#define BOOTMSG_UPGRADE_WRONG_DEVICE                     3
#define BOOTMSG_UPGRADE_UNSUPPORTED_ORDINAL              4
#define BOOTMSG_UPGRADE_ERASE_FAILED                     5

#define BOOTMSG_IDENTIFICATION_TERMINATION_MASK             (0x80)

#endif /* BOOT_PROTOCOL_H */
