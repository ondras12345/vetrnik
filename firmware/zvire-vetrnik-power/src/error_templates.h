#pragma once

#include <errm.h>

// error weights:
// 10 - emergency stop
const errm_template etemplate_settings_changed = { 1, "sett changed" };  // should reset to apply
const errm_template etemplate_comms_arg = { 1, "comms arg" };
const errm_template etemplate_comms_mode = { 1, "comms mode" };
const errm_template etemplate_comms_malformed = { 1, "comms malformed" };
const errm_template etemplate_comms_timeout = { 10, "comms timeout" };
const errm_template etemplate_OVP_stop = { 5, "OVP stop" };
const errm_template etemplate_OVP_short = { 10, "OVP short" };
const errm_template etemplate_emergency = { 10, "emergency" };
const errm_template etemplate_temperature = { 10, "temperature" };
