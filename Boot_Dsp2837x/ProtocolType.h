#ifndef __IPROTOCOL_TYPE_H__
#define __IPROTOCOL_TYPE_H__
///////////////////////////////////////////////////////////////////////
typedef enum {
    eProtclType_Xc,
    eProtclType_Raw,
    eProtclType_Q
}eProtclType_t;

#define xcPRO_PC      0x00
#define xcPRO_MASTER  0x01
#define xcPRO_SLAVE   0x02
#define xcPRO_DISPLAY 0x03


typedef enum {
    eComHW_SciA,
    eComHW_SciB,
    eComHW_Rsvd
}eComHW_Type_t;


#endif
//__IPROTOCOL_TYPE_H__
