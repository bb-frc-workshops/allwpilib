#include <iostream>

#include <hal/Main.h>

#include "HALSimWSClient.h"
#include <WSProviderContainer.h>
#include <WSProvider_DIO.h>
#include <WSProvider_Analog.h>
#include <WSProvider_DriverStation.h>
#include <WSProvider_Encoder.h>
#include <WSProvider_PWM.h>
#include <WSProvider_Relay.h>
#include <WSProvider_RoboRIO.h>
#include <WSProvider_dPWM.h>
#include <WSProvider_SimDevice.h>

using namespace wpilibws;

static ProviderContainer providers;
static HALSimWSProviderSimDevices simDevices(providers);

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif

int HALSIM_InitExtension(void) {
  std::cout << "HALSim WS Client Extension Initializing" << std::endl;

  auto hws = std::make_shared<HALSimWS>(providers);
  HALSimWS::SetInstance(hws);

  if (!hws->Initialize()) {
    return -1;
  }

  WSRegisterFunc registerFunc = std::bind(&ProviderContainer::Add, &providers, std::placeholders::_1, std::placeholders::_2);

  HALSimWSProviderAnalogIn::Initialize(registerFunc);
  HALSimWSProviderAnalogOut::Initialize(registerFunc);
  HALSimWSProviderDIO::Initialize(registerFunc);
  HALSimWSProviderDigitalPWM::Initialize(registerFunc);
  HALSimWSProviderDriverStation::Initialize(registerFunc);
  HALSimWSProviderEncoder::Initialize(registerFunc);
  HALSimWSProviderPWM::Initialize(registerFunc);
  HALSimWSProviderRelay::Initialize(registerFunc);
  // HALSimWSProviderRoboRIO::Initialize(registerFunc);

  simDevices.Initialize();

  HAL_SetMain(nullptr, HALSimWS::Main, HALSimWS::Exit);

  std::cout << "HALSim WS Client Extension Initialized" << std::endl;
  return 0;
}


}