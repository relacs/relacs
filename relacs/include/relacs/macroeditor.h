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
#include <relacs/filterdetectors.h>

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

  signals:
    void macroParameterChanged(const std::string& oldName, const std::string& newName);

  public:
    void createGUI(MacroInfo*) override;

    const std::string& name() const { return Name; }
    const std::string& value() const { return Value; }
    const std::string& unit() const { return Unit; }

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
  class MacroCommandReproMacro;
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

    bool deactivated() const { return Deactivated; }
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
    MAP_TYPE(CommandType::REPRO, MacroCommandReproMacro)
    MAP_TYPE(CommandType::MACRO, MacroCommandReproMacro)


#undef MAP_TYPE


  private slots:
    void updateDeactivated(int);
    void updateType(QString);
    void clickedUp();
    void clickedDown();

  public slots:
    void macroParameterAdded(const std::string& name);
    void macroParameterRemoved(const std::string& name);

  signals:
    void clickedUp(MacroGUI::MacroCommandInfo*);
    void clickedDown(MacroGUI::MacroCommandInfo*);

  protected:
    CommandType Type = CommandType::REPRO;
    bool Deactivated = false;

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
    const std::string& command() const { return Command; }

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
    const std::string& path() const { return Path; }

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
    const std::string& path() const { return Path; }

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

    const std::string& title() const { return Title; }
    const std::string& text() const { return Text; }
    int timeout() const { return Timeout; }

  private slots:
    void updatedText();
    void updatedTitle(const QString& text);

  private:
    std::string Title;
    int Timeout = 0;
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
    void setActive(const std::string& name);
    void setAll(bool all);
    void setMode(ModeType mode);
  public slots:
    void setConfigure(double time);
  public:
    void setSave(const std::string& param);

    void createGUI(MacroCommandInfo* info);

    const std::string& active() const { return Active; }
    bool all() const { return All; }
    ModeType mode() const { return Mode; }
    double configure() const { return Configure; }
    const std::string& save() const { return Save; }

  private slots:
    void updatedMode(const QString& mode);
    void updatedSave(const QString& param);
    void updatedActive(const QString& name);
    void updatedAll(int);

  private:
    std::vector<std::string> Available;
    std::string Active;
    bool All = false;
    ModeType Mode = ModeType::CONFIGURE;
    double Configure = 0.0d;
    std::string Save;

    QComboBox* ActiveEdit;
    QCheckBox* AllEdit;
    QComboBox* ModeEdit;
    QDoubleSpinBox* ConfigureEdit;
    QLineEdit* SaveEdit;
  };

  class MacroCommandParameter : public QObject, public DetailElement<MacroCommandReproMacro>
  {
    Q_OBJECT
  public:
    enum class InputType
    {
      DIRECT, REFERENCE, SEQUENCE_SINGLE, SEQUENCE_LIST
    };

    enum class SequenceMode
    {
      UP, DOWN, ALTERNATE_IN_UP, ALTERNATE_IN_DOWN, ALTERNATE_OUT_UP, ALTERNATE_OUT_DOWN, RANDOM, PSEUDO_RANDOM
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
    void setList(const std::string& list);
    void setMode(SequenceMode mode);

    void createGUI(MacroCommandReproMacro*);
    QListWidgetItem* listItem() const { return ListItem; }

    void updatedReferences(const std::string& name, bool added);

    const std::string& name() const { return Name; }
    InputType type() const { return Type; }
    const std::string& value() const { return Direct.Value; }
    const std::string& unit() const { return Unit; }
    const std::string& reference() const { return Reference.Reference; }
    int min() const { return Sequence.Min; }
    int max() const { return Sequence.Max; }
    int step() const { return Sequence.Step; }
    int resolution() const { return Sequence.Resolution; }
    const std::string& list() const { return SequenceList.List; }
    SequenceMode mode() const { return Sequence.Mode; }

  private slots:
    void updatedName(const QString& name);
    void updatedType(int type);
    void updatedValue(const QString& value);
    void updatedUnit(const QString& unit);
    void updatedReference(const QString& reference);
    void updatedMode(const QString& mode);
    void updatedList(const QString& list);

  private:
    std::string Name;
    InputType Type = InputType::DIRECT;
    std::string Unit;
    struct
    {
      std::string Value;
    } Direct;
    struct
    {
      std::vector<std::string> AvailableReferences;
      std::string Reference;
    } Reference;
    struct
    {
      int Min = 0;
      int Max = 100;
      int Step = 1;
      int Resolution = 1;
      SequenceMode Mode = SequenceMode::UP;
    } Sequence;
    struct
    {
      std::string List;
    } SequenceList;


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
    struct
    {
      QLineEdit* List;
      QLineEdit* Unit;
    } SequenceListEdit;
  };

  class MacroCommandReproMacro : public QObject, public DetailElement<MacroCommandInfo>
  {
    Q_OBJECT
  public:
    void setAvailable(const std::vector<std::string>& repros);
    void setActive(const std::string& repro);
    void addParameter(MacroCommandParameter* param);
    void removeParameter(MacroCommandParameter* param);

    void createGUI(MacroCommandInfo* info);

    void updateParameterReferences(const std::string& param, bool added);

    const std::string& active() const { return Active; }
    const std::vector<MacroCommandParameter*> parameter() const { return Parameter; }

  public slots:
    void updateMacroReferences(const std::string& added, const std::string& removed);
  private slots:
    void updatedActive(const QString& name);
    void addParameter();
    void removeParameter();

  private:
    std::vector<std::string> Available;
    std::string Active;
    std::vector<MacroCommandParameter*> Parameter;

    QComboBox* ActiveEdit;
    QListWidget* ParameterList;
    QStackedWidget* ParameterValues;
  };

  class MacroInfo : public QObject, public TreeElement<MacroEditor>, public DetailElement<MacroEditor>
  {
    friend class ::relacs::MacroEditor;
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

    const std::string& name() const { return Name; }
    const std::set<Keyword>& keywords() const { return Keywords; }
    const std::vector<MacroCommandInfo*>& commands() const { return Commands; }
    const std::vector<MacroParameter*>& parameter() const { return Parameter; }

  protected:
    std::vector<MacroCommandInfo*>& commands() { return Commands; }

  private slots:
    void updatedName(const QString& name);
    void updatedKeywords(int);
    void addParameter();
    void removeParameter();

  signals:
    void macroParameterAdded(const std::string& name);
    void macroParameterRemoved(const std::string& name);
    void macroNameChanged(const std::string& newName, const std::string& oldName);

  public slots:
    void macroParameterChanged(const std::string& oldName, const std::string& newName);

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

  class MacroFileWriter
  {
  public:
    MacroFileWriter(MacroGUI::MacroFile* file, const std::string& filename);
    void save();

  private:
    void write(MacroGUI::MacroInfo* macro);
    void write(MacroGUI::MacroCommandInfo* cmd);
    void write(MacroGUI::MacroCommandParameter* param);

  private:
    MacroGUI::MacroFile* MacroFile;
    std::string Filename;
    std::ofstream File;
  };
}

class MacroEditor : public QWidget
{
  Q_OBJECT

public:
  MacroEditor(Macros* macros, QWidget* parent = nullptr);

  void load();

  void setRepros(RePros* repros);
  void setFilterDetectors(FilterDetectors* filters);
  const std::vector<std::string>& repros() const { return Repros; }
  const std::vector<std::string>& filters() const { return Filters; }
  const std::vector<std::string>& detectors() const { return Detectors; }
  const std::vector<std::string>& macros() const { return MacroList; }

public slots:
  void dialogClosed( int code );
  void clickedUp(MacroGUI::MacroCommandInfo* cmd);
  void clickedDown(MacroGUI::MacroCommandInfo* cmd);

signals:
  void macroDefinitionsChanged();
  void macroChanged(const std::string& added, const std::string& removed);

private slots:
  void currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
  void clickedAdd();
  void clickedDelete();

private:
  void populate(const std::vector<MacroGUI::MacroFile*>& macrofiles);
  std::vector<MacroGUI::MacroFile*> readFiles();

  void moveItem(MacroGUI::MacroCommandInfo* cmd, bool up);

public:
  int addDetailView(QWidget* view, QTreeWidgetItem* treeItem);

private:
  Macros* InternalMacros;
  std::vector<std::string> Repros;
  std::vector<std::string> Filters;
  std::vector<std::string> Detectors;
  std::vector<std::string> MacroList;

  std::vector<MacroGUI::MacroFile*> MacroFiles;
  QTreeWidget* MacroTree;
  QStackedWidget* DetailViewContainer;
  std::map<QTreeWidgetItem*, int> TreeToDetailMap;
};

}
