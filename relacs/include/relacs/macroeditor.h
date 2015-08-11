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
#include <QCheckBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <relacs/macros.h>

namespace relacs {

class MacroEditor;

namespace MacroGUI
{
  template<typename T>
  class GUIElement
  {
  public:
    virtual void createGUI(T*) = 0;

  protected:
    bool GuiCreated = false;
    T* Owner = nullptr;
  };

  template<typename T>
  class TreeElement : public virtual GUIElement<T>
  {
  public:
    QTreeWidgetItem* treeItem() const { return TreeItem; }

  protected:
    QTreeWidgetItem* TreeItem = nullptr;
  };

  template<typename T>
  class DetailElement : public virtual GUIElement<T>
  {
  public:
    QWidget* detailView() const { return DetailView; }

  protected:
    QWidget* DetailView = nullptr;
  };


  class MacroInfo;


  class MacroParameter : public QObject, public DetailElement<MacroInfo>
  {
    Q_OBJECT

  public:
    void setName(const std::string& name);
    void setValue(const std::string& value);
    void setUnit(const std::string& unit);

  private slots:
    void updatedName(const QString& name);
    void updatedValue(const QString& value);
    void updatedUnit(const QString& unit);

  public:
    void createGUI(MacroInfo*) override;

    QListWidgetItem* listItem() const { return ListItem; }

  private:
    std::string Name;
    std::string Value;
    std::string Unit;

    QListWidgetItem* ListItem;
    QLineEdit* NameEdit;
    QLineEdit* ValueEdit;
    QLineEdit* UnitEdit;
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

  class MacroInfo : public QObject, public TreeElement<MacroEditor>, public DetailElement<MacroEditor>
  {
    Q_OBJECT
  public:
    enum class Keyword
    {
      STARTUP, SHUTDOWN,STARTSESSION,STOPSESSION,FALLBACK, NOKEY, NOBUTTON, NOMENU, KEEP, OVERWRITE
    };

  public:
    void setName(const std::string& name);
    void addParameter(MacroParameter* param);
    void removeParameter(MacroParameter* param);

    void createGUI(MacroEditor* parent) override;

  private slots:
    void updatedName(const QString& name);
    void updatedKeywords(int);
    void addParameter();
    void removeParameter();

  private:
    std::string Name;
    std::set<Keyword> Keywords;
    std::vector<MacroParameter*> Parameter;
    std::vector<MacroCommandInfo*> Commands;

    std::map<Keyword, QCheckBox*> KeywordToCheckbox;
    QLineEdit* NameEdit;
    QListWidget* ParamList;
    QStackedWidget* ParamEdit;
  };

  class MacroFile : public TreeElement<MacroEditor>
  {
  public:
    void setName(const std::string& name);
    std::string name() const { return Name; }
    void addMacro(MacroInfo* macro);
    void delMacro(MacroInfo* macro);
    void delMacro(QTreeWidgetItem* item);

    void createGUI(MacroEditor* parent) override;

  private:
    std::string Name;
    std::vector<MacroInfo*> Macros;
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
  void clickedAdd();
  void clickedDelete();

private:
  void populate(const std::vector<MacroGUI::MacroFile*>& macrofiles);
public:
  int addDetailView(QWidget* view, QTreeWidgetItem* treeItem);

private:
  Macros* InternalMacros;

  std::vector<MacroGUI::MacroFile*> MacroFiles;
  QTreeWidget* MacroTree;
  QStackedWidget* DetailViewContainer;
  std::map<QTreeWidgetItem*, int> TreeToDetailMap;
};

}
