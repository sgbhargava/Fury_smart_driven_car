/// DBC file: 243.dbc    Self node: MOTOR
/// This file should be included by a source file, for example: #include "generated.c"
#include <stdbool.h>
#include <stdint.h>



/// Missing in Action structure
typedef struct {
    uint32_t is_mia : 1;          ///< Missing in action flag
    uint32_t mia_counter_ms : 31; ///< Missing in action counter
} mia_info_t;

/// CAN message header structure
typedef struct { 
    uint32_t mid; ///< Message ID of the message
    uint8_t  dlc; ///< Data length of the message
} msg_hdr_t; 

static const msg_hdr_t SENSOR_TX_SONARS_HDR =                 {  200, 6 };
static const msg_hdr_t DRIVER_TX_RESET_MOTOR_HDR =            {   20, 1 };
static const msg_hdr_t DRIVER_TX_STEER_HDR =                  {   21, 1 };
static const msg_hdr_t DRIVER_TX_THROTTLE_HDR =               {   22, 1 };
static const msg_hdr_t MOTOR_TX_MOTOR_HEARTBEAT_HDR =         {  100, 8 };
static const msg_hdr_t MOTOR_TX_SPEED_HDR =                   {  102, 1 };


/// Not generating 'SENSOR_TX_SONARS_t' since we are not the sender or a recipient of any of its signals

/// Message: RESET_MOTOR from 'DRIVER', DLC: 1 byte(s), MID: 20
typedef struct {
    uint8_t RESET_MOTOR_cmd;             ///< B0:0   Destination: MOTOR

    mia_info_t mia_info;
} DRIVER_TX_RESET_MOTOR_t;


/// Message: STEER from 'DRIVER', DLC: 1 byte(s), MID: 21
typedef struct {
    uint8_t DRIVER_STEER_dir;            ///< B3:0  Min: 0 Max: 4   Destination: MOTOR

    mia_info_t mia_info;
} DRIVER_TX_STEER_t;


/// Message: THROTTLE from 'DRIVER', DLC: 1 byte(s), MID: 22
typedef struct {
    uint8_t DRIVER_THROTTLE_stop;        ///< B0:0  Min: 0 Max: 1   Destination: MOTOR
    uint8_t DRIVER_THROTTLE_forward;     ///< B1:1  Min: 0 Max: 1   Destination: MOTOR
    uint8_t DRIVER_THROTTLE_usecustom;   ///< B2:2  Min: 0 Max: 1   Destination: MOTOR
    uint8_t DRIVER_THROTTLE_custom;      ///< B4:3  Min: 0 Max: 3   Destination: MOTOR
    uint8_t DRIVER_THROTTLE_incr;        ///< B5:5  Min: 0 Max: 1   Destination: MOTOR

    mia_info_t mia_info;
} DRIVER_TX_THROTTLE_t;


/// Message: MOTOR_HEARTBEAT from 'MOTOR', DLC: 8 byte(s), MID: 100
typedef struct {
    uint32_t MOTOR_HEARTBEAT_cmd;        ///< B63:0   Destination: DRIVER

    mia_info_t mia_info;
} MOTOR_TX_MOTOR_HEARTBEAT_t;


/// Message: SPEED from 'MOTOR', DLC: 1 byte(s), MID: 102
typedef struct {
    uint8_t MOTOR_SPEED_speed;           ///< B7:0   Destination: DRIVER
    uint8_t MOTOR_SPEED_rpm;             ///< B15:8   Destination: DRIVER

    mia_info_t mia_info;
} MOTOR_TX_SPEED_t;


/// These 'externs' need to be defined in a source file of your project
extern const uint32_t RESET_MOTOR__MIA_MS;
extern const DRIVER_TX_RESET_MOTOR_t RESET_MOTOR__MIA_MSG;
extern const uint32_t STEER__MIA_MS;
extern const DRIVER_TX_STEER_t STEER__MIA_MSG;
extern const uint32_t THROTTLE__MIA_MS;
extern const DRIVER_TX_THROTTLE_t THROTTLE__MIA_MSG;
extern const uint32_t MOTOR_HEARTBEAT__MIA_MS;
extern const MOTOR_TX_MOTOR_HEARTBEAT_t MOTOR_HEARTBEAT__MIA_MSG;
extern const uint32_t SPEED__MIA_MS;
extern const MOTOR_TX_SPEED_t SPEED__MIA_MSG;


/// Encode DRIVER's 'RESET_MOTOR' message
/// @returns the message header of this message
static msg_hdr_t DRIVER_TX_RESET_MOTOR_encode(uint64_t *to, DRIVER_TX_RESET_MOTOR_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->RESET_MOTOR_cmd - (0)) / 1.0) + 0.5)) & 0x01;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x01) << 0); ///< 1 bit(s) to B0

    return DRIVER_TX_RESET_MOTOR_HDR;
}


/// Encode DRIVER's 'STEER' message
/// @returns the message header of this message
static msg_hdr_t DRIVER_TX_STEER_encode(uint64_t *to, DRIVER_TX_STEER_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    if(from->DRIVER_STEER_dir < 0) { from->DRIVER_STEER_dir = 0; }
    if(from->DRIVER_STEER_dir > 4) { from->DRIVER_STEER_dir = 4; }
    raw_signal = ((uint64_t)(((from->DRIVER_STEER_dir - (0)) / 1.0) + 0.5)) & 0x0f;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x0f) << 0); ///< 4 bit(s) to B0

    return DRIVER_TX_STEER_HDR;
}


/// Encode DRIVER's 'THROTTLE' message
/// @returns the message header of this message
static msg_hdr_t DRIVER_TX_THROTTLE_encode(uint64_t *to, DRIVER_TX_THROTTLE_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    if(from->DRIVER_THROTTLE_stop < 0) { from->DRIVER_THROTTLE_stop = 0; }
    if(from->DRIVER_THROTTLE_stop > 1) { from->DRIVER_THROTTLE_stop = 1; }
    raw_signal = ((uint64_t)(((from->DRIVER_THROTTLE_stop - (0)) / 1.0) + 0.5)) & 0x01;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x01) << 0); ///< 1 bit(s) to B0

    if(from->DRIVER_THROTTLE_forward < 0) { from->DRIVER_THROTTLE_forward = 0; }
    if(from->DRIVER_THROTTLE_forward > 1) { from->DRIVER_THROTTLE_forward = 1; }
    raw_signal = ((uint64_t)(((from->DRIVER_THROTTLE_forward - (0)) / 1.0) + 0.5)) & 0x01;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x01) << 1); ///< 1 bit(s) to B1

    if(from->DRIVER_THROTTLE_usecustom < 0) { from->DRIVER_THROTTLE_usecustom = 0; }
    if(from->DRIVER_THROTTLE_usecustom > 1) { from->DRIVER_THROTTLE_usecustom = 1; }
    raw_signal = ((uint64_t)(((from->DRIVER_THROTTLE_usecustom - (0)) / 1.0) + 0.5)) & 0x01;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x01) << 2); ///< 1 bit(s) to B2

    if(from->DRIVER_THROTTLE_custom < 0) { from->DRIVER_THROTTLE_custom = 0; }
    if(from->DRIVER_THROTTLE_custom > 3) { from->DRIVER_THROTTLE_custom = 3; }
    raw_signal = ((uint64_t)(((from->DRIVER_THROTTLE_custom - (0)) / 1.0) + 0.5)) & 0x03;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x03) << 3); ///< 2 bit(s) to B3

    if(from->DRIVER_THROTTLE_incr < 0) { from->DRIVER_THROTTLE_incr = 0; }
    if(from->DRIVER_THROTTLE_incr > 1) { from->DRIVER_THROTTLE_incr = 1; }
    raw_signal = ((uint64_t)(((from->DRIVER_THROTTLE_incr - (0)) / 1.0) + 0.5)) & 0x01;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0x01) << 5); ///< 1 bit(s) to B5

    return DRIVER_TX_THROTTLE_HDR;
}


/// Encode MOTOR's 'MOTOR_HEARTBEAT' message
/// @returns the message header of this message
static msg_hdr_t MOTOR_TX_MOTOR_HEARTBEAT_encode(uint64_t *to, MOTOR_TX_MOTOR_HEARTBEAT_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->MOTOR_HEARTBEAT_cmd - (0)) / 1.0) + 0.5)) & 0xffffffffffffffff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0
    bytes[1] |= (((uint8_t)(raw_signal >> 8) & 0xff) << 0); ///< 8 bit(s) to B8
    bytes[2] |= (((uint8_t)(raw_signal >> 16) & 0xff) << 0); ///< 8 bit(s) to B16
    bytes[3] |= (((uint8_t)(raw_signal >> 24) & 0xff) << 0); ///< 8 bit(s) to B24
    bytes[4] |= (((uint8_t)(raw_signal >> 32) & 0xff) << 0); ///< 8 bit(s) to B32
    bytes[5] |= (((uint8_t)(raw_signal >> 40) & 0xff) << 0); ///< 8 bit(s) to B40
    bytes[6] |= (((uint8_t)(raw_signal >> 48) & 0xff) << 0); ///< 8 bit(s) to B48
    bytes[7] |= (((uint8_t)(raw_signal >> 56) & 0xff) << 0); ///< 8 bit(s) to B56

    return MOTOR_TX_MOTOR_HEARTBEAT_HDR;
}


/// Encode MOTOR's 'SPEED' message
/// @returns the message header of this message
static msg_hdr_t MOTOR_TX_SPEED_encode(uint64_t *to, MOTOR_TX_SPEED_t *from)
{
    *to = 0; ///< Default the entire destination data with zeroes
    uint8_t *bytes = (uint8_t*) to;
    uint64_t raw_signal;

    raw_signal = ((uint64_t)(((from->MOTOR_SPEED_speed - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[0] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B0

    raw_signal = ((uint64_t)(((from->MOTOR_SPEED_rpm - (0)) / 1.0) + 0.5)) & 0xff;
    bytes[1] |= (((uint8_t)(raw_signal >> 0) & 0xff) << 0); ///< 8 bit(s) to B8

    return MOTOR_TX_SPEED_HDR;
}


/// Decode DRIVER's 'RESET_MOTOR' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool DRIVER_TX_RESET_MOTOR_decode(DRIVER_TX_RESET_MOTOR_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != DRIVER_TX_RESET_MOTOR_HDR.dlc || hdr->mid != DRIVER_TX_RESET_MOTOR_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0)& 0x01); ///< 1 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->RESET_MOTOR_cmd = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode DRIVER's 'STEER' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool DRIVER_TX_STEER_decode(DRIVER_TX_STEER_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != DRIVER_TX_STEER_HDR.dlc || hdr->mid != DRIVER_TX_STEER_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0)& 0x0f); ///< 4 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_STEER_dir = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode DRIVER's 'THROTTLE' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool DRIVER_TX_THROTTLE_decode(DRIVER_TX_THROTTLE_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != DRIVER_TX_THROTTLE_HDR.dlc || hdr->mid != DRIVER_TX_THROTTLE_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0)& 0x01); ///< 1 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_THROTTLE_stop = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 1)& 0x01); ///< 1 bit(s) from B1
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_THROTTLE_forward = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 2)& 0x01); ///< 1 bit(s) from B2
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_THROTTLE_usecustom = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 3)& 0x03); ///< 2 bit(s) from B3
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_THROTTLE_custom = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 5)& 0x01); ///< 1 bit(s) from B5
    raw_signal    |= (bits_from_byte << 0);
    to->DRIVER_THROTTLE_incr = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode MOTOR's 'MOTOR_HEARTBEAT' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool MOTOR_TX_MOTOR_HEARTBEAT_decode(MOTOR_TX_MOTOR_HEARTBEAT_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != MOTOR_TX_MOTOR_HEARTBEAT_HDR.dlc || hdr->mid != MOTOR_TX_MOTOR_HEARTBEAT_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0)& 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    bits_from_byte = ((bytes[1] >> 0)& 0xff); ///< 8 bit(s) from B8
    raw_signal    |= (bits_from_byte << 8);
    bits_from_byte = ((bytes[2] >> 0)& 0xff); ///< 8 bit(s) from B16
    raw_signal    |= (bits_from_byte << 16);
    bits_from_byte = ((bytes[3] >> 0)& 0xff); ///< 8 bit(s) from B24
    raw_signal    |= (bits_from_byte << 24);
    bits_from_byte = ((bytes[4] >> 0)& 0xff); ///< 8 bit(s) from B32
    raw_signal    |= (bits_from_byte << 32);
    bits_from_byte = ((bytes[5] >> 0)& 0xff); ///< 8 bit(s) from B40
    raw_signal    |= (bits_from_byte << 40);
    bits_from_byte = ((bytes[6] >> 0)& 0xff); ///< 8 bit(s) from B48
    raw_signal    |= (bits_from_byte << 48);
    bits_from_byte = ((bytes[7] >> 0)& 0xff); ///< 8 bit(s) from B56
    raw_signal    |= (bits_from_byte << 56);
    to->MOTOR_HEARTBEAT_cmd = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Decode MOTOR's 'SPEED' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool MOTOR_TX_SPEED_decode(MOTOR_TX_SPEED_t *to, const uint64_t *from, const msg_hdr_t *hdr)
{
    const bool success = true;
    if (NULL != hdr && (hdr->dlc != MOTOR_TX_SPEED_HDR.dlc || hdr->mid != MOTOR_TX_SPEED_HDR.mid)) {
        return !success;
    }
    uint64_t raw_signal;
    uint64_t bits_from_byte;
    const uint8_t *bytes = (const uint8_t*) from;

    raw_signal = 0;
    bits_from_byte = ((bytes[0] >> 0)& 0xff); ///< 8 bit(s) from B0
    raw_signal    |= (bits_from_byte << 0);
    to->MOTOR_SPEED_speed = (raw_signal * 1.0) + (0);

    raw_signal = 0;
    bits_from_byte = ((bytes[1] >> 0)& 0xff); ///< 8 bit(s) from B8
    raw_signal    |= (bits_from_byte << 0);
    to->MOTOR_SPEED_rpm = (raw_signal * 1.0) + (0);

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter
    return success;
}


/// Handle the MIA for DRIVER's 'RESET_MOTOR' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool DRIVER_TX_RESET_MOTOR_handle_mia(DRIVER_TX_RESET_MOTOR_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= RESET_MOTOR__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = RESET_MOTOR__MIA_MSG;
        msg->mia_info.mia_counter_ms = RESET_MOTOR__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for DRIVER's 'STEER' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool DRIVER_TX_STEER_handle_mia(DRIVER_TX_STEER_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= STEER__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = STEER__MIA_MSG;
        msg->mia_info.mia_counter_ms = STEER__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for DRIVER's 'THROTTLE' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool DRIVER_TX_THROTTLE_handle_mia(DRIVER_TX_THROTTLE_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= THROTTLE__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = THROTTLE__MIA_MSG;
        msg->mia_info.mia_counter_ms = THROTTLE__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for MOTOR's 'MOTOR_HEARTBEAT' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool MOTOR_TX_MOTOR_HEARTBEAT_handle_mia(MOTOR_TX_MOTOR_HEARTBEAT_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= MOTOR_HEARTBEAT__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = MOTOR_HEARTBEAT__MIA_MSG;
        msg->mia_info.mia_counter_ms = MOTOR_HEARTBEAT__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for MOTOR's 'SPEED' message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter is not reset, and goes beyond the MIA value, the MIA flag is set
static inline bool MOTOR_TX_SPEED_handle_mia(MOTOR_TX_SPEED_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= SPEED__MIA_MS);

    if (!msg->mia_info.is_mia) { 
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { 
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = SPEED__MIA_MSG;
        msg->mia_info.mia_counter_ms = SPEED__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}
