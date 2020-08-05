/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WebServerClientTest.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>
#include <wpi/uv/util.h>

#include "WebServerClientConnectionTest.h"

static constexpr int kTcpConnectAttemptTimeout = 1000;

namespace uv = wpi::uv;

namespace wpilibws {

std::shared_ptr<WebServerClientTest> WebServerClientTest::g_instance;

bool WebServerClientTest::Initialize(std::shared_ptr<uv::Loop> & loop) {

  //m_loop = std::make_shared(loop);
  m_loop = loop;
  m_loop->error.connect(
      [](uv::Error err) { wpi::errs() << "uv Error: " << err.str() << "\n"; });

  m_tcp_client = uv::Tcp::Create(m_loop);
  if (!m_tcp_client) {
    return false;
  }

  // Hook up TCP client events
  m_tcp_client->error.connect(
      [this, socket = m_tcp_client.get()](wpi::uv::Error err) {
        if (m_tcp_connected) {
          m_tcp_connected = false;
          m_connect_attempts = 0;
          m_loop->Stop();
          return;
        }

        // If we weren't previously connected, attempt a reconnection
        m_connect_timer->Start(uv::Timer::Time(kTcpConnectAttemptTimeout));
      });

  m_tcp_client->closed.connect(
      []() { wpi::errs() << "TCP connection closed\n"; });

  // Set up the connection timer
  m_connect_timer = uv::Timer::Create(m_loop);
  if (!m_connect_timer) {
    return false;
  }
  // Set up the timer to attempt connection
  m_connect_timer->timeout.connect([this] { AttemptConnect(); });

  wpi::errs() << "WebServerClientTest Initialized\n";
  wpi::errs() << "Will attempt to connect to: localhost:8080/wpilibws\n";

  return true;
}

void WebServerClientTest::StartTimer() {
  m_connect_timer->Start(uv::Timer::Time(0));
}

void WebServerClientTest::AttemptConnect() {
  m_connect_attempts++;
  wpi::errs() << "Connection Attempt " << m_connect_attempts << "\n";

  if(m_connect_attempts >= 10) {
    Exit();
    return;
  }

  struct sockaddr_in dest;
  uv::NameToAddr("localhost", 8080, &dest);
  m_tcp_client->Connect(dest, [this, socket = m_tcp_client.get()]() {
    m_tcp_connected = true;
    m_hws = std::make_shared<WebServerClientConnectionTest>(m_tcp_client);
    m_hws->Initialize("localhost", 8080, "/wpilibws");
  });
}

void WebServerClientTest::Exit() {
  auto inst = GetInstance();
  if (!inst) {
    return;
  }

  auto loop = inst->m_loop;
  loop->Walk([](uv::Handle& h) {
    h.SetLoopClosing(true);
    h.Close();
  });
}

void WebServerClientTest::SendMessage(const wpi::json& msg) {
  m_hws->SendMessage(msg);
}

void WebServerClientTest::OnNetValueChanged(const wpi::json& msg) {
 // Save the last message sent
 m_json = msg;
}

const wpi::json & WebServerClientTest::GetLastMessage() {
  return m_json;
}

}  // namespace wpilibws
