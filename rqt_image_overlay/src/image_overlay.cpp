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

#include <functional>
#include <string>
#include "./image_overlay.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace rqt_image_overlay
{

ImageOverlay::ImageOverlay()
: rqt_gui_cpp::Plugin(), imageManager(node_), overlayManager(node_),
  compositor(imageManager, overlayManager, 30.0)
{
}

void ImageOverlay::initPlugin(qt_gui_cpp::PluginContext & context)
{
  QWidget * widget = new QWidget();
  ui.setupUi(widget);
  context.addWidget(widget);

  ui.overlay_table->setModel(&overlayManager);
  ui.image_topics_combo_box->setModel(&imageManager);

  ui.overlay_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  fillOverlayMenu();

  ui.image_topics_combo_box->setCurrentIndex(ui.image_topics_combo_box->findText(""));
  connect(
    ui.image_topics_combo_box, SIGNAL(currentTextChanged(QString)), &imageManager,
    SLOT(onTopicChanged(QString)));

  connect(
    ui.refresh_image_topics_button, SIGNAL(pressed()), &imageManager,
    SLOT(updateImageTopicList()));

  connect(ui.remove_overlay_button, SIGNAL(pressed()), this, SLOT(removeOverlay()));

  compositor.setCallableSetImage(
    std::bind(
      &CompositionFrame::setImage, ui.image_frame,
      std::placeholders::_1));
}

void ImageOverlay::addOverlay(QString plugin_class)
{
  overlayManager.addOverlay(plugin_class.toStdString());
}

void ImageOverlay::removeOverlay()
{
  QItemSelectionModel * select = ui.overlay_table->selectionModel();
  for (auto const & index : select->selectedRows()) {
    overlayManager.removeOverlay(index.row());
  }
}


void ImageOverlay::saveSettings(
  qt_gui_cpp::Settings &,
  qt_gui_cpp::Settings & instance_settings) const
{
  instance_settings.setValue("image_topic", ui.image_topics_combo_box->currentText());
  overlayManager.saveSettings(instance_settings);
}

void ImageOverlay::restoreSettings(
  const qt_gui_cpp::Settings &,
  const qt_gui_cpp::Settings & instance_settings)
{
  if (instance_settings.contains("image_topic")) {
    QString topic = instance_settings.value("image_topic").toString();
    if (topic != "") {
      imageManager.setTopicExplicitly(topic);
      ui.image_topics_combo_box->setCurrentIndex(1);
    }
  }

  overlayManager.restoreSettings(instance_settings);
}

void ImageOverlay::fillOverlayMenu()
{
  menu.clear();

  for (const std::string & str_plugin_class : overlayManager.getDeclaredPluginClasses()) {
    QString qstr_plugin_class = QString::fromStdString(str_plugin_class);
    QAction * action = new QAction(qstr_plugin_class, this);
    menu.addAction(action);  // ownership transferred
    connect(action, SIGNAL(triggered()), &signalMapper, SLOT(map()));
    signalMapper.setMapping(action, qstr_plugin_class);
  }

  connect(&signalMapper, SIGNAL(mapped(QString)), this, SLOT(addOverlay(QString)));

  ui.add_overlay_button->setMenu(&menu);
}


}  // namespace rqt_image_overlay


PLUGINLIB_EXPORT_CLASS(rqt_image_overlay::ImageOverlay, rqt_gui_cpp::Plugin)
