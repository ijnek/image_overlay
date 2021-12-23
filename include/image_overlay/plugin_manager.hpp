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

#ifndef IMAGE_OVERLAY__PLUGIN_MANAGER_HPP_
#define IMAGE_OVERLAY__PLUGIN_MANAGER_HPP_

#include <QAbstractTableModel>
#include <vector>
#include <string>
#include <memory>
#include "image_overlay/plugin.hpp"
#include "pluginlib/class_loader.hpp"

// forward declaration
namespace qt_gui_cpp {class Settings;}


// Refer to https://doc.qt.io/archives/4.6/model-view-model-subclassing.html
// for implementing methods of QAbstractTableModel

class PluginManager : public QAbstractTableModel
{
public:
  explicit PluginManager(const rclcpp::Node::SharedPtr & node, QObject * parent = nullptr);
  const std::vector<std::string> & getDeclaredClasses();
  bool addPlugin(std::string plugin_class);
  void overlay(QImage & image) const;
  void saveSettings(qt_gui_cpp::Settings & instance_settings) const;
  void restoreSettings(const qt_gui_cpp::Settings & settings);

protected:
  // Overriding QAbstractTableModel methods
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant headerData(
    int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const override;
  bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

private:
  pluginlib::ClassLoader<ImageOverlayPlugin> plugin_loader;
  const std::vector<std::string> declared_classes;
  const rclcpp::Node::SharedPtr & node_;

  std::vector<std::unique_ptr<Plugin>> plugins;
  const std::vector<std::string> columns;
};

#endif  // IMAGE_OVERLAY__PLUGIN_MANAGER_HPP_
