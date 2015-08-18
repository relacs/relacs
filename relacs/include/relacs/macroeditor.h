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
#include <type_traits>
#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
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

  class MacroCommandBrowse;
  class MacroCommandFilterDetector;
  class MacroCommandMessage;
  class MacroCommandRepro;
  class MacroCommandShell;
  class MacroCommandStartsession;
  class MacroCommandSwitch;

  class MacroCommandInfo : public QObject, public TreeElement<MacroEditor>, public DetailElement<MacroEditor>
  {
    Q_OBJECT
  public:
    enum class CommandType
    {
      UNKNOWN,
      FILTER, DETECTOR, MESSAGE, BROWSE, SHELL, SWITCH, START_SESSION, REPRO, MACRO
    };

    MacroCommandInfo();

    void createGUI(MacroEditor*) override;

  public:
    void setDeactivated(bool state);
    void setType(CommandType type);

    CommandType type() const { return Type; }

#define MAP_TYPE(ENUM_VAL, CLASS_NAME) \
  template<CommandType TYPE> \
  typename std::enable_if<TYPE == ENUM_VAL, CLASS_NAME*>::type \
  command() const \
  { \
    return dynamic_cast<CLASS_NAME*>(Commands.at(TYPE)); \
  }

    MAP_TYPE(CommandType::FILTER, MacroCommandFilterDetector)
    MAP_TYPE(CommandType::DETECTOR, MacroCommandFilterDetector)
    MAP_TYPE(CommandType::MESSAGE, MacroCommandMessage)
    MAP_TYPE(CommandType::BROWSE, MacroCommandBrowse)
    MAP_TYPE(CommandType::SHELL, MacroCommandShell)
    MAP_TYPE(CommandType::SWITCH, MacroCommandSwitch)
    MAP_TYPE(CommandType::START_SESSION, MacroCommandStartsession)
    MAP_TYPE(CommandType::REPRO, MacroCommandRepro)
    MAP_TYPE(CommandType::MACRO, MacroCommandRepro)


#undef MAP_TYPE


  private slots:
    void updateDeactivated(int);
    void updateType(QString);

  protected:
    CommandType Type;
    bool Deactivated;

  private:
    std::map<CommandType, DetailElement<MacroCommandInfo>*> Commands;

    QCheckBox* DeactivatedEdit;
    QComboBox* TypeEdit;
    QStackedWidget* CommandsEdit;
  };

  class MacroCommandShell : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setCommand(const std::string& string);

    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedCommand();

  private:
    std::string Command;

    QTextEdit* CommandEdit;
  };

  class MacroCommandBrowse : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setPath(const std::string& string);

    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedPath(const QString& string);

  private:
    std::string Path;

    QLineEdit* PathEdit;
  };

  class MacroCommandStartsession : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void createGUI(MacroCommandInfo* info);
  };

  class MacroCommandSwitch : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setPath(const std::string& string);

    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedPath(const QString& string);

  private:
    std::string Path;

    QLineEdit* PathEdit;
  };

  class MacroCommandMessage : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setTitle(const std::string& title);
    void setText(const std::string& string);
  public slots:
    void setTimeout(int timeout);
  public:
    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedText();
    void updatedTitle(const QString& text);

  private:
    std::string Title;
    int Timeout;
    std::string Text;

    QTextEdit* TextEdit;
    QLineEdit* TitleEdit;
    QSpinBox* TimeoutEdit;
  };

  class MacroCommandFilterDetector : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    enum class ModeType
    {
      SAVE, CONFIGURE
    };

    void setAvailable(const std::vector<std::string>& available);
    void setName(const std::string& name);
    void setAll(bool all);
    void setMode(ModeType mode);
  public slots:
    void setConfigure(double time);
  public:
    void setSave(const std::string& param);

    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedMode(const QString& mode);
    void updatedSave(const QString& param);
    void updatedName(const QString& name);
    void updatedAll(int);

  private:
    std::vector<std::string> Available;
    std::string Name;
    bool All;
    ModeType Mode;
    double Configure;
    std::string Save;

    QComboBox* NameEdit;
    QCheckBox* AllEdit;
    QComboBox* ModeEdit;
    QDoubleSpinBox* ConfigureEdit;
    QLineEdit* SaveEdit;
  };

  class MacroCommandRepro;

  class MacroCommandParameter : public QObject, public DetailElement<MacroCommandRepro>
  {
    Q_OBJECT
  public:
    enum class InputType
    {
      DIRECT, REFERENCE, SEQUENCE
    };

    enum class SequenceMode
    {
      UP, DOWN
    };

    void setName(const std::string& name);
    void setType(InputType type);
    void setValue(const std::string& value);
    void setUnit(const std::string& unit);
    void setAvailableReferences(const std::vector<std::string>& refs);
    void setReference(const std::string& ref);
  public slots:
    void setMinimum(int min);
    void setMaximum(int max);
    void setStep(int step);
    void setResolution(int resolution);
  public:
    //void setMode(SequenceMode mode);

    void createGUI(MacroCommandRepro*);
    QListWidgetItem* listItem() const { return ListItem; }

  private slots:
    void updatedName(const QString& name);
    void updatedType(int type);
    void updatedValue(const QString& value);
    void updatedUnit(const QString& unit);
    void updatedReference(const QString& reference);
    //void updatedMode(const QString& mode);

  private:
    std::string Name;
    InputType Type;
    struct
    {
      std::string Value;
      std::string Unit;
    } Direct;
    struct
    {
      std::vector<std::string> AvailableReferences;
      std::string Reference;
    } Reference;
    struct
    {
      int Min;
      int Max;
      int Step;
      int Resolution;
      SequenceMode Mode;
      std::string Unit;
    } Sequence;


    QListWidgetItem* ListItem;

    QLineEdit* NameEdit;
    QComboBox* TypeEdit;
    QStackedWidget* TypeValues;
    struct
    {
      QLineEdit* Value;
      QLineEdit* Unit;
    } DirectEdit;
    struct
    {
      QComboBox* References;
    } ReferenceEdit;
    struct
    {
      QSpinBox* Min;
      QSpinBox* Max;
      QSpinBox* Step;
      QSpinBox* Resolution;
      QComboBox* Mode;
      QLineEdit* Unit;
    } SequenceEdit;
  };

  class MacroCommandRepro : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setAvailableRepors(const std::vector<std::string>& repros);
    void setRepro(const std::string& repro);
    void addParameter(MacroCommandParameter* param);
    void removeParameter(MacroCommandParameter* param);

    void createGUI(MacroCommandInfo* info);

  private slots:
    void updatedRepro(const QString& name);
    void addParameter();
    void removeParameter();

  private:
    std::vector<std::string> AvailableRepros;
    std::string Repro;
    std::vector<MacroCommandParameter*> Parameter;

    QComboBox* ReproEdit;
    QListWidget* ParameterList;
    QStackedWidget* ParameterValues;
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
    void setKeyword(Keyword keyword);
    void removeKeyword(Keyword keyword);
    void addParameter(MacroParameter* param);
    void removeParameter(MacroParameter* param);
    void addCommand(MacroCommandInfo* command);
    void removeCommand(MacroCommandInfo* command);
    void removeCommand(QTreeWidgetItem* item);

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
    friend class ::relacs::MacroEditor;

  public:
    void setName(const std::string& name);
    std::string name() const { return Name; }
    void addMacro(MacroInfo* macro);
    void delMacro(MacroInfo* macro);
    void delMacro(QTreeWidgetItem* item);

    const std::vector<MacroInfo*>& macros() const { return Macros; }

    void createGUI(MacroEditor* parent) override;

  private:
    std::string Name;
    std::vector<MacroInfo*> Macros;
  };

}

namespace MacroMgr
{
  class MacroFileReader
  {
    struct CommandInput
    {
      MacroGUI::MacroCommandInfo::CommandType type;
      bool deactivated;
      Str name;
      Str params;
    };

  public:
    void load(const std::string& filename);

    MacroGUI::MacroFile* file() const { return MacroFile; }

  private:
    void loadMacro(Str realLine);
    void loadMacroParameter(const Str& realLine);
    Str loadMacroCommands(std::string& line, std::ifstream& file);
    CommandInput loadMacroCommand(const std::string& line);
    void addCommandsToMacro();

  private:
    MacroGUI::MacroFile* MacroFile;
    std::vector<CommandInput> TempCommands;

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
  std::vector<MacroGUI::MacroFile*> readFiles();
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
