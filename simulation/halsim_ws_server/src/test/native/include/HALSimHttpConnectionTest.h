/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimHttpConnection.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Tcp.h"
#include "gtest/gtest.h"

namespace wpilibws {

class HALSimHttpConnectionTest : public ::testing::test {
  public:
    HALSimHttpConnectionTest() {
      m_loop = wpi::uv::Loop::Create();
      m_server = wpi::uv::Tcp::Create(m_loop);
      m_server->Bind("", 8080);
    }
    ~HALSimHttpConnectionTest() {}
  protected:
    std::shared_ptr<HALSimBaseWebSocketConnection> m_hws;
    std::shared_ptr<wpi::uv::Tcp> m_server;
    std::shared_ptr<wpi::uv::Loop> m_loop;
};

} // namespace wpilibws