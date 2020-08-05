/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/WebSocket.h>
#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/uv/AsyncFunction.h>
#include <wpi/uv/Buffer.h>
#include <wpi/uv/Stream.h>

namespace wpilibws {

class WebServerClientTest;

class WebServerClientConnectionTest :
      public std::enable_shared_from_this<WebServerClientConnectionTest> {
 public:
  using BufferPool = wpi::uv::SimpleBufferPool<4>;
  using LoopFunc = std::function<void(void)>;
  using UvExecFunc = wpi::uv::AsyncFunction<void(LoopFunc)>;

  explicit WebServerClientConnectionTest(std::shared_ptr<wpi::uv::Stream> stream)
      : m_stream(stream) {}

 public:
  void SendMessage(const wpi::json& msg);
  void Initialize(const std::string & host, int port, const std::string & uri);

 private:
  std::shared_ptr<wpi::uv::Stream> m_stream;

  bool m_ws_connected = false;
  wpi::WebSocket* m_websocket = nullptr;

  std::shared_ptr<UvExecFunc> m_exec;
  std::unique_ptr<BufferPool> m_buffers;
  std::mutex m_buffers_mutex;
};

}  // namespace wpilibws
