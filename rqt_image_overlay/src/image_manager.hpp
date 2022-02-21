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

#ifndef IMAGE_MANAGER_HPP_
#define IMAGE_MANAGER_HPP_

#include <QAbstractListModel>
#include <QImage>
#include <memory>
#include <string>
#include <vector>
#include "image_transport/subscriber.hpp"
#include "image_topic.hpp"

namespace rclcpp {class Node;}
namespace qt_gui_cpp {class Settings;}

namespace rqt_image_overlay
{

class ImageManager : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit ImageManager(const std::shared_ptr<rclcpp::Node> & node);
  std::shared_ptr<QImage> getImage() const;
  void saveSettings(qt_gui_cpp::Settings & settings, int currentIndex) const;

  // return whether a topic and transport was found from settings, and added to the model at index 1
  bool restoreSettings(const qt_gui_cpp::Settings & settings);

protected:
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

public slots:
  void onTopicChanged(int index);
  void updateImageTopicList();

private:
  void callbackImage(const sensor_msgs::msg::Image::ConstSharedPtr & msg);

  void addImageTopicExplicitly(ImageTopic topic);

  image_transport::Subscriber subscriber;
  const std::shared_ptr<rclcpp::Node> & node;
  sensor_msgs::msg::Image::ConstSharedPtr lastMsg;

  std::vector<ImageTopic> topics;
};

}  // namespace rqt_image_overlay

#endif  // IMAGE_MANAGER_HPP_
