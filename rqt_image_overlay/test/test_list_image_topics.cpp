// Copyright 2021 Kenji Brameld
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <memory>
#include "../src/list_image_topics.hpp"
#include "rclcpp/node.hpp"
#include "rclcpp/rclcpp.hpp"
#include "image_transport/image_transport.hpp"
#include "theora_image_transport/msg/packet.hpp"
#include "std_msgs/msg/string.hpp"

// In this test, there's two types of transports (ie. "raw" and "theora") available.
// The "theora" transport is declared as a test dependency so we can write tests for
// image_transport plugins.

class TestListImageTopics : public ::testing::Test
{
protected:
  static void SetUpTestCase()
  {
    rclcpp::init(0, nullptr);
  }

  static void TearDownTestCase()
  {
    rclcpp::shutdown();
  }
};

TEST_F(TestListImageTopics, TestNone)
{
  auto node = std::make_shared<rclcpp::Node>("test_node");

  // Give a chance for the topic to be picked up
  rclcpp::sleep_for(std::chrono::milliseconds(10));
  rclcpp::spin_some(node);

  auto topics = rqt_image_overlay::ListImageTopics(*node);
  EXPECT_EQ(topics.size(), 0u);
}

TEST_F(TestListImageTopics, TestOne)
{
  auto node = std::make_shared<rclcpp::Node>("test_node");

  auto publisher = image_transport::create_publisher(node.get(), "/test_topic");

  // Give a chance for the topic to be picked up
  rclcpp::sleep_for(std::chrono::milliseconds(10));
  rclcpp::spin_some(node);

  auto topics = rqt_image_overlay::ListImageTopics(*node);
  ASSERT_EQ(topics.size(), 2u);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_topic", "raw"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_topic", "theora"}), 1);
}

TEST_F(TestListImageTopics, TestThree)
{
  auto node = std::make_shared<rclcpp::Node>("test_node");

  auto publisher1 = image_transport::create_publisher(node.get(), "/test_ns1/test_topic1");
  auto publisher2 = image_transport::create_publisher(node.get(), "/test_ns2/test_topic2");
  auto publisher3 = image_transport::create_publisher(node.get(), "/test_ns3/test_topic3");

  // Give a chance for the topic to be picked up
  rclcpp::sleep_for(std::chrono::milliseconds(10));
  rclcpp::spin_some(node);

  auto topics = rqt_image_overlay::ListImageTopics(*node);
  ASSERT_EQ(topics.size(), 6u);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns1/test_topic1", "raw"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns1/test_topic1", "theora"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns2/test_topic2", "raw"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns2/test_topic2", "theora"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns3/test_topic3", "raw"}), 1);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_ns3/test_topic3", "theora"}), 1);
}

TEST_F(TestListImageTopics, TestOnlyTheoraTransport)
{
  auto node = std::make_shared<rclcpp::Node>("test_node");

  auto publisher =
    node->create_publisher<theora_image_transport::msg::Packet>("/test_topic/theora", 1);

  // Give a chance for the topic to be picked up
  rclcpp::sleep_for(std::chrono::milliseconds(10));
  rclcpp::spin_some(node);

  auto topics = rqt_image_overlay::ListImageTopics(*node);
  ASSERT_EQ(topics.size(), 1u);
  EXPECT_EQ(
    std::count(
      topics.begin(), topics.end(),
      rqt_image_overlay::ImageTopic{"/test_topic", "theora"}), 1);
}

TEST_F(TestListImageTopics, TestFakeTheoraTransport)
{
  // In this example, we test a case where the topic name ends with a transport type, but the msg
  // type is not theora_image_transport::msg::Packet.
  auto node = std::make_shared<rclcpp::Node>("test_node");

  auto publisher =
    node->create_publisher<std_msgs::msg::String>("/test_topic/theora", 1);

  // Give a chance for the topic to be picked up
  rclcpp::sleep_for(std::chrono::milliseconds(10));
  rclcpp::spin_some(node);

  auto topics = rqt_image_overlay::ListImageTopics(*node);
  ASSERT_TRUE(topics.empty());
}
