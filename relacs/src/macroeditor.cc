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

namespace relacs {

namespace {
static const std::map<std::string, int> MACRO_KEYWORDS =
{
  {"startup", 0x01},
  {"shutdown", 0x02},
  {"startsession", 0x04},
  {"stopsession", 0x08},
  {"fallback", 0x10},
  {"nokey", 0x20},
  {"nobutton", 0x40},
  {"nomenu", 0x80},
  {"keep", 0x100},
  {"overwrite",0x200}
};

class MacroParameter
{
public:
  Parameter param;

  void updateGUI()
  {
    gui.group = new QWidget();
    gui.group->setLayout(new QHBoxLayout());
    gui.group->layout()->addWidget(new QLabel("Name: "));
    gui.name = new QLineEdit(param.name().c_str());
    gui.group->layout()->addWidget(gui.name);
    gui.group->layout()->addWidget(new QLabel("Value: "));
    gui.value = new QLineEdit(param.text().c_str());
    gui.group->layout()->addWidget(gui.value);
    gui.group->layout()->addWidget(new QLabel("Unit: "));
    gui.unit = new QLineEdit(param.unit().c_str());
    gui.group->layout()->addWidget(gui.unit);
    QPushButton* del = new QPushButton("Delete");
    gui.group->layout()->addWidget(del);
  }

  QWidget* widget() const { return gui.group; }

  struct
  {
    QWidget* group;

    QLineEdit* name;
    QLineEdit* value;
    QLineEdit* unit;
  } gui;
};

class MacroCommandInfo
{
public:
  enum class Type
  {
    FILTER, DETECTOR, MESSAGE, BROWSE, SHELL, SWITCH, START_SESSION, REPRO, MACRO
  };

  const std::map<Type, std::string> TYPE_TO_STRING = {
    {Type::FILTER, "filter"},
    {Type::DETECTOR, "detector" },
    {Type::MESSAGE, "message" },
    {Type::BROWSE, "browse" },
    {Type::SHELL, "shell" },
    {Type::SWITCH, "switch" },
    {Type::START_SESSION, "startsession" },
    {Type::REPRO, "repro" },
    {Type::MACRO, "macro" }
  };

  Type type;
  std::string name;
  bool deactivated;

  void updateGUI()
  {
    gui.group = new QGroupBox(("Command: " + name).c_str());
    gui.group->setLayout(new QVBoxLayout());
    {
      QHBoxLayout* lay = new QHBoxLayout();
      lay->addWidget(new QLabel("Type:"));
      QLineEdit* edit = new QLineEdit(TYPE_TO_STRING.at(type).c_str());
      edit->setReadOnly(true);
      lay->addWidget(edit);
      QCheckBox* deactive = new QCheckBox("Deactivated");
      deactive->setCheckState(deactivated ? Qt::Checked : Qt::Unchecked);
      lay->addWidget(deactive);
      QPushButton* del = new QPushButton("Delete");
      QPushButton* up = new QPushButton("Up");
      QPushButton* down = new QPushButton("Down");
      lay->addWidget(del);
      lay->addWidget(up);
      lay->addWidget(down);
      static_cast<QVBoxLayout*>(gui.group->layout())->addLayout(lay);
    }
    updateGUISub();
    gui.group->layout()->addWidget(widgetSub());
  }

  QWidget* widget() const { return gui.group; }

protected:
  virtual void updateGUISub() = 0;
  virtual QWidget* widgetSub() = 0;
  struct
  {
    QWidget* group;
  } gui;
};

class MacroCommandFilterDetector : public MacroCommandInfo
{
public:
  void updateGUISub() override
  {
    gui.group = new QWidget();
    gui.group->setLayout(new QVBoxLayout());

    QComboBox* mode = new QComboBox();
    mode->addItem("Configure");
    mode->addItem("Save");

    gui.group->layout()->addWidget(mode);

    QStackedWidget* stack = new QStackedWidget();
    {
      QWidget* widget = new QWidget();
      widget->setLayout(new QHBoxLayout());
      widget->layout()->addWidget(new QLabel("Time: "));
      gui.editTime = new QLineEdit();
      widget->layout()->addWidget(gui.editTime);

      stack->addWidget(widget);
    }
    {
      QWidget* widget = new QWidget();
      widget->setLayout(new QHBoxLayout());
      widget->layout()->addWidget(new QLabel("Parameter: "));
      gui.editParam = new QLineEdit();
      widget->layout()->addWidget(gui.editParam);

      stack->addWidget(widget);
    }
    gui.group->layout()->addWidget(stack);

    QObject::connect(mode, SIGNAL(activated(int)), stack, SLOT(setCurrentIndex(int)));
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
    QLineEdit* editTime;
    QLineEdit* editParam;
  } gui;
};

class MacroCommandMessage : public MacroCommandInfo
{
  void updateGUISub() override
  {
    gui.group = new QWidget();
    QVBoxLayout* lay = new QVBoxLayout();
    gui.group->setLayout(lay);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Title:"));
      gui.editTitle = new QLineEdit();
      sub->addWidget(gui.editTitle);

      lay->addLayout(sub);
    }
    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Timeout:"));
      gui.editTime = new QLineEdit();
      sub->addWidget(gui.editTime);

      lay->addLayout(sub);
    }
    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Text:"));
      gui.editText = new QTextEdit();
      sub->addWidget(gui.editText);

      lay->addLayout(sub);
    }
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
    QLineEdit* editTime;
    QLineEdit* editTitle;
    QTextEdit* editText;
  } gui;
};

class MacroCommandBrowse : public MacroCommandInfo
{
  void updateGUISub() override
  {
    gui.group = new QWidget();
    QHBoxLayout* lay = new QHBoxLayout();
    gui.group->setLayout(lay);

    lay->addWidget(new QLabel("Path:"));
    gui.editPath = new QLineEdit();
    lay->addWidget(gui.editPath);
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
    QLineEdit* editPath;
  } gui;
};

class MacroCommandShell : public MacroCommandInfo
{
  void updateGUISub() override
  {
    gui.group = new QWidget();
    QHBoxLayout* lay = new QHBoxLayout();
    gui.group->setLayout(lay);

    lay->addWidget(new QLabel("Command:"));
    gui.editCommand = new QTextEdit();
    lay->addWidget(gui.editCommand);
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
    QTextEdit* editCommand;
  } gui;
};

class MacroCommandSwitch : public MacroCommandInfo
{
  void updateGUISub() override
  {
    gui.group = new QWidget();
    QHBoxLayout* lay = new QHBoxLayout();
    gui.group->setLayout(lay);

    lay->addWidget(new QLabel("File:"));
    gui.editPath = new QLineEdit();
    lay->addWidget(gui.editPath);
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
    QLineEdit* editPath;
  } gui;
};

class MacroCommandStartSession : public MacroCommandInfo
{
  void updateGUISub() override
  {
    gui.group = new QWidget();
  }

  QWidget* widgetSub() override
  {
    return gui.group;
  }

private:
  struct
  {
    QWidget* group;
  } gui;
};

class MacroCommandParameter
{
public:
  void updateGUI()
  {
    gui.widget = new QGroupBox(QString::fromStdString("Parameter: ").append(name.c_str()));
    QVBoxLayout* layout = new QVBoxLayout();
    gui.widget->setLayout(layout);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Type: "));
      gui.editType = new QComboBox();
      gui.editType->addItem("Direct");
      gui.editType->addItem("Reference");
      gui.editType->addItem("Sequence");
      sub->addWidget(gui.editType);
      QPushButton* del = new QPushButton("Delete");
      sub->addWidget(del);

      layout->addLayout(sub);
    }
    {
      gui.stack = new QStackedWidget();
      {
        QWidget* sub = new QWidget;
        QHBoxLayout* subsub = new QHBoxLayout();
        sub->setLayout(subsub);
        subsub->addWidget(new QLabel("Name: "));
        gui.direct.name = new QLineEdit();
        subsub->addWidget(gui.direct.name);
        subsub->addWidget(new QLabel("Value: "));
        gui.direct.value = new QLineEdit();
        subsub->addWidget(gui.direct.value);
        subsub->addWidget(new QLabel("Unit: "));
        gui.direct.unit = new QLineEdit();
        subsub->addWidget(gui.direct.unit);

         gui.stack->addWidget(sub);
      }
      {
        QWidget* sub = new QWidget();
        sub->setLayout(new QHBoxLayout());
        sub->layout()->addWidget(new QLabel("Name: "));
        gui.reference.name = new QLineEdit();
        sub->layout()->addWidget(gui.reference.name);
        sub->layout()->addWidget(new QLabel("Parameter: "));
        gui.reference.value = new QComboBox();
        // XXX todo fill
        gui.reference.value->addItem("Reference 1");
        gui.reference.value->addItem("Reference 2");
        sub->layout()->addWidget(gui.reference.value);

        gui.stack->addWidget(sub);
      }
      {
        QWidget* sub = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout();
        sub->setLayout(lay);

        {
          QHBoxLayout* subsub = new QHBoxLayout();
          subsub->addWidget(new QLabel("Name: "));
          gui.sequence.name = new QLineEdit();
          subsub->addWidget(gui.sequence.name);
          subsub->addWidget(new QLabel("Unit: "));
          gui.sequence.unit = new QLineEdit();
          subsub->addWidget(gui.sequence.unit);

          lay->addLayout(subsub);
        }
        {
          QHBoxLayout* subsub = new QHBoxLayout();
          subsub->addWidget(new QLabel("Min:"));
          gui.sequence.min = new QSpinBox();
          subsub->addWidget(gui.sequence.min);
          subsub->addWidget(new QLabel("Max:"));
          gui.sequence.max = new QSpinBox();
          subsub->addWidget(gui.sequence.max);
          subsub->addWidget(new QLabel("Step:"));
          gui.sequence.step = new QSpinBox();
          subsub->addWidget(gui.sequence.step);
          lay->addLayout(subsub);
        }
        {
          QHBoxLayout* subsub = new QHBoxLayout();
          subsub->addWidget(new QLabel("Resolution:"));
          gui.sequence.resolution = new QSpinBox();
          subsub->addWidget(gui.sequence.resolution);
          subsub->addWidget(new QLabel("Mode:"));
          gui.sequence.mode = new QComboBox();
          gui.sequence.mode->addItem("up");
          gui.sequence.mode->addItem("down");
          gui.sequence.mode->addItem("random");
          subsub->addWidget(gui.sequence.mode);
          lay->addLayout(subsub);
        }

        gui.stack->addWidget(sub);
      }
      layout->addWidget(gui.stack);
    }
    QObject::connect(gui.editType, SIGNAL(activated(int)), gui.stack, SLOT(setCurrentIndex(int)));
  }

  QWidget* widget() const { return gui.widget; }

  std::string name;
  struct
  {
    QWidget* widget;
    QComboBox* editType;
    QStackedWidget* stack;
    struct
    {
      QLineEdit* name;
      QLineEdit* unit;
      QLineEdit* value;
    } direct;
    struct
    {
      QLineEdit* name;
      QComboBox* value;
    } reference;
    struct
    {
      QLineEdit* name;
      QLineEdit* unit;
      QSpinBox* min;
      QSpinBox* max;
      QSpinBox* step;
      QSpinBox* resolution;
      QComboBox* mode;
    } sequence;

  } gui;
};

class MacroCommandRepro : public MacroCommandInfo
{
public:
  void updateGUISub()
  {
    gui.widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    gui.widget->setLayout(layout);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("RePro: "));
      gui.editRepro = new QComboBox();
      for (const std::string& repro : repros)
        gui.editRepro->addItem(QString::fromStdString(repro));
      sub->addWidget(gui.editRepro);

      layout->addLayout(sub);
    }
    {
      QGroupBox* sub = new QGroupBox("Parameter");
      sub->setLayout(new QVBoxLayout());

      for (MacroCommandParameter& param : parameter)
      {
        param.updateGUI();
        sub->layout()->addWidget(param.widget());
      }

      layout->addWidget(sub);
    }
  }

  QWidget* widgetSub() override { return gui.widget; }

  std::vector<std::string> repros;
  std::vector<MacroCommandParameter> parameter;

  struct
  {
    QWidget* widget;
    QComboBox* editRepro;
  } gui;
};

class MacroCommandMacro : public MacroCommandInfo
{
public:
  void updateGUISub()
  {
    gui.widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    gui.widget->setLayout(layout);

    {
      QHBoxLayout* sub = new QHBoxLayout();
      sub->addWidget(new QLabel("Macro: "));
      gui.editMacro = new QComboBox();
      for (const std::string& macro : macros)
        gui.editMacro->addItem(QString::fromStdString(macro));
      sub->addWidget(gui.editMacro);

      layout->addLayout(sub);
    }
    {
      QGroupBox* sub = new QGroupBox("Parameter");
      sub->setLayout(new QVBoxLayout());

      for (MacroCommandParameter& param : parameter)
      {
        param.updateGUI();
        sub->layout()->addWidget(param.widget());
      }

      layout->addWidget(sub);
    }
  }

  QWidget* widgetSub() override { return gui.widget; }

  std::vector<std::string> macros;
  std::vector<MacroCommandParameter> parameter;

  struct
  {
    QWidget* widget;
    QComboBox* editMacro;
  } gui;
};

class MacroInfoNew
{
public:
  enum class Keyword
  {
    STARTUP, SHUTDOWN,STARTSESSION,STOPSESSION,FALLBACK, NOKEY, NOBUTTON, NOMENU, KEEP, OVERWRITE
  };

  const std::map<Keyword, std::string> KEYWORD_TO_STRING = {
    {Keyword::STARTUP, "startup"},
    {Keyword::SHUTDOWN, "shutdown"},
    {Keyword::STARTSESSION, "startsession"},
    {Keyword::STOPSESSION, "stopsession"},
    {Keyword::FALLBACK, "fallback"},
    {Keyword::NOKEY, "nokey"},
    {Keyword::NOBUTTON, "nobutton"},
    {Keyword::NOMENU, "nomenu"},
    {Keyword::KEEP, "keep"},
    {Keyword::OVERWRITE, "overwrite"},
  };

  void updateGUI()
  {
    gui.group = new QGroupBox(("Macro: " + name).c_str());
    gui.group->setLayout(new QVBoxLayout());

    {
      gui.param = new QGroupBox("Parameter");
      gui.param->setLayout(new QVBoxLayout());

      QPushButton* add = new QPushButton("Add");
      gui.param->layout()->addWidget(add);

      for (MacroParameter& param : parameter)
      {
        param.updateGUI();
        gui.param->layout()->addWidget(param.widget());
      }

      gui.group->layout()->addWidget(gui.param);
    }
    {
      QGroupBox* grp = new QGroupBox("Keywords");
      QHBoxLayout* lay = new QHBoxLayout();
      grp->setLayout(lay);
      for (const std::pair<Keyword, std::string>& key : KEYWORD_TO_STRING)
      {
        QCheckBox* box = new QCheckBox(QString::fromStdString(key.second));
        box->setCheckState((keywords.count(key.first) > 0) ? Qt::Checked : Qt::Unchecked);
        lay->addWidget(box);
      }
      gui.group->layout()->addWidget(grp);
    }
    {
      gui.comm = new QGroupBox("Commands");
      gui.comm->setLayout(new QVBoxLayout());

      QPushButton* add = new QPushButton("Add");
      gui.comm->layout()->addWidget(add);

      QWidget* container = new QWidget();
      container->setLayout(new QVBoxLayout());
      for (MacroCommandInfo* comm : commands)
      {
        comm->updateGUI();
        container->layout()->addWidget(comm->widget());
      }

      QScrollArea* scroll = new QScrollArea();
      scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      scroll->setWidget(container);
      gui.comm->layout()->addWidget(scroll);
      gui.group->layout()->addWidget(gui.comm);
    }
  }

  std::string name;
  std::set<Keyword> keywords;
  std::vector<MacroParameter> parameter;
  std::vector<MacroCommandInfo*> commands;

  struct
  {
    QGroupBox* group = nullptr;
    QGroupBox* param = nullptr;
    QGroupBox* comm = nullptr;
  } gui;
};

}

MacroEditor::MacroEditor(Macros* macros, QWidget *parent) :
  QWidget(parent),
  InternalMacros(macros)
{
  setLayout(new QVBoxLayout());

  {
    QGroupBox* group = new QGroupBox("Macro files", this);
    group->setLayout(new QHBoxLayout());

    FileSelector = new QComboBox();
    QPushButton* add = new QPushButton("Add");
    QPushButton* del = new QPushButton("Delete");

    QObject::connect(FileSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(setMacroFile(QString)));

    group->layout()->addWidget(FileSelector);
    group->layout()->addWidget(add);
    group->layout()->addWidget(del);

    this->layout()->addWidget(group);
  }
  {
    QGroupBox* group = new QGroupBox("Macros", this);
    group->setLayout(new QVBoxLayout());

    {
      QHBoxLayout* child = new QHBoxLayout();

      MacroSelector = new QComboBox();
      QPushButton* add = new QPushButton("Add");
      QPushButton* del = new QPushButton("Delete");

      child->addWidget(MacroSelector);
      child->addWidget(add);
      child->addWidget(del);

      dynamic_cast<QVBoxLayout*>(group->layout())->addLayout(child);
    }
    {
      MacroInfoPages = new QStackedWidget();
      group->layout()->addWidget(MacroInfoPages);
    }

    QObject::connect(MacroSelector, SIGNAL(activated(int)), MacroInfoPages, SLOT(setCurrentIndex(int)));


    this->layout()->addWidget(group);
  }

  std::vector<std::string> filenames;
  InternalMacros->texts("file", filenames);
  for (const std::string& filename : filenames)
    FileSelector->addItem(QString::fromStdString(filename));
  FileSelector->setCurrentIndex(FileSelector->findText("macros.cfg"));
}

void MacroEditor::dialogClosed(int code)
{
  if (code != 1)
    delete this;
}

void MacroEditor::delMacroFile()
{

}

void MacroEditor::addMacroFile()
{

}

void MacroEditor::setMacroFile(const QString& filename)
{
  MacroInfoNew macro;
  macro.name = "Test";
  MacroParameter param;
  param.param.setName("param1");
  param.param.setText("10");
  param.param.setUnit("V");
  macro.parameter.push_back(param);
  MacroParameter param2;
  param2.param.setName("param2");
  param2.param.setText("5");
  param2.param.setUnit("");
  macro.parameter.push_back(param2);

  MacroCommandInfo* com = new MacroCommandFilterDetector();
  com->type = MacroCommandInfo::Type::FILTER;
  com->name = "Test Filter";
  macro.commands.push_back(com);

  com = new MacroCommandFilterDetector();
  com->type = MacroCommandInfo::Type::DETECTOR;
  com->name = "Test Detector";
  macro.commands.push_back(com);

  com = new MacroCommandMessage();
  com->type = MacroCommandInfo::Type::MESSAGE;
  com->name = "Test Message";
  macro.commands.push_back(com);

  com = new MacroCommandBrowse();
  com->type = MacroCommandInfo::Type::BROWSE;
  com->name = "Test Browse";
  macro.commands.push_back(com);

  com = new MacroCommandShell();
  com->type = MacroCommandInfo::Type::SHELL;
  com->name = "Test Shell";
  macro.commands.push_back(com);

  com = new MacroCommandSwitch();
  com->type = MacroCommandInfo::Type::SWITCH;
  com->name = "Test Switch";
  macro.commands.push_back(com);

  com = new MacroCommandStartSession();
  com->type = MacroCommandInfo::Type::START_SESSION;
  com->name = "Test Startsession";
  macro.commands.push_back(com);

  MacroCommandRepro* re = new MacroCommandRepro();
  re->type = MacroCommandInfo::Type::REPRO;
  re->name = "Test RePro";
  re->repros.push_back("Repro Test 1");
  re->repros.push_back("Repro Test 2");
  re->repros.push_back("Repro Test 3");
  MacroCommandParameter p1;
  p1.name = "Param 1";
  re->parameter.push_back(p1);
  MacroCommandParameter p2;
  p2.name = "Param 2";
  re->parameter.push_back(p2);
  macro.commands.push_back(re);

  MacroCommandMacro* ma = new MacroCommandMacro();
  ma->type = MacroCommandInfo::Type::MACRO;
  ma->name = "Test Macro";
  ma->macros.push_back("Macro Test 1");
  ma->macros.push_back("Macro Test 2");
  MacroCommandParameter m1;
  m1.name = "Param 1";
  ma->parameter.push_back(m1);
  macro.commands.push_back(ma);

  macro.updateGUI();

  MacroSelector->addItem(macro.name.c_str());
  MacroInfoPages->addWidget(macro.gui.group);

  /*
  MacroFileLoader loader(filename.toStdString());
  loader.load();

  MacroSelector->clear();
  while (MacroInfoPages->count())
    MacroInfoPages->removeWidget(MacroInfoPages->widget(0));
  for (const MacroInfo& macro : loader.macros())
  {
    MacroSelector->addItem(QString::fromStdString(macro.name));
    {
      QGroupBox* box = new QGroupBox(QString::fromStdString("Macro: " + macro.name));
      box->setLayout(new QVBoxLayout());

      {
        QGroupBox* grpkey = new QGroupBox("Keywords");
        grpkey->setLayout(new QHBoxLayout());

        for (const std::pair<std::string, int> keyword : MACRO_KEYWORDS)
        {
          QCheckBox* check = new QCheckBox(QString::fromStdString(keyword.first));
          check->setCheckState((macro.keywords & keyword.second) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

          grpkey->layout()->addWidget(check);
        }

        box->layout()->addWidget(grpkey);
      }
      {
        QGroupBox* grpparam = new QGroupBox("Parameter");
        grpparam->setLayout(new QVBoxLayout());

        for (const Parameter& param : macro.parameter)
        {
          QHBoxLayout* lay = new QHBoxLayout();
          lay->addWidget(new QLineEdit(param.name().c_str()));
          lay->addWidget(new QLineEdit(param.text().c_str()));
          lay->addWidget(new QLineEdit(param.unit().c_str()));
          static_cast<QVBoxLayout*>(grpparam->layout())->addLayout(lay);
        }

        box->layout()->addWidget(grpparam);
      }
      {
        QGroupBox* actions = new QGroupBox("Actions");
        box->layout()->addWidget(actions);
      }

      MacroInfoPages->addWidget(box);
    }
  }
  */
}





MacroFileLoader::MacroFileLoader(const string &filename) :
  Filename(filename)
{

}

void MacroFileLoader::load()
{
  std::ifstream in(Filename);

  std::string tmp;

  while (std::getline(in, tmp))
  {
    Str line = Str(tmp);
    line.strip(Str::WhiteSpace, "#");

    // line was a comment or empty
    if (line.empty())
      continue;

    if (line.front() == '$')
    {
      //line = readMacro(in, line);
      MacroInfo macro{};
      int seperator = line.find_first_of(":");
      Str nameandkeys = line.substr(1, seperator - 1);

      for (const std::pair<std::string, int>& keyword : MACRO_KEYWORDS)
        if (nameandkeys.erase(keyword.first, 0, false, 3, Str::WordSpace) > 0)
          macro.keywords |= keyword.second;

      macro.name = nameandkeys.strip();

      if (seperator != std::string::npos)
      {
        Str params = line.substr(seperator + 1);
        macro.parameter.load(params, "=", ";");
      }

      Macros.emplace_back(std::move(macro));
    }

    if (Str(line).contains('=') && !Str(line).contains(':'))
    {
      std::cout << "lol found ya:" << line << std::endl;
      Macros.back().parameter.load( line, "=", ";" );
    }

  }
}


Str MacroFileLoader::readMacro(std::ifstream& file, const Str& line)
{
  MacroInfo macro{};

  // extract definition
  {
    size_t seperator = line.find_first_of(":");
    Str nameandkeys = line.substr(1, seperator - 1);

    for (const std::pair<std::string, int>& keyword : MACRO_KEYWORDS)
      if (nameandkeys.erase(keyword.first, 0, false, 3, Str::WordSpace) > 0)
        macro.keywords |= keyword.second;

    macro.name = nameandkeys.strip();

    if (seperator != std::string::npos)
    {
      Str params = line.substr(seperator + 1);
      macro.parameter.load(params, "=", ";");
    }
  }

  // read contents
  {
    /*
    std::string tmp;
    while (std::getline(file, tmp))
    {
      line = Str(tmp);
      line.strip(Str::WhiteSpace, "#");

      if (line.empty())
        continue;

      if (line[0] == '$')
        return line;

      if (line.contains("=") && !line.contains(":"))
        macro.parameter.load(line, "=", ";");


    }
    */
  }

}

}

#include <moc_macroeditor.cc>
