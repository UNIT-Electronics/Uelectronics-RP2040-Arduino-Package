#include "USBAudio.h"

#if !defined(USE_TINYUSB) && !defined(NO_USB)

#include "USB.h"
#include <tusb.h>
#include <cstring>

// ============================================================
// UNIT Electronics USB Audio
// ============================================================

USBAudioClass USBAudio;

// ============================================================
// UAC2 Entity IDs
// ============================================================
//
// These IDs come from TinyUSB's
// TUD_AUDIO_MIC_ONE_CH_DESCRIPTOR.
//
// 0x01 = Input Terminal
// 0x02 = Feature Unit
// 0x03 = Output Terminal
// 0x04 = Clock Source
//

#define UAC2_ENTITY_INPUT_TERMINAL   0x01
#define UAC2_ENTITY_FEATURE_UNIT     0x02
#define UAC2_ENTITY_OUTPUT_TERMINAL  0x03
#define UAC2_ENTITY_CLOCK_SOURCE     0x04


// ============================================================
// USB Audio Runtime State
// ============================================================

static uint32_t usbAudioSampleRate = 48000;
static bool usbAudioMute = false;
static int16_t usbAudioVolume = 0;

static volatile uint32_t dbgGetCount = 0;
static volatile uint32_t dbgSetCount = 0;
static volatile uint32_t dbgSetItfCount = 0;

static volatile uint8_t dbgGetEntity = 0;
static volatile uint8_t dbgGetControl = 0;
static volatile uint8_t dbgGetChannel = 0;
static volatile uint8_t dbgGetRequest = 0;

static volatile uint8_t dbgSetEntity = 0;
static volatile uint8_t dbgSetControl = 0;
static volatile uint8_t dbgSetRequest = 0;

static volatile uint8_t dbgItf = 0;
static volatile uint8_t dbgAlt = 0;

static volatile uint32_t dbgTxDoneCount = 0;
static volatile uint32_t dbgTxBytes = 0;
static volatile uint8_t dbgTxEndpoint = 0;
static volatile uint8_t dbgTxAlt = 0;

static volatile uint32_t dbgTxPreCount = 0;
static volatile uint8_t dbgTxPreEndpoint = 0;
static volatile uint8_t dbgTxPreAlt = 0;

// ============================================================
// TinyUSB core internal diagnostics
// ============================================================

extern "C" {
    extern volatile uint32_t usb_audio_dbg_tx_entry;
    extern volatile uint32_t usb_audio_dbg_as_fail;
    extern volatile uint32_t usb_audio_dbg_alt_zero;
    extern volatile uint32_t usb_audio_dbg_pre_reached;

    extern volatile uint8_t usb_audio_dbg_ep_in_as_itf;
    extern volatile uint8_t usb_audio_dbg_idx_itf;
    extern volatile uint8_t usb_audio_dbg_alt_value;
    extern volatile uint8_t usb_audio_dbg_ep_in;

    extern volatile uint32_t usb_audio_dbg_ep_seen;
    extern volatile uint32_t usb_audio_dbg_ep_in_match;

    extern volatile uint8_t usb_audio_dbg_seen_ep_addr;
    extern volatile uint8_t usb_audio_dbg_seen_ep_dir;
    extern volatile uint8_t usb_audio_dbg_seen_ep_usage;
    extern volatile uint8_t usb_audio_dbg_seen_ep_attr;
    extern volatile uint16_t usb_audio_dbg_seen_ep_size;

    extern volatile uint32_t usb_audio_dbg_itf_match;
    extern volatile uint8_t usb_audio_dbg_req_itf;
    extern volatile uint8_t usb_audio_dbg_req_alt;
    extern volatile uint8_t usb_audio_dbg_matched_eps;
    extern volatile uint8_t usb_audio_dbg_found_eps;
    extern volatile uint32_t usb_audio_dbg_alt1_match;
    extern volatile uint8_t usb_audio_dbg_alt1_itf;
    extern volatile uint8_t usb_audio_dbg_alt1_n_eps;
    extern volatile uint8_t usb_audio_dbg_alt1_found_eps;

    extern volatile uint32_t usb_audio_dbg_setitf_entry;

    extern volatile uint32_t usb_audio_dbg_alt1_request;
    extern volatile uint8_t usb_audio_dbg_alt1_request_itf;

    extern volatile uint32_t usb_audio_dbg_alt1_lookup_ok;
    extern volatile uint32_t usb_audio_dbg_alt1_lookup_fail;

    extern volatile uint32_t usb_audio_dbg_iso_alloc_count;
    extern volatile uint8_t  usb_audio_dbg_iso_alloc_ep;
    extern volatile uint16_t usb_audio_dbg_iso_alloc_size;

    extern volatile uint32_t usb_audio_dbg_iso_activate_count;
    extern volatile uint8_t  usb_audio_dbg_iso_activate_ep;
    extern volatile uint16_t usb_audio_dbg_iso_activate_size;

    extern volatile uint32_t usb_audio_dbg_dcd_xfer_count;
    extern volatile uint8_t  usb_audio_dbg_dcd_xfer_ep;
    extern volatile uint16_t usb_audio_dbg_dcd_xfer_size;
}


// ============================================================
// Constructor
// ============================================================

USBAudioClass::USBAudioClass()
    : _interfaceID(0),
      _endpointIn(0),
      _stringID(0) {

    // --------------------------------------------------------
    // DEBUG PRODUCT NAME
    // --------------------------------------------------------
    //
    // This is intentionally done in the constructor.
    //
    // The constructor runs during static initialization,
    // before setup().
    //
    // Therefore the product string should already be defined
    // when the Arduino-Pico USB core builds its descriptors.
    //

    USB.setProduct(
        "UNIT Pulsar RP2350A AUDIO TEST"
    );


    // --------------------------------------------------------
    // Allocate Audio IN endpoint
    // --------------------------------------------------------

    _endpointIn =
        USB.registerEndpointIn();


    // --------------------------------------------------------
    // Register Audio interface string
    // --------------------------------------------------------

    _stringID =
        USB.registerString(
            "UNIT Pulsar RP2350A Microphone"
        );


    // --------------------------------------------------------
    // Register USB Audio Function
    // --------------------------------------------------------
    //
    // UAC2 microphone:
    //
    // Interface N
    //     Audio Control
    //
    // Interface N + 1
    //     Audio Streaming
    //
    // Therefore this function consumes 2 interfaces.
    //

    _interfaceID =
        USB.registerInterface(
            2,
            descriptorCallback,
            this,
            TUD_AUDIO_MIC_ONE_CH_DESC_LEN,
            20,
            0
        );
}


// ============================================================
// Descriptor Builder
// ============================================================

void USBAudioClass::descriptorCallback(
    int interfaceNumber,
    uint8_t *dst,
    int len,
    void *param
) {

    USBAudioClass *audio =
        static_cast<USBAudioClass *>(param);


    // --------------------------------------------------------
    // Build TinyUSB UAC2 microphone descriptor
    // --------------------------------------------------------

    uint8_t descriptor[] = {

        TUD_AUDIO_MIC_ONE_CH_DESCRIPTOR(

            // Audio Control interface
            static_cast<uint8_t>(
                interfaceNumber
            ),

            // Interface string
            audio->_stringID,

            // Bytes per sample
            CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX,

            // Valid bits per sample
            CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX * 8,

            // Isochronous Audio IN endpoint
            audio->_endpointIn,

            // Maximum Audio IN packet size
            CFG_TUD_AUDIO_EP_SZ_IN
        )
    };


    // --------------------------------------------------------
    // Safety check
    // --------------------------------------------------------

    if (
        dst == nullptr ||
        len < static_cast<int>(sizeof(descriptor))
    ) {
        return;
    }


    // --------------------------------------------------------
    // Copy descriptor into Arduino-Pico configuration
    // descriptor buffer
    // --------------------------------------------------------

    memcpy(
        dst,
        descriptor,
        sizeof(descriptor)
    );
}


// ============================================================
// Begin
// ============================================================

void USBAudioClass::begin() {

    // Nothing is required here.
    //
    // USB Audio registration happens in the constructor,
    // during static initialization.
    //
    // Arduino-Pico starts the USB device itself.
}


// ============================================================
// USB State
// ============================================================

bool USBAudioClass::mounted() {

    return tud_mounted();
}


bool USBAudioClass::ready() {
    return tud_mounted();
}

// ============================================================
// Debug report
// ============================================================

void USBAudioClass::debugReport() {

    static uint32_t oldGetCount = 0;
    static uint32_t oldSetCount = 0;
    static uint32_t oldItfCount = 0;

    static uint32_t lastAlive = 0;

    static uint32_t lastTxReport = 0;
    static uint32_t previousTxCount = 0;
    static uint32_t previousTxBytes = 0;

    // --------------------------------------------------------
    // Alive message
    // --------------------------------------------------------

    if (millis() - lastAlive >= 2000) {

        lastAlive = millis();

        Serial.println(
            "USBAudio debugReport alive"
        );
    }


    // --------------------------------------------------------
    // Read debug counters
    // --------------------------------------------------------

    const uint32_t getCount =
        dbgGetCount;

    const uint32_t setCount =
        dbgSetCount;

    const uint32_t itfCount =
        dbgSetItfCount;


    // --------------------------------------------------------
    // GET requests
    // --------------------------------------------------------

    if (getCount != oldGetCount) {

        oldGetCount = getCount;

        Serial.print("USB AUDIO GET #");
        Serial.print(getCount);

        Serial.print(" entity=");
        Serial.print(dbgGetEntity);

        Serial.print(" ctrl=");
        Serial.print(dbgGetControl);

        Serial.print(" ch=");
        Serial.print(dbgGetChannel);

        Serial.print(" req=");
        Serial.println(dbgGetRequest);
    }


    // --------------------------------------------------------
    // SET requests
    // --------------------------------------------------------

    if (setCount != oldSetCount) {

        oldSetCount = setCount;

        Serial.print("USB AUDIO SET #");
        Serial.print(setCount);

        Serial.print(" entity=");
        Serial.print(dbgSetEntity);

        Serial.print(" ctrl=");
        Serial.print(dbgSetControl);

        Serial.print(" req=");
        Serial.println(dbgSetRequest);
    }


    // --------------------------------------------------------
    // SET_INTERFACE
    // --------------------------------------------------------

    if (itfCount != oldItfCount) {

        oldItfCount = itfCount;

        Serial.print(
            "USB AUDIO SET_INTERFACE #"
        );

        Serial.print(itfCount);

        Serial.print(" itf=");
        Serial.print(dbgItf);

        Serial.print(" alt=");
        Serial.println(dbgAlt);
    }


    // --------------------------------------------------------
    // Audio IN transfer statistics
    // --------------------------------------------------------

    if (millis() - lastTxReport >= 1000) {

        lastTxReport = millis();

        const uint32_t txCount =
            dbgTxDoneCount;

        const uint32_t txBytes =
            dbgTxBytes;

        const uint32_t txPerSecond =
            txCount - previousTxCount;

        const uint32_t bytesPerSecond =
            txBytes - previousTxBytes;

        previousTxCount = txCount;
        previousTxBytes = txBytes;

        Serial.print("USB AUDIO TX/s=");
        Serial.print(txPerSecond);

        Serial.print(" bytes/s=");
        Serial.print(bytesPerSecond);

        Serial.print(" ep=0x");
        Serial.print(dbgTxEndpoint, HEX);

        Serial.print(" alt=");
        Serial.println(dbgTxAlt);

        Serial.print(" pre=");
        Serial.print(dbgTxPreCount);

        Serial.print(" preEP=0x");
        Serial.print(dbgTxPreEndpoint, HEX);

        Serial.print(" preAlt=");
        Serial.print(dbgTxPreAlt);

        Serial.print("  CORE entry=");
        Serial.print(usb_audio_dbg_tx_entry);

        Serial.print(" asFail=");
        Serial.print(usb_audio_dbg_as_fail);

        Serial.print(" altZero=");
        Serial.print(usb_audio_dbg_alt_zero);

        Serial.print(" preReached=");
        Serial.print(usb_audio_dbg_pre_reached);

        Serial.print(" ASitf=");
        Serial.print(usb_audio_dbg_ep_in_as_itf);

        Serial.print(" idx=");
        Serial.print(usb_audio_dbg_idx_itf);

        Serial.print(" altVal=");
        Serial.print(usb_audio_dbg_alt_value);

        Serial.print(" coreEP=0x");
        Serial.print(usb_audio_dbg_ep_in, HEX);

        Serial.print(" epSeen=");
        Serial.print(usb_audio_dbg_ep_seen);

        Serial.print(" inMatch=");
        Serial.print(usb_audio_dbg_ep_in_match);

        Serial.print(" seenAddr=0x");
        Serial.print(usb_audio_dbg_seen_ep_addr, HEX);

        Serial.print(" dir=");
        Serial.print(usb_audio_dbg_seen_ep_dir);

        Serial.print(" usage=");
        Serial.print(usb_audio_dbg_seen_ep_usage);

        Serial.print(" xfer=");
        Serial.print(usb_audio_dbg_seen_ep_attr);

        Serial.print(" size=");
        Serial.println(usb_audio_dbg_seen_ep_size);

        Serial.print(" itfMatch=");
        Serial.print(usb_audio_dbg_itf_match);

        Serial.print(" reqItf=");
        Serial.print(usb_audio_dbg_req_itf);

        Serial.print(" reqAlt=");
        Serial.print(usb_audio_dbg_req_alt);

        Serial.print(" nEps=");
        Serial.print(usb_audio_dbg_matched_eps);

        Serial.print(" foundEps=");
        Serial.println(usb_audio_dbg_found_eps);

        Serial.print(" ALT1 match=");
        Serial.print(usb_audio_dbg_alt1_match);

        Serial.print(" itf=");
        Serial.print(usb_audio_dbg_alt1_itf);

        Serial.print(" nEps=");
        Serial.print(usb_audio_dbg_alt1_n_eps);

        Serial.print(" foundEps=");
        Serial.println(usb_audio_dbg_alt1_found_eps);

        Serial.print(" SETITF entry=");
        Serial.print(usb_audio_dbg_setitf_entry);

        Serial.print(" ALT1req=");
        Serial.print(usb_audio_dbg_alt1_request);

        Serial.print(" itf=");
        Serial.print(usb_audio_dbg_alt1_request_itf);

        Serial.print(" lookupOK=");
        Serial.print(usb_audio_dbg_alt1_lookup_ok);

        Serial.print(" lookupFAIL=");
        Serial.println(usb_audio_dbg_alt1_lookup_fail);

        Serial.print("ISO alloc=");
        Serial.print(usb_audio_dbg_iso_alloc_count);

        Serial.print(" ep=0x");
        Serial.print(usb_audio_dbg_iso_alloc_ep, HEX);

        Serial.print(" size=");
        Serial.println(usb_audio_dbg_iso_alloc_size);


        Serial.print("ISO activate=");
        Serial.print(usb_audio_dbg_iso_activate_count);

        Serial.print(" ep=0x");
        Serial.print(usb_audio_dbg_iso_activate_ep, HEX);

        Serial.print(" size=");
        Serial.println(usb_audio_dbg_iso_activate_size);


        Serial.print("DCD xfer=");
        Serial.print(usb_audio_dbg_dcd_xfer_count);

        Serial.print(" ep=0x");
        Serial.print(usb_audio_dbg_dcd_xfer_ep, HEX);

        Serial.print(" size=");
        Serial.println(usb_audio_dbg_dcd_xfer_size);
    }
}


// ============================================================
// Write PCM Samples
// ============================================================

size_t USBAudioClass::write(
    const int16_t *samples,
    size_t sampleCount
) {

    if (
        samples == nullptr ||
        sampleCount == 0
    ) {
        return 0;
    }

    if (!tud_mounted()) {
        return 0;
    }

    const uint16_t bytes =
        static_cast<uint16_t>(
            sampleCount *
            sizeof(int16_t)
        );

    const uint16_t written =
        tud_audio_write(
            reinterpret_cast<const uint8_t *>(samples),
            bytes
        );

    return written /
           sizeof(int16_t);
}

// ============================================================
// TinyUSB UAC2 GET Entity Request
// ============================================================

extern "C"
bool tud_audio_get_req_entity_cb(
    uint8_t rhport,
    tusb_control_request_t const *request
) {

    const uint8_t entityID =
        static_cast<uint8_t>(
            request->wIndex >> 8
        );

    const uint8_t controlSelector =
        static_cast<uint8_t>(
            request->wValue >> 8
        );

    const uint8_t channelNumber =
        static_cast<uint8_t>(
            request->wValue & 0xFF
        );

    dbgGetEntity  = entityID;
    dbgGetControl = controlSelector;
    dbgGetChannel = channelNumber;
    dbgGetRequest = request->bRequest;
    dbgGetCount++;    

    // ========================================================
    // CLOCK SOURCE
    // ========================================================

    if (
        entityID ==
        UAC2_ENTITY_CLOCK_SOURCE
    ) {

        // ----------------------------------------------------
        // Sampling Frequency Control
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_CS_CTRL_SAM_FREQ
        ) {

            // ------------------------------------------------
            // GET CUR
            // ------------------------------------------------

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                audio_control_cur_4_t cur = {
                    .bCur = static_cast<int32_t>(usbAudioSampleRate)
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &cur,
                        sizeof(cur)
                    );
            }


            // ------------------------------------------------
            // GET RANGE
            // ------------------------------------------------

            if (
                request->bRequest ==
                AUDIO_CS_REQ_RANGE
            ) {

                audio_control_range_4_n_t(1) range = {
                    .wNumSubRanges = 1,
                    .subrange = {
                        {
                            .bMin = 48000,
                            .bMax = 48000,
                            .bRes = 0
                        }
                    }
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &range,
                        sizeof(range)
                    );
            }
        }


        // ----------------------------------------------------
        // Clock Valid Control
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_CS_CTRL_CLK_VALID
        ) {

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                audio_control_cur_1_t cur = {
                    .bCur = 1
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &cur,
                        sizeof(cur)
                    );
            }
        }


        return false;
    }

// ========================================================
// INPUT TERMINAL
// ========================================================

    if (
        entityID ==
        UAC2_ENTITY_INPUT_TERMINAL
    ) {

        // ----------------------------------------------------
        // Terminal Connector
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_TE_CTRL_CONNECTOR
        ) {

            audio_desc_channel_cluster_t ret;

            ret.bNrChannels = 1;
            ret.bmChannelConfig =
                static_cast<audio_channel_config_t>(0);
            ret.iChannelNames = 0;

            return
                tud_audio_buffer_and_schedule_control_xfer(
                    rhport,
                    request,
                    &ret,
                    sizeof(ret)
                );
        }

        return false;
    }
    // ========================================================
    // FEATURE UNIT
    // ========================================================

    if (
        entityID ==
        UAC2_ENTITY_FEATURE_UNIT
    ) {

        // ----------------------------------------------------
        // Mute
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_FU_CTRL_MUTE
        ) {

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                audio_control_cur_1_t cur = {
                    .bCur = static_cast<int8_t>(usbAudioMute ? 1 : 0)
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &cur,
                        sizeof(cur)
                    );
            }
        }


        // ----------------------------------------------------
        // Volume
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_FU_CTRL_VOLUME
        ) {

            // GET CUR
            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                audio_control_cur_2_t cur = {
                    .bCur = usbAudioVolume
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &cur,
                        sizeof(cur)
                    );
            }


            // GET RANGE
            if (
                request->bRequest ==
                AUDIO_CS_REQ_RANGE
            ) {

                audio_control_range_2_n_t(1) range = {
                    .wNumSubRanges = 1,
                    .subrange = {
                        {
                            // -90 dB
                            .bMin = -90 * 256,

                            // 0 dB
                            .bMax = 0,

                            // 1 dB steps
                            .bRes = 1 * 256
                        }
                    }
                };

                return
                    tud_audio_buffer_and_schedule_control_xfer(
                        rhport,
                        request,
                        &range,
                        sizeof(range)
                    );
            }
        }


        return false;
    }


    return false;
}


// ============================================================
// TinyUSB UAC2 SET Entity Request
// ============================================================

extern "C"
bool tud_audio_set_req_entity_cb(
    uint8_t rhport,
    tusb_control_request_t const *request,
    uint8_t *buffer
) {

    const uint8_t entityID =
        static_cast<uint8_t>(
            request->wIndex >> 8
        );

    const uint8_t controlSelector =
        static_cast<uint8_t>(
            request->wValue >> 8
        );

    dbgSetEntity  = entityID;
    dbgSetControl = controlSelector;
    dbgSetRequest = request->bRequest;
    dbgSetCount++;

    // ========================================================
    // CLOCK SOURCE
    // ========================================================

    if (
        entityID ==
        UAC2_ENTITY_CLOCK_SOURCE
    ) {

        if (
            controlSelector ==
            AUDIO_CS_CTRL_SAM_FREQ
        ) {

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                const audio_control_cur_4_t *cur =
                    reinterpret_cast<
                        audio_control_cur_4_t *
                    >(buffer);

                // We only support 48 kHz.
                if (
                    cur->bCur ==
                    48000
                ) {
                    usbAudioSampleRate =
                        cur->bCur;

                    return true;
                }

                return false;
            }
        }

        return false;
    }


    // ========================================================
    // FEATURE UNIT
    // ========================================================

    if (
        entityID ==
        UAC2_ENTITY_FEATURE_UNIT
    ) {

        // ----------------------------------------------------
        // Mute
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_FU_CTRL_MUTE
        ) {

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                const audio_control_cur_1_t *cur =
                    reinterpret_cast<
                        audio_control_cur_1_t *
                    >(buffer);

                usbAudioMute =
                    cur->bCur != 0;

                return true;
            }
        }


        // ----------------------------------------------------
        // Volume
        // ----------------------------------------------------

        if (
            controlSelector ==
            AUDIO_FU_CTRL_VOLUME
        ) {

            if (
                request->bRequest ==
                AUDIO_CS_REQ_CUR
            ) {

                const audio_control_cur_2_t *cur =
                    reinterpret_cast<
                        audio_control_cur_2_t *
                    >(buffer);

                usbAudioVolume =
                    cur->bCur;

                return true;
            }
        }


        return false;
    }


    return false;
}

extern "C"
bool tud_audio_set_itf_cb(
    uint8_t rhport,
    tusb_control_request_t const *request
) {
    (void) rhport;

    const uint8_t interfaceNumber =
        static_cast<uint8_t>(
            request->wIndex & 0xFF
        );

    const uint8_t altSetting =
        static_cast<uint8_t>(
            request->wValue & 0xFF
        );

    dbgItf = interfaceNumber;
    dbgAlt = altSetting;
    dbgSetItfCount++;

    return true;
}

extern "C"
bool tud_audio_tx_done_pre_load_cb(
    uint8_t rhport,
    uint8_t func_id,
    uint8_t ep_in,
    uint8_t cur_alt_setting
) {
    (void)rhport;
    (void)func_id;

    dbgTxPreCount++;
    dbgTxPreEndpoint = ep_in;
    dbgTxPreAlt = cur_alt_setting;

    return true;
}

extern "C"
bool tud_audio_tx_done_post_load_cb(
    uint8_t rhport,
    uint16_t n_bytes_copied,
    uint8_t func_id,
    uint8_t ep_in,
    uint8_t cur_alt_setting
) {
    (void)rhport;
    (void)func_id;

    dbgTxDoneCount++;
    dbgTxBytes += n_bytes_copied;
    dbgTxEndpoint = ep_in;
    dbgTxAlt = cur_alt_setting;

    return true;
}

#endif