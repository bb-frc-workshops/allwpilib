/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PWM.h"

#include <hal/Ports.h>
#include <hal/simulation/PWMData.h>

namespace wpilibws {

void HALSimWSProviderPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPWM>("PWM", HAL_GetNumPWMChannels(),
                                       HALSIM_RegisterPWMAllCallbacks,
                                       webRegisterFunc);
}

wpi::json HALSimWSProviderPWM::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);
  bool sendDiffOnly = (cbType != "");

  wpi::json result;

  if (cbType == "Initialized" || !sendDiffOnly) {
    result["<init"] = static_cast<bool>(HALSIM_GetPWMInitialized(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "Speed" || !sendDiffOnly) {
    result["<speed"] = HALSIM_GetPWMSpeed(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "Position" || !sendDiffOnly) {
    result["<position"] = HALSIM_GetPWMPosition(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "RawValue" || !sendDiffOnly) {
    result["<raw"] = HALSIM_GetPWMRawValue(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "PeriodScale" || !sendDiffOnly) {
    result["<period_scale"] = HALSIM_GetPWMPeriodScale(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "ZeroLatch" || !sendDiffOnly) {
    result["<zero_latch"] =
        static_cast<bool>(HALSIM_GetPWMZeroLatch(m_channel));
    if (sendDiffOnly) return result;
  }

  return result;
}

}  // namespace wpilibws
