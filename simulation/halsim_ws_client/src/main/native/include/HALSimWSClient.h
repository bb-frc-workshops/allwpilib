/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <WSProviderContainer.h>
#include <WSProvider_SimDevice.h>
#include <wpi/json.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>

namespace wpilibws {

class HALSimWSClientConnection;

class HALSimWS {
 public:
  static std::shared_ptr<HALSimWS> GetInstance() { return g_instance; }
  static void SetInstance(std::shared_ptr<HALSimWS> inst) { g_instance = inst; }

  explicit HALSimWS(ProviderContainer& providers,
                    HALSimWSProviderSimDevices& simDevicesProvider)
      : m_providers(providers), m_simDevicesProvider(simDevicesProvider) {}
  HALSimWS(const HALSimWS&) = delete;
  HALSimWS& operator=(const HALSimWS&) = delete;

  bool Initialize();
  static void Main(void*);
  static void Exit(void*);

  bool RegisterWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);
  void CloseWebsocket(std::shared_ptr<HALSimBaseWebSocketConnection> hws);

  void OnNetValueChanged(const wpi::json& msg);

  std::string GetTargetHost() const { return m_host; }
  std::string GetTargetUri() const { return m_uri; }
  int GetTargetPort() { return m_port; }
  std::shared_ptr<wpi::uv::Loop> GetLoop() { return m_loop; }

 private:
  static std::shared_ptr<HALSimWS> g_instance;

  void MainLoop();

  void AttemptConnect();

  bool m_tcp_connected = false;
  std::shared_ptr<wpi::uv::Timer> m_connect_timer;
  int m_connect_attempts = 0;

  std::weak_ptr<HALSimBaseWebSocketConnection> m_hws;

  ProviderContainer& m_providers;
  HALSimWSProviderSimDevices& m_simDevicesProvider;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_tcp_client;

  std::string m_host;
  std::string m_uri;
  int m_port;
};

}  // namespace wpilibws
