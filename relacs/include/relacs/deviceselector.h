/*
  deviceselector.h
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

#ifndef DEVICESELECTOR_H
#define DEVICESELECTOR_H

#include <map>
#include <set>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <relacs/relacsdevices.h>
#include <relacs/configclass.h>

namespace relacs {

/*!
\class DeviceSelector
\author Philipp Kuhlmann
\brief Dialog to allow configuration of available and active devices

All available devices are extracted through checking every loaded plugin for devices.
Active devices are loaded directly from the configuration file.

For adding/activating a new device and loading current active devices, a temporary device object
 is created to get information about all options. The temporary device is destroyed immediatly after its use.
*/
class DeviceSelector : public QWidget
{
  Q_OBJECT
public:
  /*! Dialog button control codes */
  enum Codes
  {
    CODE_APPLY = 1,
    CODE_OK = 2,
  };
private:
  /*! Internal data needed to manage available devices */
  struct AvailableData
  {
    int pluginIndex;  ///< plugin id for the device class
  };

  /*! Internal data needed to manage active devices */
  struct ActiveData
  {
    Options options;      ///< device configuration
    bool deviceAvailable; ///< device is currently available though a plugin
  };

  /*! Internal tree reprenentation to simplify modification */
  template<typename T>
  struct Tree
  {
    QTreeWidget* widget;

    struct Group
    {
      int type;                   ///< plugin id for TreeAvailable, device type for TreeActive
      QTreeWidgetItem* groupItem;

      struct Entry
      {
        QTreeWidgetItem* item;
        T data;
      };

      std::map<QTreeWidgetItem*, Entry> entries;
    };

    std::map<int, Group> groups;  ///< plugin id for TreeAvailable, device type for TreeActive
  };

public:
  /*! Creates a new device selector widget */
  DeviceSelector(std::map<int, ConfigClass*> deviceLists, QWidget* parent = nullptr);

private:
  /*! Initializes available devices */
  void initAvailable();
  /*! Adds a new device group */
  void addGroup(Tree<AvailableData>& tree, int type);
  /*! Adds a new device to a group */
  void addAvailableDevice(Tree<AvailableData>::Group& group, const std::string& pluginName, int pluginIndex);

  /*! Initializes active devices */
  void initActive();
  /*! Adds a new device group */
  void addGroup(Tree<ActiveData> &tree, int type);
  /*! Adds a new device to a group */
  void addActiveDevice(Tree<ActiveData>::Group& group, Device* device);
  /*! Adds a new device to a group, creates a temporary device instance */
  void addActiveDevice(Tree<ActiveData>::Group& group, int pluginIndex, Options* option);

  /*! Opens creation dialog, creates temporary device object */
  void openCreateDeviceDialog(int type, int pluginIndex, const std::string& pluginName);
  /*! Edits options of a certain device */
  void openEditDeviceDialog(Tree<ActiveData>::Group::Entry* entry);

private slots:
  /*! Slot for activate button */
  void activateDevice();
  /*! Slot for deactivate button */
  void deactivateDevice();
  /*! Slot for edit button */
  void editDevice();

public slots:
  /*!
     * \brief Dialog callback, handles saving
     * \see Codes
    */
  void dialogClosed( int code );

signals:
  /*! Signaled when settings were changed */
  void newDeviceSettings();

private:
  std::map<int, ConfigClass*> DeviceLists;  ///< (plugin id, device list) pairs
  Tree<AvailableData> TreeAvailable;        ///< all available devices
  Tree<ActiveData> TreeActive;              ///< all active devices
};

}

#endif // DEVICESELECTOR_H

