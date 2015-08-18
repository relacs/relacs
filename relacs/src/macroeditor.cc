/*
  macroeditor.cc
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

#include <relacs/macroeditor.h>
#include <functional>
#include <QString>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QCheckBox>
#include <QSpinBox>
#include <QTreeWidgetItem>
#include <QListWidget>

namespace relacs {

namespace {

std::vector<MacroGUI::MacroFile*> testdata()
{
  using namespace MacroGUI;

  MacroFile* file1 = new MacroFile();
  file1->setName("macro_file_1.cfg");
  {
    MacroInfo* macro1 = new MacroInfo();
    macro1->setName("Macro 1");
    macro1->setKeyword(MacroGUI::MacroInfo::Keyword::STARTUP);

    {
      MacroParameter* p1 = new MacroParameter();
      p1->setName("param1");
      p1->setValue("10");
      p1->setUnit("V");
      macro1->addParameter(p1);
    }
    {
      MacroParameter* p1 = new MacroParameter();
      p1->setName("param2");
      p1->setValue("1");
      p1->setUnit("");
      macro1->addParameter(p1);
    }

    {
      MacroCommandInfo* c1 = new MacroCommandInfo();
      c1->setType(MacroCommandInfo::CommandType::SHELL);
      c1->setDeactivated(false);
      macro1->addCommand(c1);
    }

    file1->addMacro(macro1);
  }

  MacroFile* file2 = new MacroFile();
  file2->setName("macro_file_2.cfg");
  {
    MacroInfo* macro1 = new MacroInfo();
    macro1->setName("Macro 1");

    file2->addMacro(macro1);
  }
  {
    MacroInfo* macro2 = new MacroInfo();
    macro2->setName("Macro 2");

    file2->addMacro(macro2);
  }


  return {file1, file2};
}
}

namespace MacroGUI
{
  void MacroCommandShell::updatedCommand() { setCommand(CommandEdit->toPlainText().toStdString()); }
  void MacroCommandShell::setCommand(const std::string& string)
  {
    Command = string;
    if (GuiCreated)
      CommandEdit->setText(QString::fromStdString(string));
  }

  void MacroCommandShell::createGUI(MacroCommandInfo* info)
  {
    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("Command:");
      group->setLayout(new QHBoxLayout());
      CommandEdit = new QTextEdit();
      CommandEdit->setText(QString::fromStdString(Command));
      QObject::connect(CommandEdit, SIGNAL(textChanged()), this, SLOT(updatedCommand()));
      group->layout()->addWidget(CommandEdit);

      DetailView->layout()->addWidget(group);
    }

    GuiCreated = true;
  }

  void MacroCommandBrowse::updatedPath(const QString& string) { setPath(string.toStdString()); }
  void MacroCommandBrowse::setPath(const std::string& string)
  {
    Path = string;
    if (GuiCreated)
      PathEdit->setText(QString::fromStdString(string));
  }

  void MacroCommandBrowse::createGUI(MacroCommandInfo* info)
  {
    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("Path:");
      group->setLayout(new QHBoxLayout());
      PathEdit = new QLineEdit();
      PathEdit->setText(QString::fromStdString(Path));
      QObject::connect(PathEdit, SIGNAL(textChanged(QString)), this, SLOT(updatedPath(QString)));
      group->layout()->addWidget(PathEdit);

      DetailView->layout()->addWidget(group);
    }

    GuiCreated = true;
  }

  void MacroCommandSwitch::updatedPath(const QString& string) { setPath(string.toStdString()); }
  void MacroCommandSwitch::setPath(const std::string& string)
  {
    Path = string;
    if (GuiCreated)
      PathEdit->setText(QString::fromStdString(string));
  }

  void MacroCommandSwitch::createGUI(MacroCommandInfo* info)
  {
    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("Path:");
      group->setLayout(new QHBoxLayout());
      PathEdit = new QLineEdit();
      PathEdit->setText(QString::fromStdString(Path));
      QObject::connect(PathEdit, SIGNAL(textChanged(QString)), this, SLOT(updatedPath(QString)));
      group->layout()->addWidget(PathEdit);

      DetailView->layout()->addWidget(group);
    }

    GuiCreated = true;
  }

  void MacroCommandStartsession::createGUI(MacroCommandInfo *info)
  {
    DetailView = new QWidget();
    GuiCreated = true;
  }

  void MacroCommandMessage::updatedText() { setText(TextEdit->toPlainText().toStdString()); }
  void MacroCommandMessage::setText(const string &string)
  {
    Text = string;
    if (GuiCreated)
      TextEdit->setText(QString::fromStdString(string));
  }

  void MacroCommandMessage::updatedTitle(const QString& text) { setTitle(text.toStdString()); }
  void MacroCommandMessage::setTitle(const string &string)
  {
    Title = string;
    if (GuiCreated)
      TitleEdit->setText(QString::fromStdString(string));
  }

  void MacroCommandMessage::setTimeout(int timeout)
  {
    Timeout = timeout;
    if (GuiCreated)
      TimeoutEdit->setValue(timeout);
  }

  void MacroCommandMessage::createGUI(MacroCommandInfo *info)
  {
    DetailView = new QWidget();
    QVBoxLayout* lay = new QVBoxLayout();
    DetailView->setLayout(lay);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Title:"));
      TitleEdit = new QLineEdit();
      TitleEdit->setText(QString::fromStdString(Title));
      QObject::connect(TitleEdit, SIGNAL(textChanged(QString)), this, SLOT(updatedTitle(QString)));
      sub->addWidget(TitleEdit);

      lay->addLayout(sub);
    }
    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Timeout:"));
      TimeoutEdit = new QSpinBox();
      TimeoutEdit->setValue(Timeout);
      QObject::connect(TimeoutEdit, SIGNAL(valueChanged(int)), this, SLOT(setTimeout(int)));
      sub->addWidget(TimeoutEdit);

      lay->addLayout(sub);
    }
    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Text:"));
      TextEdit = new QTextEdit();
      TextEdit->setText(QString::fromStdString(Text));
      QObject::connect(TextEdit, SIGNAL(textChanged()), this, SLOT(updatedText()));
      sub->addWidget(TextEdit);

      lay->addLayout(sub);
    }

    GuiCreated = true;
  }

  void MacroCommandFilterDetector::setConfigure(double time)
  {
    Configure = time;
    if (GuiCreated)
      ConfigureEdit->setValue(time);
  }

  void MacroCommandFilterDetector::updatedMode(const QString &mode) { setMode(mode == "Save" ? ModeType::SAVE : ModeType::CONFIGURE); }
  void MacroCommandFilterDetector::setMode(ModeType mode)
  {
    Mode = mode;
    if (GuiCreated)
    {
      if (mode == ModeType::SAVE)
        ModeEdit->setCurrentIndex(ModeEdit->findText("Save"));
      else
        ModeEdit->setCurrentIndex(ModeEdit->findText("Configure"));
    }
  }

  void MacroCommandFilterDetector::updatedSave(const QString &param) { setSave(param.toStdString()); }
  void MacroCommandFilterDetector::setSave(const string &param)
  {
    Save = param;
    if (GuiCreated)
      SaveEdit->setText(QString::fromStdString(param));
  }

  void MacroCommandFilterDetector::setAvailable(const std::vector<string> &available)
  {
    Available = available;

    if (GuiCreated)
    {
      NameEdit->clear();
      for (const std::string& name : available)
        NameEdit->addItem(QString::fromStdString(name));

      NameEdit->setCurrentIndex(NameEdit->findText(QString::fromStdString(Name)));
    }
  }

  void MacroCommandFilterDetector::updatedName(const QString &name) { setName(name.toStdString()); }
  void MacroCommandFilterDetector::setName(const string &name)
  {
    Name = name;

    if (GuiCreated)
      NameEdit->setCurrentIndex(NameEdit->findText(QString::fromStdString(name)));
  }

  void MacroCommandFilterDetector::updatedAll(int val) { setAll(val == Qt::Checked); }
  void MacroCommandFilterDetector::setAll(bool all)
  {
    All = all;

    if (GuiCreated)
      AllEdit->setCheckState(all ? Qt::Checked : Qt::Unchecked);
  }

  void MacroCommandFilterDetector::createGUI(MacroCommandInfo *info)
  {
    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());

    {
      QGroupBox* grp = new QGroupBox("Filter/Detector");
      grp->setLayout(new QHBoxLayout());
      NameEdit = new QComboBox();
      for (const std::string& name : Available)
        NameEdit->addItem(QString::fromStdString(name));
      NameEdit->setCurrentIndex(NameEdit->findText(QString::fromStdString(Name)));
      QObject::connect(NameEdit, SIGNAL(activated(QString)), this, SLOT(updatedName(QString)));
      grp->layout()->addWidget(NameEdit);
      AllEdit = new QCheckBox("all");
      AllEdit->setCheckState(All ? Qt::Checked : Qt::Unchecked);
      QObject::connect(AllEdit, SIGNAL(stateChanged(int)), this, SLOT(updatedAll(int)));
      grp->layout()->addWidget(AllEdit);

      DetailView->layout()->addWidget(grp);
    }
    {
      QGroupBox* grp = new QGroupBox("Mode");
      grp->setLayout(new QVBoxLayout());
      ModeEdit = new QComboBox();
      ModeEdit->addItem("Save");
      ModeEdit->addItem("Configure");
      grp->layout()->addWidget(ModeEdit);

      DetailView->layout()->addWidget(grp);
    }

    QStackedWidget* stack = new QStackedWidget();
    {
      QWidget* widget = new QWidget();
      widget->setLayout(new QHBoxLayout());
      widget->layout()->addWidget(new QLabel("Parameter: "));
      SaveEdit = new QLineEdit();
      SaveEdit->setText(QString::fromStdString(Save));
      QObject::connect(SaveEdit, SIGNAL(textChanged(QString)), this, SLOT(updatedSave(QString)));
      widget->layout()->addWidget(SaveEdit);

      stack->addWidget(widget);
    }
    {
      QWidget* widget = new QWidget();
      widget->setLayout(new QHBoxLayout());
      widget->layout()->addWidget(new QLabel("Time: "));
      ConfigureEdit = new QDoubleSpinBox();
      ConfigureEdit->setValue(Configure);
      QObject::connect(ConfigureEdit, SIGNAL(valueChanged(double)), this, SLOT(setConfigure(double)));
      widget->layout()->addWidget(ConfigureEdit);

      stack->addWidget(widget);
    }
    DetailView->layout()->addWidget(stack);

    QObject::connect(ModeEdit, SIGNAL(currentIndexChanged(int)), stack, SLOT(setCurrentIndex(int)));
    if (Mode == ModeType::SAVE)
      ModeEdit->setCurrentIndex(ModeEdit->findText("Save"));
    else
      ModeEdit->setCurrentIndex(ModeEdit->findText("Configure"));

    GuiCreated = true;
  }


  void MacroCommandRepro::setAvailableRepors(const std::vector<string> &repros)
  {
    AvailableRepros = repros;
    if (GuiCreated)
    {
      ReproEdit->clear();
      for (const std::string& repro : AvailableRepros)
        ReproEdit->addItem(QString::fromStdString(repro));
      setRepro(Repro);
    }
  }

  void MacroCommandRepro::updatedRepro(const QString &name) { setRepro(name.toStdString()); }
  void MacroCommandRepro::setRepro(const string &repro)
  {
    Repro = repro;
    if (GuiCreated)
      ReproEdit->setCurrentIndex(ReproEdit->findText(QString::fromStdString(repro)));
  }


  void MacroCommandRepro::addParameter(MacroCommandParameter *param)
  {
    Parameter.push_back(param);

    if (GuiCreated)
    {
      param->createGUI(nullptr);
      ParameterList->addItem(param->listItem());
      ParameterValues->addWidget(param->detailView());
    }
  }

  void MacroCommandRepro::removeParameter(MacroCommandParameter *param)
  {
    auto itr = std::find(Parameter.begin(), Parameter.end(), param);
    if (itr == Parameter.end())
      return;

    if (GuiCreated)
    {
      delete param->listItem();
      ParameterValues->removeWidget(param->detailView());
    }
    delete param;
    Parameter.erase(itr);
  }

  void MacroCommandRepro::addParameter()
  {
    MacroCommandParameter* param = new MacroCommandParameter();
    param->setName("new parameter");
    addParameter(param);
  }

  void MacroCommandRepro::removeParameter()
  {
    QList<QListWidgetItem*> selections = ParameterList->selectedItems();
    if (selections.empty() || selections.size() > 1)
      return;
    QListWidgetItem* selection = selections.front();

    auto itr = std::find_if(Parameter.begin(), Parameter.end(), [&selection](MacroCommandParameter* param) { return param->listItem() == selection; });
    removeParameter(*itr);
  }

  void MacroCommandRepro::createGUI(MacroCommandInfo *info)
  {
    DetailView = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    DetailView->setLayout(layout);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("RePro: "));
      ReproEdit = new QComboBox();
      for (const std::string& repro : AvailableRepros)
        ReproEdit->addItem(QString::fromStdString(repro));
      QObject::connect(ReproEdit, SIGNAL(activated(QString)), this, SLOT(updatedRepro(QString)));
      sub->addWidget(ReproEdit);

      layout->addLayout(sub);
    }
    {
      QGroupBox* sub = new QGroupBox("Parameter");
      QVBoxLayout* sublay = new QVBoxLayout();
      sub->setLayout(sublay);

      {
        QHBoxLayout* lay = new QHBoxLayout();
        ParameterList = new QListWidget();
        lay->addWidget(ParameterList);
        {
          QVBoxLayout* but = new QVBoxLayout();
          QPushButton* add = new QPushButton("+");
          QObject::connect(add, SIGNAL(clicked()), this, SLOT(addParameter()));
          QPushButton* del = new QPushButton("-");
          QObject::connect(del, SIGNAL(clicked()), this, SLOT(removeParameter()));
          but->addWidget(add);
          but->addWidget(del);
          lay->addLayout(but);
        }
        sublay->addLayout(lay);
      }
      {
        ParameterValues = new QStackedWidget();
        sublay->addWidget(ParameterValues);
      }

      layout->addWidget(sub);
    }

    for (MacroCommandParameter* param : Parameter)
    {
      param->createGUI(nullptr);
      ParameterList->addItem(param->listItem());
      ParameterValues->addWidget(param->detailView());
    }

    QObject::connect(ParameterList, SIGNAL(currentRowChanged(int)), ParameterValues, SLOT(setCurrentIndex(int)));

    GuiCreated = true;
  }


  void MacroCommandParameter::updatedName(const QString &name) { setName(name.toStdString()); }
  void MacroCommandParameter::setName(const string &name)
  {
    Name = name;
    if (GuiCreated)
    {
      ListItem->setText(QString::fromStdString(name));
      NameEdit->setText(QString::fromStdString(name));
    }
  }

  void MacroCommandParameter::updatedUnit(const QString &name) { setUnit(name.toStdString()); }
  void MacroCommandParameter::setUnit(const string &name)
  {
    Direct.Unit = name;
    Sequence.Unit = name;
    if (GuiCreated)
    {
      DirectEdit.Unit->setText(QString::fromStdString(name));
      SequenceEdit.Unit->setText(QString::fromStdString(name));
    }
  }

  void MacroCommandParameter::updatedValue(const QString &name) { setValue(name.toStdString()); }
  void MacroCommandParameter::setValue(const string &name)
  {
    Direct.Value = name;
    if (GuiCreated)
    {
      DirectEdit.Value->setText(QString::fromStdString(name));
    }
  }

  void MacroCommandParameter::updatedReference(const QString &name) { setReference(name.toStdString()); }
  void MacroCommandParameter::setReference(const string &name)
  {
    Reference.Reference = name;
    if (GuiCreated)
    {
      ReferenceEdit.References->setCurrentIndex(ReferenceEdit.References->findText(QString::fromStdString(Reference.Reference)));
    }
  }

  void MacroCommandParameter::setAvailableReferences(const std::vector<std::string>& refs)
  {
    Reference.AvailableReferences = refs;
    if (GuiCreated)
    {
      ReferenceEdit.References->clear();
      for (const std::string& str : Reference.AvailableReferences)
        ReferenceEdit.References->addItem(QString::fromStdString(str));
      ReferenceEdit.References->setCurrentIndex(ReferenceEdit.References->findText(QString::fromStdString(Reference.Reference)));
    }
  }

  void MacroCommandParameter::updatedType(int type)
  {
    switch (type)
    {
      case 0: return setType(InputType::DIRECT);
      case 1: return setType(InputType::REFERENCE);
      case 2: return setType(InputType::SEQUENCE);
    }
  }

  void MacroCommandParameter::setType(InputType type)
  {
    Type = type;
    if (GuiCreated)
    {
      switch (Type)
      {
        case InputType::DIRECT: TypeEdit->setCurrentIndex(0); break;
        case InputType::REFERENCE: TypeEdit->setCurrentIndex(1); break;
        case InputType::SEQUENCE: TypeEdit->setCurrentIndex(2); break;
      }
    }
  }

  void MacroCommandParameter::setMinimum(int value)
  {
    Sequence.Min = value;
    if (GuiCreated)
      SequenceEdit.Min->setValue(value);
  }
  void MacroCommandParameter::setMaximum(int value)
  {
    Sequence.Max = value;
    if (GuiCreated)
      SequenceEdit.Max->setValue(value);
  }
  void MacroCommandParameter::setStep(int value)
  {
    Sequence.Step = value;
    if (GuiCreated)
      SequenceEdit.Step->setValue(value);
  }
  void MacroCommandParameter::setResolution(int value)
  {
    Sequence.Resolution = value;
    if (GuiCreated)
      SequenceEdit.Resolution->setValue(value);
  }

  void MacroCommandParameter::createGUI(MacroCommandRepro *)
  {
    ListItem = new QListWidgetItem();
    ListItem->setText(QString::fromStdString(Name));

    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("General");
      QVBoxLayout* layout = new QVBoxLayout();
      group->setLayout(layout);
      {
        QHBoxLayout* sub = new QHBoxLayout();
        sub->addWidget(new QLabel("Name: "));
        NameEdit = new QLineEdit();
        NameEdit->setText(QString::fromStdString(Name));
        QObject::connect(NameEdit, SIGNAL(textChanged(QString)), this, SLOT(updatedName(QString)));
        sub->addWidget(NameEdit);

        layout->addLayout(sub);
      }
      {
        QHBoxLayout* sub = new QHBoxLayout();
        sub->addWidget(new QLabel("Type: "));
        TypeEdit = new QComboBox();
        TypeEdit->addItem("direct");
        TypeEdit->addItem("reference");
        TypeEdit->addItem("sequence");
        QObject::connect(TypeEdit, SIGNAL(activated(int)), this, SLOT(updatedType(int)));
        sub->addWidget(TypeEdit);

        layout->addLayout(sub);
      }
      DetailView->layout()->addWidget(group);
    }
    {
      TypeValues = new QStackedWidget();
      {
        QWidget* widget = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout();
        widget->setLayout(lay);
        lay->addWidget(new QLabel("Value: "));
        DirectEdit.Value = new QLineEdit();
        DirectEdit.Value->setText(QString::fromStdString(Direct.Value));
        QObject::connect(DirectEdit.Value, SIGNAL(textChanged(QString)), this, SLOT(updatedValue(QString)));
        lay->addWidget(DirectEdit.Value);
        lay->addWidget(new QLabel("Unit: "));
        DirectEdit.Unit = new QLineEdit();
        DirectEdit.Unit->setText(QString::fromStdString(Direct.Unit));
        QObject::connect(DirectEdit.Unit, SIGNAL(textChanged(QString)), this, SLOT(updatedUnit(QString)));
        lay->addWidget(DirectEdit.Unit);

        TypeValues->addWidget(widget);
      }
      {
        QWidget* widget = new QWidget();
        QHBoxLayout* lay = new QHBoxLayout();
        widget->setLayout(lay);
        lay->addWidget(new QLabel("Reference: "));
        ReferenceEdit.References = new QComboBox();
        for (const std::string& str : Reference.AvailableReferences)
          ReferenceEdit.References->addItem(QString::fromStdString(str));
        ReferenceEdit.References->setCurrentIndex(ReferenceEdit.References->findText(QString::fromStdString(Reference.Reference)));
        QObject::connect(ReferenceEdit.References, SIGNAL(activated(QString)), this, SLOT(updatedReference(QString)));
        lay->addWidget(ReferenceEdit.References);

        TypeValues->addWidget(widget);
      }
      {
        QWidget* widget = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout();
        widget->setLayout(lay);
        {
          QHBoxLayout* sub = new QHBoxLayout();

          sub->addWidget(new QLabel("Min: "));
          SequenceEdit.Min = new QSpinBox();
          SequenceEdit.Min->setRange(0, std::numeric_limits<int>::max());
          SequenceEdit.Min->setValue(Sequence.Min);
          QObject::connect(SequenceEdit.Min, SIGNAL(valueChanged(int)), this, SLOT(setMinimum(int)));
          sub->addWidget(SequenceEdit.Min);

          sub->addWidget(new QLabel("Max: "));
          SequenceEdit.Max = new QSpinBox();
          SequenceEdit.Max->setRange(0, std::numeric_limits<int>::max());
          SequenceEdit.Max->setValue(Sequence.Max);
          QObject::connect(SequenceEdit.Max, SIGNAL(valueChanged(int)), this, SLOT(setMaximum(int)));
          sub->addWidget(SequenceEdit.Max);

          sub->addWidget(new QLabel("Step: "));
          SequenceEdit.Step = new QSpinBox();
          SequenceEdit.Step->setRange(0, std::numeric_limits<int>::max());
          SequenceEdit.Step->setValue(Sequence.Step);
          QObject::connect(SequenceEdit.Step, SIGNAL(valueChanged(int)), this, SLOT(setStep(int)));
          sub->addWidget(SequenceEdit.Step);

          lay->addLayout(sub);
        }
        {
          QHBoxLayout* sub = new QHBoxLayout();

          sub->addWidget(new QLabel("Resolution: "));
          SequenceEdit.Resolution = new QSpinBox();
          SequenceEdit.Resolution->setValue(Sequence.Resolution);
          QObject::connect(SequenceEdit.Resolution, SIGNAL(valueChanged(int)), this, SLOT(setResolution(int)));
          sub->addWidget(SequenceEdit.Resolution);

          sub->addWidget(new QLabel("Mode: "));
          SequenceEdit.Mode = new QComboBox();
          SequenceEdit.Mode->addItem("up");
          SequenceEdit.Mode->addItem("down");
          //
          sub->addWidget(SequenceEdit.Mode);

          sub->addWidget(new QLabel("Unit: "));
          SequenceEdit.Unit = new QLineEdit();
          SequenceEdit.Unit->setText(QString::fromStdString(Sequence.Unit));
          QObject::connect(SequenceEdit.Unit, SIGNAL(textChanged(QString)), this, SLOT(updatedUnit(QString)));
          sub->addWidget(SequenceEdit.Unit);

          lay->addLayout(sub);
        }

        TypeValues->addWidget(widget);
      }
      DetailView->layout()->addWidget(TypeValues);
    }

    QObject::connect(TypeEdit, SIGNAL(currentIndexChanged(int)), TypeValues, SLOT(setCurrentIndex(int)));
    switch (Type)
    {
      case InputType::DIRECT: TypeEdit->setCurrentIndex(0); break;
      case InputType::REFERENCE: TypeEdit->setCurrentIndex(1); break;
      case InputType::SEQUENCE: TypeEdit->setCurrentIndex(2); break;
    }

    GuiCreated = true;
  }



  void MacroFile::setName(const string &name)
  {
    Name = name;

    if (GuiCreated)
    {
      treeItem()->setText(0, QString::fromStdString(name));
    }
  }

  void MacroFile::addMacro(MacroInfo* macro)
  {
    Macros.push_back(macro);

    if (GuiCreated)
    {
      macro->createGUI(Owner);
      treeItem()->addChild(macro->treeItem());
    }
  }

  void MacroFile::delMacro(MacroInfo* macro)
  {
    if (GuiCreated)
    {
      treeItem()->removeChild(macro->treeItem());
    }
    auto itr = std::find(Macros.begin(), Macros.end(), macro);
    Macros.erase(itr);
  }

  void MacroFile::delMacro(QTreeWidgetItem *item)
  {
    treeItem()->removeChild(item);
    auto itr = std::find_if(Macros.begin(), Macros.end(), [&item] (MacroInfo* comp) { return comp->treeItem() == item; });
    Macros.erase(itr);
  }

  void MacroFile::createGUI(MacroEditor* owner)
  {
    TreeItem = new QTreeWidgetItem();
    TreeItem->setText(0, QString::fromStdString(Name));

    for (MacroInfo* macro : Macros)
    {
      macro->createGUI(owner);
      TreeItem->addChild(macro->treeItem());
    }

    GuiCreated = true;
    Owner = owner;
  }

  struct KeywordInfo
  {
    MacroInfo::Keyword keyword;
    std::string name;
    std::string description;
  };

#define ADD_KEYWORD(KEY, NAME, DESC) \
  { KEY, { KEY, NAME, DESC } },

  static const std::map<MacroInfo::Keyword, KeywordInfo> KEYWORD_LIST = {
    ADD_KEYWORD(MacroInfo::Keyword::STARTUP, "startup", "test")
    ADD_KEYWORD(MacroInfo::Keyword::SHUTDOWN, "shutdown", "")
    ADD_KEYWORD(MacroInfo::Keyword::STARTSESSION, "startsession", "")
    ADD_KEYWORD(MacroInfo::Keyword::STOPSESSION, "stopsession", "")
    ADD_KEYWORD(MacroInfo::Keyword::FALLBACK, "fallback", "")
    ADD_KEYWORD(MacroInfo::Keyword::NOKEY, "nokey", "")
    ADD_KEYWORD(MacroInfo::Keyword::NOBUTTON, "nobutton", "")
    ADD_KEYWORD(MacroInfo::Keyword::NOMENU, "nomenu", "")
    ADD_KEYWORD(MacroInfo::Keyword::KEEP, "keep", "")
    ADD_KEYWORD(MacroInfo::Keyword::OVERWRITE, "overwrite", "")
  };

#undef ADD_KEYWORD

  void MacroInfo::addCommand(MacroCommandInfo *command)
  {
    Commands.push_back(command);

    if (GuiCreated)
    {
      command->createGUI(Owner);
      treeItem()->addChild(command->treeItem());
    }
  }

  void MacroInfo::removeCommand(MacroCommandInfo *command)
  {
    Commands.erase(std::find(Commands.begin(), Commands.end(), command));

    if (GuiCreated)
    {
      treeItem()->removeChild(command->treeItem());
    }
  }

  void MacroInfo::removeCommand(QTreeWidgetItem *item)
  {
    auto itr = std::find_if(Commands.begin(), Commands.end(), [&item](MacroCommandInfo* cmd) { return cmd->treeItem() == item; });
    if (itr != Commands.end())
      removeCommand(*itr);
  }

  void MacroInfo::addParameter(MacroParameter *param)
  {
    Parameter.push_back(param);

    if (GuiCreated)
    {
      param->createGUI(this);
      ParamList->addItem(param->listItem());
      ParamEdit->addWidget(param->detailView());
    }
  }

  void MacroInfo::removeParameter(MacroParameter *param)
  {
    auto itr = std::find(Parameter.begin(), Parameter.end(), param);
    if (itr == Parameter.end())
      return;

    if (GuiCreated)
    {
      delete param->listItem();
      ParamEdit->removeWidget(param->detailView());
    }
    delete param;
    Parameter.erase(itr);
  }

  void MacroInfo::setName(const string &name)
  {
    Name = name;

    if (GuiCreated)
    {
      TreeItem->setText(0, QString::fromStdString(name));
      NameEdit->setText(QString::fromStdString(name));
    }
  }

  void MacroInfo::setKeyword(Keyword keyword)
  {
    Keywords.insert(keyword);

    if (GuiCreated)
    {
      KeywordToCheckbox[keyword]->setCheckState(Qt::Checked);
    }
  }

  void MacroInfo::removeKeyword(Keyword keyword)
  {
    Keywords.erase(keyword);

    if (GuiCreated)
    {
      KeywordToCheckbox[keyword]->setCheckState(Qt::Unchecked);
    }
  }

  void MacroInfo::updatedName(const QString &name)
  {
    setName(name.toStdString());
  }

  void MacroInfo::updatedKeywords(int)
  {
    Keywords.clear();

    for (const std::pair<MacroInfo::Keyword, QCheckBox*>& pair : KeywordToCheckbox)
      if (pair.second->checkState() == Qt::Checked)
        Keywords.insert(pair.first);
  }

  void MacroInfo::addParameter()
  {
    MacroParameter* param = new MacroParameter();
    param->setName("new parameter");
    addParameter(param);
  }

  void MacroInfo::removeParameter()
  {
    QList<QListWidgetItem*> selections = ParamList->selectedItems();
    if (selections.empty() || selections.size() > 1)
      return;
    QListWidgetItem* selection = selections.front();

    auto itr = std::find_if(Parameter.begin(), Parameter.end(), [&selection](MacroParameter* param) { return param->listItem() == selection; });
    removeParameter(*itr);
  }

  void MacroInfo::createGUI(MacroEditor* owner)
  {
    TreeItem = new QTreeWidgetItem();
    TreeItem->setText(0, QString::fromStdString(Name));

    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("Name");
      group->setLayout(new QHBoxLayout());
      NameEdit = new QLineEdit();
      NameEdit->setText(QString::fromStdString(Name));
      QObject::connect(NameEdit, SIGNAL(textEdited(QString)), this, SLOT(updatedName(QString)));
      group->layout()->addWidget(NameEdit);

      DetailView->layout()->addWidget(group);
    }
    {
      QGroupBox* group = new QGroupBox("Keywords");
      group->setLayout(new QGridLayout());

      for (const std::pair<MacroInfo::Keyword, KeywordInfo>& key : KEYWORD_LIST)
      {
        QCheckBox* box = new QCheckBox(QString::fromStdString(key.second.name));
        box->setCheckState(Keywords.count(key.first) ? Qt::Checked : Qt::Unchecked);
        box->setToolTip(QString::fromStdString(key.second.description));

        QObject::connect(box, SIGNAL(stateChanged(int)), this, SLOT(updatedKeywords(int)));
        KeywordToCheckbox[key.first] = box;

        group->layout()->addWidget(box);
      }

      DetailView->layout()->addWidget(group);
    }
    {
      QGroupBox* group = new QGroupBox("Parameter");
      QVBoxLayout* layout = new QVBoxLayout();
      group->setLayout(layout);

      {
        QHBoxLayout* hbox = new QHBoxLayout();
        ParamList = new QListWidget();

        hbox->addWidget(ParamList);
        {
          QVBoxLayout* vbox = new QVBoxLayout();
          QPushButton* add = new QPushButton("+");
          QObject::connect(add, SIGNAL(clicked()), this, SLOT(addParameter()));
          QPushButton* del = new QPushButton("-");
          QObject::connect(del, SIGNAL(clicked()), this, SLOT(removeParameter()));
          vbox->addWidget(add);
          vbox->addWidget(del);
          hbox->addLayout(vbox);
        }
        layout->addLayout(hbox);
      }
      {
        ParamEdit = new QStackedWidget();
        layout->addWidget(ParamEdit);
      }

      DetailView->layout()->addWidget(group);
    }

    QObject::connect(ParamList, SIGNAL(currentRowChanged(int)), ParamEdit, SLOT(setCurrentIndex(int)));
    for (MacroParameter* param : Parameter)
    {
      param->createGUI(this);
      ParamList->addItem(param->listItem());
      ParamEdit->addWidget(param->detailView());
    }

    for (MacroCommandInfo* command : Commands)
    {
      command->createGUI(owner);
      TreeItem->addChild(command->treeItem());
    }

    owner->addDetailView(DetailView, TreeItem);

    GuiCreated = true;
    Owner = owner;
  }


  void MacroParameter::setName(const string &name)
  {
    Name = name;
    if (GuiCreated)
    {
      NameEdit->setText(QString::fromStdString(name));
      ListItem->setText(QString::fromStdString(name));
    }
  }
  void MacroParameter::setUnit(const string &name)
  {
    Unit = name;
    if (GuiCreated)
    {
      UnitEdit->setText(QString::fromStdString(name));
    }
  }
  void MacroParameter::setValue(const string &name)
  {
    Value = name;
    if (GuiCreated)
    {
      ValueEdit->setText(QString::fromStdString(name));
    }
  }

  void MacroParameter::updatedName(const QString &name) { setName(name.toStdString()); }
  void MacroParameter::updatedValue(const QString &name) { setValue(name.toStdString()); }
  void MacroParameter::updatedUnit(const QString &name) { setUnit(name.toStdString()); }

  void MacroParameter::createGUI(MacroInfo *)
  {
    ListItem = new QListWidgetItem();
    ListItem->setText(QString::fromStdString(Name));

    DetailView = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout();
    DetailView->setLayout(layout);

    {
      QHBoxLayout* group = new QHBoxLayout();
      group->addWidget(new QLabel("Name:"));
      NameEdit = new QLineEdit(QString::fromStdString(Name));
      QObject::connect(NameEdit, SIGNAL(textEdited(QString)), this, SLOT(updatedName(QString)));
      group->addWidget(NameEdit);

      layout->addLayout(group);
    }
    {
      QHBoxLayout* group = new QHBoxLayout();
      group->addWidget(new QLabel("Value:"));
      ValueEdit = new QLineEdit(QString::fromStdString(Value));
      QObject::connect(ValueEdit, SIGNAL(textEdited(QString)), this, SLOT(updatedValue(QString)));
      group->addWidget(ValueEdit);

      layout->addLayout(group);
    }
    {
      QHBoxLayout* group = new QHBoxLayout();
      group->addWidget(new QLabel("Unit:"));
      UnitEdit = new QLineEdit(QString::fromStdString(Unit));
      QObject::connect(UnitEdit, SIGNAL(textEdited(QString)), this, SLOT(updatedUnit(QString)));
      group->addWidget(UnitEdit);

      layout->addLayout(group);
    }

    GuiCreated = true;
  }

  struct CommandTypeInfo
  {
    MacroCommandInfo::CommandType type;
    std::string name;
    std::function<DetailElement<MacroCommandInfo>*()> creator;
  };

#define ADD_TYPE(KEY, NAME, TYPE) \
  { KEY, { KEY, NAME, [] { return new TYPE(); } } },

  static const std::map<MacroCommandInfo::CommandType, CommandTypeInfo> COMMANDTYPE_LIST = {
    ADD_TYPE(MacroCommandInfo::CommandType::BROWSE, "browse", MacroCommandBrowse)
    ADD_TYPE(MacroCommandInfo::CommandType::DETECTOR, "detector", MacroCommandFilterDetector)
    ADD_TYPE(MacroCommandInfo::CommandType::FILTER, "filter", MacroCommandFilterDetector)
    ADD_TYPE(MacroCommandInfo::CommandType::MESSAGE, "message", MacroCommandMessage)
    ADD_TYPE(MacroCommandInfo::CommandType::SHELL, "shell", MacroCommandShell)
    ADD_TYPE(MacroCommandInfo::CommandType::SWITCH, "switch", MacroCommandSwitch)
    ADD_TYPE(MacroCommandInfo::CommandType::START_SESSION, "startsession", MacroCommandStartsession)
    ADD_TYPE(MacroCommandInfo::CommandType::REPRO, "repro", MacroCommandRepro)
    ADD_TYPE(MacroCommandInfo::CommandType::MACRO, "macro", MacroCommandShell)
  };

#undef ADD_TYPE

  MacroCommandInfo::MacroCommandInfo()
  {
    for (const std::pair<CommandType, CommandTypeInfo>& type : COMMANDTYPE_LIST)
    {
      DetailElement<MacroCommandInfo>* cmd = type.second.creator();
      Commands[type.first] = cmd;
    }
  }

  void MacroCommandInfo::setDeactivated(bool state)
  {
    Deactivated = state;

    if (GuiCreated)
      DeactivatedEdit->setCheckState(state ? Qt::Checked : Qt::Unchecked);
  }

  void MacroCommandInfo::setType(CommandType type)
  {
    Type = type;

    if (GuiCreated)
    {
      QString name = QString::fromStdString(COMMANDTYPE_LIST.at(type).name);
      TypeEdit->setCurrentIndex(TypeEdit->findText(name));
      TreeItem->setText(0, name);
    }
  }

  void MacroCommandInfo::updateDeactivated(int) { setDeactivated(DeactivatedEdit->checkState() == Qt::Checked); }
  void MacroCommandInfo::updateType(QString text)
  {
    for (const std::pair<CommandType, CommandTypeInfo>& type : COMMANDTYPE_LIST)
      if (type.second.name == text.toStdString())
      {
        setType(type.first);
        break;
      }
  }

  void MacroCommandInfo::createGUI(MacroEditor* owner)
  {
    TreeItem = new QTreeWidgetItem();
    TreeItem->setText(0, QString::fromStdString(COMMANDTYPE_LIST.at(Type).name));

    DetailView = new QWidget();
    DetailView->setLayout(new QVBoxLayout());
    {
      QGroupBox* group = new QGroupBox("General");
      QVBoxLayout* layout = new QVBoxLayout();
      group->setLayout(layout);

      DeactivatedEdit = new QCheckBox("deactivated");
      DeactivatedEdit->setCheckState(Deactivated ? Qt::Checked : Qt::Unchecked);
      QObject::connect(DeactivatedEdit, SIGNAL(stateChanged(int)), this, SLOT(updateDeactivated(int)));
      layout->addWidget(DeactivatedEdit);

      {
        QHBoxLayout* sub = new QHBoxLayout();
        sub->addWidget(new QLabel("Type:"));
        TypeEdit = new QComboBox();
        QObject::connect(TypeEdit, SIGNAL(activated(QString)), this, SLOT(updateType(QString)));
        sub->addWidget(TypeEdit);

        layout->addLayout(sub);
      }

      DetailView->layout()->addWidget(group);
    }
    {
      CommandsEdit = new QStackedWidget();
      DetailView->layout()->addWidget(CommandsEdit);
    }

    for (const std::pair<CommandType, CommandTypeInfo>& type : COMMANDTYPE_LIST)
    {
      TypeEdit->addItem(QString::fromStdString(type.second.name));
      DetailElement<MacroCommandInfo>* cmd =  Commands.at(type.first);
      cmd->createGUI(this);
      CommandsEdit->addWidget(cmd->detailView());
    }
    QObject::connect(TypeEdit, SIGNAL(currentIndexChanged(int)), CommandsEdit, SLOT(setCurrentIndex(int)));
    TypeEdit->setCurrentIndex(TypeEdit->findText(QString::fromStdString(COMMANDTYPE_LIST.at(Type).name)));

    owner->addDetailView(DetailView, TreeItem);

    Owner = owner;
    GuiCreated = true;
  }
}

namespace MacroMgr
{
  void MacroFileReader::load(const string &filename)
  {
    MacroFile = new MacroGUI::MacroFile();
    MacroFile->setName(filename);

    std::ifstream file(filename);
    std::string line = "";
    Str realLine = "";
    while (std::getline(file, line))
    {
      realLine = line;
      realLine.strip( Str::WhiteSpace, "#" );
      if (realLine.empty())
        continue;

      // first macro defintion;
      if (realLine.front() == '$')
        break;
    }

    while (!realLine.empty())
    {
      if (realLine.front() == '$')
      {
        realLine.erase(0, 1);
        realLine.strip();
        loadMacro(realLine);
      }
      else
        loadMacroParameter(realLine);

      realLine = loadMacroCommands(line, file);
    }

    if (!MacroFile->macros().empty() && !TempCommands.empty())
      addCommandsToMacro();

  }

  void MacroFileReader::loadMacro(Str realLine)
  {
    if (!MacroFile->macros().empty() && !TempCommands.empty())
      addCommandsToMacro();

    MacroFile->addMacro(new MacroGUI::MacroInfo());
    TempCommands.clear();

    int index = realLine.find(':');
    if (index > 0)
    {
      loadMacroParameter(realLine.substr(index + 1));
      realLine.erase(index);
    }

    for (const std::pair<MacroGUI::MacroInfo::Keyword, MacroGUI::KeywordInfo> pair : MacroGUI::KEYWORD_LIST)
    {
      if (realLine.erase(pair.second.name, 0, false, 3, Str::WordSpace) > 0)
        MacroFile->macros().back()->setKeyword(pair.first);
    }

    MacroFile->macros().back()->setName(realLine.stripped( Str::WordSpace ));
  }

  void MacroFileReader::loadMacroParameter(const Str& realLine)
  {
    Options vars;
    vars.load( realLine, "=", ";" );
    vars.setToDefaults();

    for (Parameter& param : vars)
    {
      MacroGUI::MacroParameter* p = new MacroGUI::MacroParameter();
      p->setName(param.name());
      p->setUnit(param.unit());
      p->setValue(param.text());

      MacroFile->macros().back()->addParameter(p);
    }
  }

  Str MacroFileReader::loadMacroCommands(std::string& line, ifstream &file)
  {
    using CmdType = MacroGUI::MacroCommandInfo::CommandType;

    bool appendable = true;
    bool appendmacro = true;
    bool appendparam = true;

    while (std::getline(file, line))
    {
      Str realLine = line;
      realLine.strip(Str::WhiteSpace, "#");

      if (realLine.empty())
      {
        appendable = false;
        continue;
      }

      if (realLine.front() == '$')
        return realLine;

      CommandInput info = loadMacroCommand(realLine);

      if (appendable &&
          info.type == CmdType::UNKNOWN &&
          info.params.empty() &&
          ((appendparam && realLine.find('=') >= 0) ||
           (!appendparam && line.find_first_not_of(Str::WhiteSpace) != std::string::npos)))
      {
        if (appendmacro)
          return realLine;
        else
        {
          Str& params = TempCommands.back().params;

          if (appendparam && !params.empty())
            params.provideLast(';');
          params.provideLast(' ');
          params += realLine;
        }
      }
      else if (info.type != CmdType::START_SESSION &&
               info.type != CmdType::SHELL &&
               info.type != CmdType::FILTER &&
               info.type != CmdType::DETECTOR &&
               info.type != CmdType::MESSAGE &&
               info.type != CmdType::BROWSE &&
               info.name.empty())
      {
        appendable = false;
      }
      else
      {
        if (info.type  == CmdType::UNKNOWN &&
            !info.name.empty())
          info.type  = CmdType::REPRO;

        if (info.type  == CmdType::UNKNOWN)
        {
          appendable = false;
        }
        else
        {
          TempCommands.push_back(info);

          if (info.type  == CmdType::START_SESSION ||
              info.type  == CmdType::BROWSE ||
              info.type  == CmdType::SWITCH)
            appendable = false;
          else if (info.type  == CmdType::SHELL ||
                   info.type  == CmdType::MESSAGE)
          {
            appendable = true;
            appendmacro = false;
            appendparam = false;
          }
          else
          {
            appendable = true;
            appendmacro = false;
            appendparam = true;
          }
        }
      }
    }
    return "";
  }

  MacroFileReader::CommandInput MacroFileReader::loadMacroCommand(const std::string& line)
  {
    using CmdType = MacroGUI::MacroCommandInfo::CommandType;

    CmdType type;
    bool deactivated;
    Str name;
    Str params;

    size_t pos = line.find_first_of(':');
    if (pos != std::string::npos)
    {
      name = line.substr(0, pos);
      params = line.substr(pos + 1);
      params.strip();
    }
    else
      name = line;

    if (line.front() == '!')
    {
      deactivated = true;
      name.erase(0, 1);
    }
    else
      deactivated = false;

    type = CmdType::UNKNOWN;
    for (const std::pair<MacroGUI::MacroCommandInfo::CommandType, MacroGUI::CommandTypeInfo>& pair : MacroGUI::COMMANDTYPE_LIST)
    {
      if (name.eraseFirst(pair.second.name, 0, false, 3, Str::WhiteSpace))
      {
        type = pair.first;
        break;
      }
    }

    name.strip(Str::WhiteSpace);

    return {type, deactivated, name, params};
  }

  void MacroFileReader::addCommandsToMacro()
  {
    using CmdType = MacroGUI::MacroCommandInfo::CommandType;
    using namespace MacroGUI;

    MacroInfo* macro = MacroFile->macros().back();

    /*
    for (const CommandInput& info : TempCommands)
    {
      std::cout << "type: " << (int)info.type << " deactive: " << info.deactivated
                   << "name: " << info.name << " params: " << info.params << std::endl;
    }
    */

    for (CommandInput& info : TempCommands)
    {
      MacroCommandInfo* cmd = new MacroCommandInfo();
      macro->addCommand(cmd);

      cmd->setDeactivated(info.deactivated);
      cmd->setType(info.type);

      switch (info.type)
      {
        case CmdType::MESSAGE:
        {
          MacroCommandMessage* msg = cmd->command<CmdType::MESSAGE>();
          msg->setText(info.params);

          int n = 0;
          msg->setTimeout(info.name.number(0.0, 0, &n));
          msg->setTitle(info.name.substr(n + 1).strip(Str::WhiteSpace));
          break;
        }
        case CmdType::SHELL:
        {
          MacroCommandShell* msg = cmd->command<CmdType::SHELL>();
          msg->setCommand(info.name);
          break;
        }
        case CmdType::BROWSE:
        {
          MacroCommandBrowse* msg = cmd->command<CmdType::BROWSE>();
          msg->setPath(info.name);
          break;
        }
        case CmdType::START_SESSION:
          break;
        case CmdType::SWITCH:
        {
          MacroCommandSwitch* msg = cmd->command<CmdType::SWITCH>();
          msg->setPath(info.name);
          break;
        }
        case CmdType::FILTER:
        case CmdType::DETECTOR:
        {
          MacroCommandFilterDetector* msg;

          if (info.type == CmdType::FILTER)
            msg = cmd->command<CmdType::FILTER>();
          else
            msg = cmd->command<CmdType::DETECTOR>();

          if (info.name.empty())
            msg->setAll(true);
          else
            msg->setName(info.name);

          if (info.params.eraseFirst("save", 0, false, 3, Str::WhiteSpace))
          {
            msg->setMode(MacroCommandFilterDetector::ModeType::SAVE);
            msg->setSave(info.params);
          }
          else if (info.params.eraseFirst("autoconf", 0, false, 3, Str::WhiteSpace))
          {
            msg->setMode(MacroCommandFilterDetector::ModeType::CONFIGURE);
            msg->setConfigure(info.params.number(1.0));
          }
        }
        case CmdType::REPRO:
        {
          MacroCommandRepro* msg = cmd->command<CmdType::REPRO>();
          msg->setAvailableRepors({info.name}); // todo XXX tmp
          msg->setRepro(info.name);

          StrQueue sq(info.params.stripped().preventLast(";"), ";");
          for (Str& str : sq)
          {
            MacroCommandParameter* param = new MacroCommandParameter();
            msg->addParameter(param);

            std::string name = str.ident(0, "=", Str::WhiteSpace);
            std::string value = str.value();

            param->setName(name);

            if (value.front() == '$')
            {
              param->setType(MacroCommandParameter::InputType::REFERENCE);
              param->setReference(value.substr(1));
            }
            else if (value.front() == '(')
            {
              param->setType(MacroCommandParameter::InputType::SEQUENCE);

              value.erase(0, 1);
              size_t idx = value.find(')');

              param->setUnit(value.substr(idx + 1));

              value = value.substr(0, idx);
              StrQueue sq(value, ",");
              for (Str& str : sq)
              {
                if (str.contains(".."))
                {
                  StrQueue sq2(str, "..");
                  param->setMinimum(sq2[0].number());
                  param->setMaximum(sq2[2].number());
                  if (sq2.size() > 4)
                    param->setStep(sq2[4].number());
                }
                else if(str.contains("r="))
                {
                  int val = static_cast<int>(str.substr(2).number());
                  param->setResolution(val);
                }
                else
                {
                  // todo XXX read type
                }
              }
            }
            else
            {
              param->setType(MacroCommandParameter::InputType::DIRECT);
              param->setValue(value);
            }
          }
        }
        case CmdType::MACRO:
          break;
      }


    }
  }
}


MacroEditor::MacroEditor(Macros* macros, QWidget *parent) :
  QWidget(parent),
  InternalMacros(macros)
{
  setLayout(new QHBoxLayout());

  {
    QGroupBox* group = new QGroupBox("Macro list");
    QVBoxLayout* layout = new QVBoxLayout();
    group->setLayout(layout);

    MacroTree = new QTreeWidget();
    MacroTree->setHeaderLabels({"Macro", "Type"});

    QObject::connect(MacroTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    layout->addWidget(MacroTree);

    {
      QHBoxLayout* lay = new QHBoxLayout();
      QPushButton* add = new QPushButton("Add");
      QObject::connect(add, SIGNAL(clicked()), this, SLOT(clickedAdd()));
      lay->addWidget(add);
      QPushButton* del = new QPushButton("Delete");
      QObject::connect(del, SIGNAL(clicked()), this, SLOT(clickedDelete()));
      lay->addWidget(del);

      layout->addLayout(lay);
    }

    this->layout()->addWidget(group);
  }
  {
    QGroupBox* group = new QGroupBox("Options");
    group->setLayout(new QVBoxLayout());

    DetailViewContainer = new QStackedWidget();
    DetailViewContainer->addWidget(new QWidget());
    group->layout()->addWidget(DetailViewContainer);

    this->layout()->addWidget(group);
  }

  populate(readFiles());
  //populate(testdata());
}

std::vector<MacroGUI::MacroFile*> MacroEditor::readFiles()
{
  std::vector<MacroGUI::MacroFile*> files;

  std::vector<std::string> filenames;
  InternalMacros->texts("file", filenames);

  for (const std::string& filename : filenames)
  {
    MacroMgr::MacroFileReader reader;
    reader.load(filename);
    files.push_back(reader.file());
  }

  return files;
}

void MacroEditor::populate(const std::vector<MacroGUI::MacroFile*> &macrofiles)
{
  MacroFiles = macrofiles;
  for (MacroGUI::MacroFile* file : MacroFiles)
  {
    file->createGUI(this);
    MacroTree->addTopLevelItem(file->treeItem());
    MacroTree->expandItem(file->treeItem());
  }

  for (int i = 0; i < MacroTree->columnCount(); ++i)
    MacroTree->resizeColumnToContents(i);
}

int MacroEditor::addDetailView(QWidget *view, QTreeWidgetItem* treeItem)
{
  int index = DetailViewContainer->addWidget(view);
  TreeToDetailMap[treeItem] = index;
  return index;
}

void MacroEditor::currentItemChanged(QTreeWidgetItem* item,QTreeWidgetItem*)
{
  auto itr = TreeToDetailMap.find(item);
  if (itr != TreeToDetailMap.end())
    DetailViewContainer->setCurrentIndex(itr->second);
  else
    DetailViewContainer->setCurrentIndex(0);
}

void MacroEditor::clickedAdd()
{
  QList<QTreeWidgetItem*> selections = MacroTree->selectedItems();
  if (selections.empty() || selections.size() > 1)
    return;
  QTreeWidgetItem* selection = selections.front();

  auto itr = std::find_if(MacroFiles.begin(), MacroFiles.end(), [&selection](MacroGUI::MacroFile* file) { return file->treeItem() == selection; });
  if (itr != MacroFiles.end())
  {
    MacroGUI::MacroInfo* macro = new MacroGUI::MacroInfo();
    macro->setName("New Macro");
    (*itr)->addMacro(macro);
  }
  else
  {
    for (MacroGUI::MacroFile* file : MacroFiles)
    {
      auto itr2 = std::find_if(file->Macros.begin(), file->Macros.end(), [&selection](MacroGUI::MacroInfo* macro) { return macro->treeItem() == selection; });
      if (itr2 != file->Macros.end())
      {
        MacroGUI::MacroCommandInfo* cmd = new MacroGUI::MacroCommandInfo();
        (*itr2)->addCommand(cmd);
        break;
      }
    }
  }
}

void MacroEditor::clickedDelete()
{
  QList<QTreeWidgetItem*> selections = MacroTree->selectedItems();
  if (selections.empty() || selections.size() > 1)
    return;
  QTreeWidgetItem* selection = selections.front();
  QTreeWidgetItem* parent = selection->parent();

  auto itr = std::find_if(MacroFiles.begin(), MacroFiles.end(), [&parent](MacroGUI::MacroFile* file) { return file->treeItem() == parent; });
  if (itr != MacroFiles.end())
    (*itr)->delMacro(selection);
  else
  {
    for (MacroGUI::MacroFile* file : MacroFiles)
    {
      auto itr2 = std::find_if(file->Macros.begin(), file->Macros.end(), [&parent](MacroGUI::MacroInfo* macro) { return macro->treeItem() == parent; });
      if (itr2 != file->Macros.end())
      {
        (*itr2)->removeCommand(selection);
        break;
      }
    }
  }
}

void MacroEditor::dialogClosed(int code)
{
  if (code != 1)
    delete this;
}

}

#include <moc_macroeditor.cc>
