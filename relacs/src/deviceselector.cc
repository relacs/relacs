/*
  deviceselector.cc
  Dialog to allow configuration of available and active devices

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <relacs/deviceselector.h>

#include <memory>
#include <iostream>
#include <QPushButton>
#include <QList>
#include <QMessageBox>
#include <relacs/plugins.h>
#include <relacs/relacsplugin.h>
#include <relacs/relacsdevices.h>

namespace relacs {

namespace {

/*! List of currently supported options
 * can be easily extended with further mappings, do not forget to pass the concrete DeviceList to the constructor
 */
static const struct { const char* name; int deviceType; int pluginType; } GROUPS[] =
{
  { "Analog Input",   Device::DeviceTypes::AnalogInputType,   RELACSPlugin::AnalogInputId },
  { "Analog Output",  Device::DeviceTypes::AnalogOutputType,  RELACSPlugin::AnalogOutputId },
  { "Digital I/O",    Device::DeviceTypes::DigitalIOType,     RELACSPlugin::DigitalIOId },
  { "Trigger",        Device::DeviceTypes::TriggerType,       RELACSPlugin::TriggerId },
  { "Attenuator",     Device::DeviceTypes::AttenuatorType,    RELACSPlugin::AttenuatorId },
  { "Attenuator Interface", Device::DeviceTypes::AttenuateType, RELACSPlugin::AttenuateId },
  { "Miscellaneous",  Device::DeviceTypes::MiscellaneousType, RELACSPlugin::DeviceId },
};
/*! Number of different groups */
static const int GROUP_COUNT = sizeof(GROUPS)/sizeof(decltype(GROUPS[0]));

/*! converts a device type to the corresponding plugin id */
int convertDeviceTypeToPluginId(int device)
{
  for (int i = 0; i < GROUP_COUNT; ++i)
    if (GROUPS[i].deviceType == device)
      return GROUPS[i].pluginType;

  return -1;
}

/*! converts a plugin id to the corresponding device type */
int convertPluginIdToDeviceType(int plugin)
{
  for (int i = 0; i < GROUP_COUNT; ++i)
    if (GROUPS[i].pluginType == plugin)
      return GROUPS[i].deviceType;

  return -1;
}

}

DeviceSelector::DeviceSelector(std::map<int, ConfigClass*> deviceLists, QWidget *parent) :
  QWidget(parent),
  DeviceLists(deviceLists)
{
  QHBoxLayout* layout = new QHBoxLayout();

  TreeAvailable.widget = new QTreeWidget(this);
  TreeAvailable.widget->setMinimumSize(500, 300);
  initAvailable();

  TreeActive.widget = new QTreeWidget(this);
  TreeActive.widget->setMinimumSize(500, 300);
  initActive();

  QVBoxLayout* buttonLayout = new QVBoxLayout();
  QPushButton* button = new QPushButton("->");
  connect(button, SIGNAL(clicked()), this, SLOT(activateDevice()));
  buttonLayout->addWidget(button);

  button = new QPushButton("Edit");
  connect(button, SIGNAL(clicked()), this, SLOT(editDevice()));
  buttonLayout->addWidget(button);

  button = new QPushButton("<-");
  connect(button, SIGNAL(clicked()), this, SLOT(deactivateDevice()));
  buttonLayout->addWidget(button);

  layout->addWidget(TreeAvailable.widget);
  layout->addLayout(buttonLayout);
  layout->addWidget(TreeActive.widget);

  setLayout(layout);
}

void DeviceSelector::dialogClosed( int code )
{
  if (code < CODE_APPLY)
  {
    delete this;
    return;
  }

  for (auto itr = TreeActive.groups.begin(); itr != TreeActive.groups.end(); ++itr)
  {
    int idx = convertDeviceTypeToPluginId(itr->second.type);
    if (idx < 0)
      continue;

    auto find = DeviceLists.find(idx);
    if (find == DeviceLists.end())
      continue;

    ConfigClass* device = find->second;
    device->clear();

    int deviceCounter = 1;
    for (auto items = itr->second.entries.begin(); items != itr->second.entries.end(); ++items)
    {
      Options& section = device->newSection("Device" + Str(deviceCounter));
      section.append(items->second.data.options);

      ++deviceCounter;
    }
  }

  emit newDeviceSettings();

  if (code != CODE_APPLY)
    delete this;
}

void DeviceSelector::activateDevice()
{
  /// single selection only
  QList<QTreeWidgetItem*> selection = TreeAvailable.widget->selectedItems();
  if (selection.empty())
    return;

  QTreeWidgetItem* item = selection[0];

  Tree<AvailableData>::Group* group = nullptr;
  Tree<AvailableData>::Group::Entry* entry = nullptr;
  for (auto itr = TreeAvailable.groups.begin(); itr != TreeAvailable.groups.end(); ++itr)
  {
    auto find = itr->second.entries.find(item);
    if (find != itr->second.entries.end())
    {
      entry = &(find->second);
      group = &(itr->second);
      break;
    }
  }

  if (!group || !entry)
    return;

  openCreateDeviceDialog(group->type, entry->data.pluginIndex, entry->item->text(0).toStdString());
}

void DeviceSelector::deactivateDevice()
{
  /// single selection only
  QList<QTreeWidgetItem*> selection = TreeActive.widget->selectedItems();
  if (selection.empty())
    return;

  QTreeWidgetItem* item = selection[0];
  Tree<ActiveData>::Group* data = NULL;
  for (auto itr = TreeActive.groups.begin(); itr != TreeActive.groups.end(); ++itr)
    if (itr->second.entries.find(item) != itr->second.entries.end())
    {
      data = &(itr->second);
      break;
    }

  if (!data)
    return;

  /// confirmation box
  {
    QMessageBox msgBox;
    msgBox.setText("Are you sure?");
    msgBox.setInformativeText("Removing: " + item->text(0) + " " + item->text(1));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret)
    {
      case QMessageBox::No:
      case QMessageBox::Cancel:
      default:
        return;
      case QMessageBox::Yes:
        break;  // nop
    }
  }

  data->groupItem->removeChild(item);
  data->entries.erase(item);
}

void DeviceSelector::editDevice()
{
  //! single selection only
  QList<QTreeWidgetItem*> selection = TreeActive.widget->selectedItems();
  if (selection.empty())
    return;

  QTreeWidgetItem* item = selection[0];
  Tree<ActiveData>::Group::Entry* device = nullptr;

  for (auto&& data : TreeActive.groups)
    for (auto&& pair : data.second.entries)
      if (pair.first == item)
      {
        device = &pair.second;
        break;
      }

  if (!device)
    return;

  openEditDeviceDialog(device);
}


void DeviceSelector::addGroup(Tree<AvailableData>& tree, int idx)
{
  Tree<AvailableData>::Group group;
  group.type = GROUPS[idx].pluginType;
  group.groupItem = new QTreeWidgetItem(tree.widget);
  group.groupItem->setText(0, GROUPS[idx].name);

  tree.groups.insert(std::make_pair(group.type, group));
}

void DeviceSelector::addAvailableDevice(Tree<AvailableData>::Group& group, const string &pluginName, int pluginIndex)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(group.groupItem);

  std::string name = pluginName.substr(0, pluginName.find_first_of("["));
  std::string plugin = pluginName.substr(name.length() + 1, pluginName.length() - (name.length() + 1) - 1);

  item->setText(0, name.c_str());
  item->setText(1, plugin.c_str());

  group.entries.insert(std::make_pair(item, Tree<AvailableData>::Group::Entry{ item, AvailableData{ pluginIndex }}));
}

void DeviceSelector::initAvailable()
{
  QStringList header;
  header << "Device" << "from Plugin";
  TreeAvailable.widget->setColumnCount(header.count());
  TreeAvailable.widget->setHeaderLabels(header);

  for (int i = 0; i < GROUP_COUNT; ++i)
    addGroup(TreeAvailable, i);

  for (auto itr = TreeAvailable.groups.begin(); itr != TreeAvailable.groups.end(); ++itr)
  {
    for (int i = 0; i < Plugins::plugins(); ++i)
      if (Plugins::type(i) & itr->first)
        addAvailableDevice(itr->second, Plugins::ident(i), i);

    TreeAvailable.widget->expandItem(itr->second.groupItem);
  }

  for (int i = 0; i < TreeAvailable.widget->columnCount(); ++i)
    TreeAvailable.widget->resizeColumnToContents(i);
}

void DeviceSelector::addGroup(Tree<ActiveData>& tree, int idx)
{
  Tree<ActiveData>::Group group;
  group.type = GROUPS[idx].deviceType;
  group.groupItem = new QTreeWidgetItem(tree.widget);
  group.groupItem->setText(0, GROUPS[idx].name);

  tree.groups.insert(std::make_pair(group.type, group));
}

void DeviceSelector::addActiveDevice(Tree<ActiveData>::Group& group, Device *device)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(group.groupItem);

  item->setText(0, device->text("plugin").c_str());
  item->setText(1, device->text("ident").c_str());
  item->setText(2, device->text("device").c_str());

  group.entries.insert(std::make_pair(item, Tree<ActiveData>::Group::Entry{ item, ActiveData{ *device, true }}));
}

void DeviceSelector::addActiveDevice(Tree<ActiveData>::Group& group, int pluginType, Options* option)
{
  int index = Plugins::index(option->text("plugin"), pluginType);
  if (index == -Plugins::InvalidPlugin)
  {
    /// fallback case if device is not available
    QTreeWidgetItem* item = new QTreeWidgetItem(group.groupItem);

    item->setText(0, option->text("plugin").c_str());
    item->setText(1, option->text("ident").c_str());
    item->setText(2, option->text("device").c_str());

    group.entries.insert(std::make_pair(item, Tree<ActiveData>::Group::Entry{ item, ActiveData{ *option, false }}));

    return;
  }

  Device* device = static_cast<Device*>(Plugins::create(index));
  device->read(*option);

  addActiveDevice(group, device);

  delete device;
  Plugins::destroy(pluginType);
}

void DeviceSelector::initActive()
{
  QStringList header;
  header << "Plugin" << "Ident" << "Device";
  TreeActive.widget->setColumnCount(header.count());
  TreeActive.widget->setHeaderLabels(header);

  for (int i = 0; i < GROUP_COUNT; ++i)
    addGroup(TreeActive, i);

  for (int i = 0; i < GROUP_COUNT; ++i)
  {
    int pluginType = GROUPS[i].pluginType;

    auto find = DeviceLists.find(pluginType);
    if (find == DeviceLists.end())
      continue;

    auto group = TreeActive.groups.find(convertPluginIdToDeviceType(pluginType));
    if (group == TreeActive.groups.end())
      continue;

    Options* options = find->second;

    if (options->sectionsSize() > 0)
    {
      for (auto itr = options->sectionsBegin(); itr != options->sectionsEnd(); ++itr)
        addActiveDevice(group->second, pluginType, *itr);
    }
    else if (options->size() > 0)
    {
      addActiveDevice(group->second, pluginType, options);
    }

    TreeActive.widget->expandItem(group->second.groupItem);
  }

  for (int i = 0; i < TreeActive.widget->columnCount(); ++i)
    TreeActive.widget->resizeColumnToContents(i);
}

void DeviceSelector::openEditDeviceDialog(Tree<ActiveData>::Group::Entry* entry)
{
  do
  {
    OptDialog* od = new OptDialog(true, this);
    od->setCaption("Device options");
    od->addOptions(entry->data.options);
    od->addButton( "&Ok", OptDialog::Accept, DeviceSelector::CODE_OK );
    od->addButton( "&Cancel" );
    int code = od->exec();
    if (code != DeviceSelector::CODE_OK)
      return;

  } while (!checkConflicts(&entry->data.options));

  entry->item->setText(0, entry->data.options.text("plugin").c_str());
  entry->item->setText(1, entry->data.options.text("ident").c_str());
  entry->item->setText(2, entry->data.options.text("device").c_str());
}

void DeviceSelector::openCreateDeviceDialog(int type, int pluginIndex, const std::string& pluginName)
{
  Device* device = static_cast<Device*>(Plugins::create(pluginIndex));

  device->setText("plugin", pluginName);

  do
  {
    OptDialog* dia = new OptDialog(true, this);
    dia->setCaption("Create device");
    dia->addOptions(*device);
    dia->addButton( "&Ok", OptDialog::Accept, DeviceSelector::CODE_OK );
    dia->addButton( "&Cancel" );
    int code = dia->exec();
    if (code != DeviceSelector::CODE_OK)
    {
      delete device;
      Plugins::destroy(pluginIndex);
      return;
    }
  } while (!checkConflicts(device));

  addActiveDevice(TreeActive.groups[convertPluginIdToDeviceType(type)], device);

  delete device;
  Plugins::destroy(pluginIndex);
}

bool DeviceSelector::checkConflicts(Options* options)
{
  /// ident must be unique
  std::set<std::string> idents;
  for (auto&& group : TreeActive.groups)
    for (auto&& entry : group.second.entries)
      if (&entry.second.data.options != options)
        idents.insert(entry.second.data.options.text("ident"));

  bool fail = false;
  if (idents.find(options->text("ident")) != idents.end())
  {
    fail |= true;

    QMessageBox::warning(this,
                         "Duplicated Ident",
                         QString::fromStdString("This ident is already in use: " + options->text("ident")),
                         QMessageBox::StandardButton::Ok);
  }

  return !fail;
}

}

#include "moc_deviceselector.cc"
