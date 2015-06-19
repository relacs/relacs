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
    int pluginIndex;
  };

  /*! Internal data needed to manage active devices */
  struct ActiveData
  {
    Options options;
    bool deviceAvailable;
  };

  /*! Internal tree reprenentation to simplify modification */
  template<typename T>
  struct Tree
  {
    QTreeWidget* widget;

    struct Group
    {
      int type;
      QTreeWidgetItem* groupItem;

      struct Entry
      {
        QTreeWidgetItem* item;
        T data;
      };

      std::map<QTreeWidgetItem*, Entry> entries;
    };

    std::map<int, Group> groups;
  };

public:
  /*! Creates a new device selector widget */
  DeviceSelector(AllDevices* adv, std::map<int, ConfigClass*> deviceLists, QWidget* parent = nullptr);

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
  AllDevices* ADV;
  std::map<int, ConfigClass*> DeviceLists;
  Tree<AvailableData> TreeAvailable;
  Tree<ActiveData> TreeActive;
};

}

#endif // DEVICESELECTOR_H

