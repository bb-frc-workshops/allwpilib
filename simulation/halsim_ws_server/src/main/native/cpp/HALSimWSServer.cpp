/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWSServer.h"

#include <wpi/FileSystem.h>
#include <wpi/Path.h>
#include <wpi/Twine.h>
#include <wpi/UrlParser.h>
#include <wpi/WebSocketServer.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>

#include "HALSimHttpConnection.h"

namespace uv = wpi::uv;

namespace wpilibws {

std::shared_ptr<HALSimWeb> HALSimWeb::g_instance;

bool HALSimWeb::Initialize() {
  // determine where to get static content from
  wpi::SmallVector<char, 64> tmp;

  const char* webroot_sys = std::getenv("HALSIMWEB_SYSROOT");
  if (webroot_sys != NULL) {
    wpi::StringRef tstr(webroot_sys);
    tmp.append(tstr.begin(), tstr.end());
  } else {
    wpi::sys::fs::current_path(tmp);
    wpi::sys::path::append(tmp, "sim");
  }
  wpi::sys::fs::make_absolute(tmp);
  m_webroot_sys = wpi::Twine(tmp).str();

  tmp.clear();
  const char* webroot_user = std::getenv("HALSIMWEB_USERROOT");
  if (webroot_user != NULL) {
    wpi::StringRef tstr(webroot_user);
    tmp.append(tstr.begin(), tstr.end());
  } else {
    wpi::sys::fs::current_path(tmp);
    wpi::sys::path::append(tmp, "sim", "user");
  }
  wpi::sys::fs::make_absolute(tmp);
  m_webroot_user = wpi::Twine(tmp).str();

  // create libuv things
  m_loop = uv::Loop::Create();
  if (!m_loop) {
    return false;
  }

  m_loop->error.connect(
      [](uv::Error err) { wpi::errs() << "uv ERROR: " << err.str() << '\n'; });

  m_server = uv::Tcp::Create(m_loop);
  if (!m_server) {
    return false;
  }

  // TODO: configurable port
  m_server->Bind("", 8080);
  return true;
}

void HALSimWeb::Main(void* param) {
  GetInstance()->MainLoop();
  SetInstance(nullptr);
}

void HALSimWeb::MainLoop() {
  // when we get a connection, accept it and start reading
  m_server->connection.connect([this, srv = m_server.get()] {
    auto tcp = srv->Accept();
    if (!tcp) return;

    auto conn = std::make_shared<HALSimHttpConnection>(tcp, m_webroot_sys,
                                                       m_webroot_user);
  });

  // start listening for incoming connections
  m_server->Listen();
  wpi::errs() << "Listening at http://localhost:8080\n";

  m_loop->Run();
}

void HALSimWeb::Exit(void* param) {
  auto inst = GetInstance();
  if (!inst) return;

  auto loop = inst->m_loop;
  loop->Walk([](uv::Handle& h) {
    h.SetLoopClosing(true);
    h.Close();
  });
}

bool HALSimWeb::RegisterWebsocket(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (m_hws.lock()) {
    return false;
  }

  m_hws = hws;

  // notify all providers that they should use this new websocket instead
  m_providers.ForEach([hws](std::shared_ptr<HALSimWSBaseProvider> provider) {
    provider->OnNetworkConnected(hws);
  });

  return true;
}

void HALSimWeb::CloseWebsocket(
    std::shared_ptr<HALSimBaseWebSocketConnection> hws) {
  if (hws == m_hws.lock()) {
    m_hws.reset();
  }
}

void HALSimWeb::OnNetValueChanged(const wpi::json& msg) {
  // Look for "type" and "device" fields so that we can
  // generate the key

  try {
    std::string type = msg.at("type").get<std::string>();
    std::string device = msg.at("device").get<std::string>();
    auto data = msg.at("data");

    auto key = type + "/" + device;
    auto provider = m_providers.Get(key);
    if (provider) {
      provider->OnNetValueChanged(data);
    }
  }
  catch (wpi::json::exception& e) {
    wpi::errs() << "Error with incoming message: " << e.what() << "\n";
  }
}

}  // namespace wpilibws
