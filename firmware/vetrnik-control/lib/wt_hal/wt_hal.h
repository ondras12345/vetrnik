#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define X_ENUM(name, value) name = value,


// mode numbers must start from 0 and be consecutive
#define CONTROL_STRATEGIES(X) \
    /** Contactor shorted. */ \
    X(control_shorted, 0) \
    /** Manual (CLI) control with no timeouts. */ \
    X(control_manual, 1) \
    /** MQTT remote control with timeout. */ \
    X(control_MQTT, 2) \
    /** Control function written in lisp. */ \
    X(control_lisp, 3)

typedef enum {
    CONTROL_STRATEGIES(X_ENUM)
} control_strategy_t;

extern const char * control_strategies[];


typedef struct {
    bool hardware: 1;  ///< status of external enable pin
    bool software: 1;
    bool overall: 1;  ///< (hardware && software)
} enabled_t;


typedef struct {
    unsigned long retrieved_millis;
    bool valid;         ///< false before first status report is received
    uint16_t time;      ///< should increment every second
    uint8_t mode;       ///< see vetrnik-power/src/globals.h mode_t
    uint8_t duty;       ///< output duty cycle, 0-255
    uint8_t OCP_max_duty;  ///< OCP duty cycle limit
    uint16_t RPM;       ///< turbine RPM
    uint16_t voltage;   ///< V, *10
    uint16_t current;   ///< mA
    enabled_t enabled;  ///< @see enabled_t
    bool emergency;     ///< emergency stop mode, requires physical reset
    uint16_t temperature_heatsink;  ///< 'C, *10
    uint16_t temperature_rectifier;  ///< 'C, *10
    uint8_t fan;        ///< fan output duty cycle, 0-255
    uint8_t error_count;
    bool last5m;        ///< true if power was being generated in the last 5 minutes
    // setting index and value ignored
} power_board_status_t;


// mode numbers must start from 0 and be consecutive
#define POWER_BOARD_MODES(X) \
    /** Generator output shorted by three-phase contactor.
     Fault condition or fresh start. */ \
    X(shorted, 0) \
    /** Trying to stop the turbine without directly shorting it.
    Used when target water temperature is reached. */ \
    X(stopping, 1) \
    /** Constant duty cycle. */ \
    X(const_duty, 2) \
    /** Start from shorted state to const_duty. */ \
    X(start, 3)

typedef enum {
    POWER_BOARD_MODES(X_ENUM)
    POWER_MODE_LAST_
} power_board_mode_t;

extern const char * power_board_modes[];


/// Wind turbine statistics
typedef struct {
    /// Energy since last reset in Wh
    uint32_t energy;
    /// Sub-Wh energy in Ws*10, always less than 36000
    uint16_t energy_Ws10;
} stats_t;


// output numbers must start from 0 and be consecutive
#define OUT_NAMES(X) \
    X(LED_BLUE, 0) \
    X(LED_RED, 1) \
    X(PUMP, 2) \
    X(REL2, 3)

#define X_ENUM_OUT(name, value) OUT_##name = value,
typedef enum {
    OUT_NAMES(X_ENUM_OUT)
    OUT_LAST_
} digital_output_t;
#undef X_EUM_OUT

extern const char * digital_output_names[];

/**
 * Wind turbine HAL.
 *
 * Any implementation must implement all functions, there mustn't be any NULL
 * pointers.
 */
typedef struct
{
    /// Get power board status
    power_board_status_t (*pwr_get_status)();
    /// Set requested duty cycle on power board.
    void (*pwr_set_duty)(uint8_t);
    /// Set requested mode on power board.
    void (*pwr_set_mode)(power_board_mode_t);
    /// Set enabled.software on power board.
    void (*pwr_set_sw_enable)(bool);
    /// Clear all errors on power board.
    void (*pwr_clear_errors)();
    /// Reset the power board MCU.
    void (*pwr_reset)();
    /// Cause watchdog reset on power board MCU.
    void (*pwr_test_WDT)();
    /**
     * Control power board REL outputs.
     * \param pin REL output number
     * \param value true (connected to GND) or false (floating - open collector)
     * \return true on success, else false
     */
    bool (*pwr_REL_write)(uint8_t pin, bool value);
    /**
     * Read status of power board REL output.
     * \param pin REL output number
     * \return 0 if off (open collector), 1 if on, -1 on error
     */
    uint8_t (*pwr_REL_read)(uint8_t pin);


    void (*ctrl_set_strategy)(control_strategy_t s);

    /**
     * Set control strategy from its string name.
     * \return true on success, otherwise false.
     */
    bool (*ctrl_set_strategy_str)(const char * str);

    control_strategy_t (*ctrl_get_strategy)();

    /**
     * Control contactor state.
     *
     * Call when you want to connect the generator to the device.
     * If all other conditions are met (e.g. !emergency), the contactor
     * will operate and be kept that way until settings.contactor_debounce_min
     * minutes pass since the last call to this function.
     *
     * In normal operation, this function should be called repeatedly as
     * long as the turbine is spinning.
     */
    void (*ctrl_contactor_set)();

    /**
     * Get state of contactor control.
     *
     * This checks whether the contactor is allowed to operate, not whether it
     * is actually operated.
     *
     * Even if this function returns value other than -1, the contactor could
     * still be off e.g. if we are in emergency mode.
     *
     * \return remaining time in ms if on, -1 otherwise
     */
    unsigned long (*ctrl_contactor_get)();

    /// Set circulation pump state.
    void (*pump_set)(bool);
    /// Get circulation pump state.
    bool (*pump_get)();

    /// Get wind turbine statistics.
    stats_t (*stats_get)();

    /// Set state of a digital output
    void (*out_set)(digital_output_t out, bool s);
    /// Get state of a digital output
    bool (*out_get)(digital_output_t out);
    /// Verify if provided value is a valid digital_output_t
    /// \return true if valid
    bool (*out_validate)(int v);

    /// Get wind velocity in m/s (or NaN if unknown)
    float (*vwind)();
} wt_hal_t;


// common implementations
bool wt_hal_out_validate(int v);


#undef X_ENUM

#ifdef __cplusplus
}
#endif
