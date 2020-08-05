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
#include <wpi/json.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>


namespace uv = wpi::uv;

namespace wpilibws {

class WebServerClientConnectionTest;

class WebServerClientTest {
 public:
  static std::shared_ptr<WebServerClientTest> GetInstance() { return g_instance; }
  static void SetInstance(std::shared_ptr<WebServerClientTest> inst) { g_instance = inst; }

  explicit WebServerClientTest() {}
  WebServerClientTest(const WebServerClientTest&) = delete;
  WebServerClientTest& operator=(const WebServerClientTest&) = delete;

  bool Initialize(std::shared_ptr<uv::Loop> & loop);
  void StartTimer();
  void AttemptConnect();
  static void Exit();

  void SendMessage(const wpi::json& msg);
  const wpi::json & GetMessage();
  void OnNetValueChanged(const wpi::json& msg);

 private:
  static std::shared_ptr<WebServerClientTest> g_instance;


  bool m_tcp_connected = false;
  std::shared_ptr<wpi::uv::Timer> m_connect_timer;
  int m_connect_attempts = 0;

  std::shared_ptr<WebServerClientConnectionTest> m_hws;

  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<wpi::uv::Tcp> m_tcp_client;

  wpi::json m_json;
};

}  // namespace wpilibws
