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
#include <QStackedWidget>
#include <relacs/macros.h>

namespace relacs {

class MacroEditor : public QWidget
{
  Q_OBJECT

public:
  MacroEditor(Macros* macros, QWidget* parent = nullptr);

public slots:
  void dialogClosed( int code );

private slots:
  void addMacroFile();
  void delMacroFile();
  void setMacroFile(const QString& filename);

signals:
  void macroDefinitionsChanged();

private:
  Macros* InternalMacros;

  QComboBox* FileSelector;
  QComboBox* MacroSelector;
  QStackedWidget* MacroInfoPages;
};

struct MacroInfo
{
  std::string name;
  int keywords;
  Options parameter;
};

class MacroFileLoader
{
public:
  MacroFileLoader(const std::string& filename);

  void load();

  const std::vector<MacroInfo> macros() const { return Macros; }

private:
  Str readMacro(std::ifstream& file, const Str& line);

private:
  std::string Filename;

  std::vector<MacroInfo> Macros;
};

}
