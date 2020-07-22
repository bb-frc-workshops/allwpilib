/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Encoder.h"

#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>

namespace wpilibws {

void HALSimWSProviderEncoder::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderEncoder>("Encoder", HAL_GetNumEncoders(),
                                           HALSIM_RegisterEncoderAllCallbacks,
                                           webRegisterFunc);
}

wpi::json HALSimWSProviderEncoder::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);

  if (cbType == "Initialized") {
    return {
        {"<init", static_cast<bool>(HALSIM_GetEncoderInitialized(m_channel))}};
  } else if (cbType == "Count") {
    return {{">count", HALSIM_GetEncoderCount(m_channel)}};
  } else if (cbType == "Period") {
    return {{">period", HALSIM_GetEncoderPeriod(m_channel)}};
  } else if (cbType == "Reset") {
    return {{"<reset", static_cast<bool>(HALSIM_GetEncoderReset(m_channel))}};
  } else if (cbType == "MaxPeriod") {
    return {{"<max_period", HALSIM_GetEncoderMaxPeriod(m_channel)}};
  } else if (cbType == "ReverseDirection") {
    return {{"<reverse_direction",
             static_cast<bool>(HALSIM_GetEncoderReverseDirection(m_channel))}};
  } else if (cbType == "SamplesToAverage") {
    return {{"<samples_to_avg", HALSIM_GetEncoderSamplesToAverage(m_channel)}};
  }

  return {};
}

void HALSimWSProviderEncoder::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">count")) != json.end()) {
    HALSIM_SetEncoderCount(m_channel, it.value());
  }
  if ((it = json.find(">period")) != json.end()) {
    HALSIM_SetEncoderPeriod(m_channel, it.value());
  }
}

}  // namespace wpilibws
