/*
  macroeditor.h
  Dialog to allow configuration of macros

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

#include <map>
#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <relacs/macros.h>

namespace relacs {

class MacroEditor;

namespace MacroGUI
{
  class MacroParameter
  {
  public:
    std::string name;
    std::string value;
    std::string unit;
  };

  class MacroCommandInfo
  {
  public:
    enum class Type
    {
      FILTER, DETECTOR, MESSAGE, BROWSE, SHELL, SWITCH, START_SESSION, REPRO, MACRO
    };

  public:
    Type type;
    bool deactivated;
  };

  class MacroInfo
  {
  public:
    enum class Keyword
    {
      STARTUP, SHUTDOWN,STARTSESSION,STOPSESSION,FALLBACK, NOKEY, NOBUTTON, NOMENU, KEEP, OVERWRITE
    };

  public:
    std::string name;
    std::set<Keyword> keywords;
    std::vector<MacroParameter> parameter;
    std::vector<MacroCommandInfo*> commands;

  public:
    void createGUI(MacroEditor* parent);
    QTreeWidgetItem* treeItem() const { return gui.treeItem; }

  private:
    MacroEditor* parent;
    struct
    {
      QTreeWidgetItem* treeItem;
      QWidget* detailView;
    } gui;
  };

  class MacroFile
  {
  public:
    std::string name;
    std::vector<MacroInfo> macros;

  public:
    void createGUI(MacroEditor* parent);
    QTreeWidgetItem* treeItem() const { return gui.treeItem; }

  private:
    struct
    {
      QTreeWidgetItem* treeItem;
    } gui;
  };

}


class MacroEditor : public QWidget
{
  Q_OBJECT

public:
  MacroEditor(Macros* macros, QWidget* parent = nullptr);

public slots:
  void dialogClosed( int code );

signals:
  void macroDefinitionsChanged();

private slots:
  void currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);

private:
  void populate(const std::vector<MacroGUI::MacroFile>& macrofiles);
public:
  int addDetailView(QWidget* view, QTreeWidgetItem* treeItem);

private:
  Macros* InternalMacros;

  std::vector<MacroGUI::MacroFile> MacroFiles;
  QTreeWidget* MacroTree;
  QStackedWidget* DetailViewContainer;
  std::map<QTreeWidgetItem*, int> TreeToDetailMap;
};

}
