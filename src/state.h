// =============================================================================
// Template-Project | State
// =============================================================================

#ifndef _TEMPLATE_PROJECT_STATE_
#define _TEMPLATE_PROJECT_STATE_

#include "hardware.h"

namespace State
{
    enum States : uint8_t
    {
        st_connect,
        st_connect_mqtt,
        st_idle,
        st_error
    };

    /// @brief State driver function
    ///
    void stateDriver();

    /// @brief Handler for the WiFi Connect state
    ///
    void stateConnectWifi();

    /// @brief Handler for the MQTT Connect state
    void stateConnectMQTT();

    /// @brief Handler for the Idle state
    ///
    void stateIdle();

    /// @brief Handler for the Error state
    ///
    void stateError();
} // namespace State

#endif // _TEMPLATE_PROJECT_STATE_
