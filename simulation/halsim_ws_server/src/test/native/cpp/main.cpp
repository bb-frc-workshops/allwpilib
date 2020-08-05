/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include <hal/Main.h>
#include <hal/HALBase.h>
#include <hal/simulation/DIOData.h>
#include "HALSimWSServer.h"
#include "WebServerClientTest.h"
#include <wpi/uv/Loop.h>
#include <wpi/raw_ostream.h>

using namespace wpilibws;

extern "C" int HALSIM_InitExtension(void); // in simulation/halsim_ws_server/src/main/native/cpp/main.cpp

class WebServerIntegrationTest : public ::testing::Test {
public:
  WebServerIntegrationTest() {
    // Needed to initialize providers and webserver loop
    HALSIM_InitExtension();

    m_webserver_client = std::shared_ptr<WebServerClientTest>((new WebServerClientTest()));
    WebServerClientTest::SetInstance(m_webserver_client);
    auto webserver = HALSimWeb::GetInstance();
    auto m_loop = webserver->GetLoop();
    // Create total time for test to run
    m_testTimer = wpi::uv::Timer::Create(m_loop);
    m_testTimer->timeout.connect([m_loop] {
      m_loop->Stop();
    });
    m_webserver_client->Initialize(m_loop);
    m_webserver_client->StartTimer();
    m_testTimer->Start(uv::Timer::Time{5000}); // End test after 5 seconds 
  }

  ~WebServerIntegrationTest() {
    HAL_ExitMain();
    m_webserver_client->Exit();
    HALSimWeb::Exit(nullptr);
    WebServerClientTest::SetInstance(nullptr);
    m_testTimer->Unreference();
  }

private:
  std::shared_ptr<wpi::uv::Loop> m_loop;
  std::shared_ptr<WebServerClientTest> m_webserver_client;
  std::shared_ptr<wpi::uv::Timer> m_testTimer;
};

TEST_F(WebServerIntegrationTest, DigitalOutput) {

  const bool EXPECTED_VALUE = false;
  const int CHANNEL = 0;
  auto ws = HALSimWeb::GetInstance();
  auto loop = ws->GetLoop();
  auto timer =  wpi::uv::Timer::Create(loop);
  timer->timeout.connect([EXPECTED_VALUE, CHANNEL] { 
    wpi::outs() << "************ Setting DIO value ***********\n";
    HALSIM_SetDIOValue(CHANNEL, EXPECTED_VALUE);
  });

  // Wait 3 seconds before sending change in value
  timer->Start(uv::Timer::Time(3000));
  HAL_RunMain();

  std::string test_type;
  std::string test_device;
  bool test_value = true; //Default from initialization
  try {
    auto & msg = WebServerClientTest::GetInstance()->GetLastMessage();
    test_type = msg.at("type").get_ref<const std::string&>();
    test_device = msg.at("device").get_ref<const std::string&>();
    auto & data = msg.at("data");
    wpi::json::const_iterator it = data.find("<>value");
    if ( it != data.end()) {
      std::cout << " <>value " << static_cast<bool>(it.value()) << std::endl;
      test_value = it.value();
    }
  } catch (wpi::json::exception& e) {
    //wpi::errs() << "Error with incoming message: " << e.what() << "\n";
    std::cerr << "Error with incoming message: " << e.what() << std::endl;
  }
  timer->Unreference();

  EXPECT_EQ("DIO",test_type);
  EXPECT_EQ(std::to_string(CHANNEL), test_device);
  EXPECT_EQ(EXPECTED_VALUE,test_value);
}

TEST_F(WebServerIntegrationTest, DigitalInput) {

  const bool EXPECTED_VALUE = false;
  const int CHANNEL = 0;
  auto ws = HALSimWeb::GetInstance();
  auto loop = ws->GetLoop();
  auto timer =  wpi::uv::Timer::Create(loop);
  timer->timeout.connect([EXPECTED_VALUE, CHANNEL] {      
    wpi::json msg = {
      {"type", "DIO"}, {"device", std::to_string(CHANNEL)}, {"data", {{"<>value", EXPECTED_VALUE}}}
    };
    wpi::outs() << "Input JSON is " << msg.dump() << "\n";
    WebServerClientTest::GetInstance()->SendMessage(msg);
  });

  // Wait 3 seconds before sending change in value
  timer->Start(uv::Timer::Time(3000));
  HAL_RunMain();
  timer->Unreference();
  bool test_value = HALSIM_GetDIOValue(CHANNEL);
  EXPECT_EQ(EXPECTED_VALUE,test_value);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  
  HAL_Initialize(500, 0);
  int ret = RUN_ALL_TESTS();
  return ret;
}
