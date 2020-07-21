/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Relay.h"

#include <hal/Ports.h>
#include <hal/simulation/RelayData.h>

namespace wpilibws {
void HALSimWSProviderRelay::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderRelay>("Relay", HAL_GetNumRelayHeaders(),
                                         HALSIM_RegisterRelayAllCallbacks,
                                         webRegisterFunc);
}

wpi::json HALSimWSProviderRelay::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);

  if (cbType == "InitializedForward") {
    return {
      {"<init_fwd",
       static_cast<bool>(HALSIM_GetRelayInitializedForward(m_channel))}
    };
  }
  else if (cbType == "InitializedReverse") {
    return {
      {"<init_rev",
       static_cast<bool>(HALSIM_GetRelayInitializedReverse(m_channel))}
    };
  }
  else if (cbType == "Forward") {
    return {
      {"<fwd", static_cast<bool>(HALSIM_GetRelayForward(m_channel))}
    };
  }
  else if (cbType == "Reverse") {
    return {
      {"<rev", static_cast<bool>(HALSIM_GetRelayReverse(m_channel))}
    };
  }

  return {};
}

}  // namespace wpilibws
