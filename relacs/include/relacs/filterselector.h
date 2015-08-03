/*
  filterselector.h
  Dialog to allow configuration of available and active filters/detectors

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

#ifndef FILTERSELECTOR_H
#define FILTERSELECTOR_H

#include <QWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <relacs/options.h>
#include <relacs/inlist.h>
#include <relacs/filterdetectors.h>

namespace relacs {

template<typename T>
class TreeWrapper
{
public:
  class Category
  {
  public:
    void add(const std::string& name)
    {
      auto w = new QTreeWidgetItem(Item);
      w->setText(0, name.c_str());
    }

  public:
    QTreeWidgetItem* Item;
    T Data;
  };

public:
  TreeWrapper(const std::initializer_list<QString>& headings)
  {
    Widget = new QTreeWidget();
    Widget->setColumnCount(headings.size());
    Widget->setHeaderLabels(headings);
  }

  Category& addCategory(const std::initializer_list<QString>& texts, const T& data = {})
  {
    Category category;
    category.Item = new QTreeWidgetItem(Widget, texts);
    category.Data = data;

    auto itr = Categories.insert(std::make_pair(category.Item, category)).first;

    Widget->expandItem(category.Item);
    for (int i = 0; i < Widget->columnCount(); ++i)
      Widget->resizeColumnToContents(i);

    return (*itr).second;
  }

public:
  QTreeWidget* Widget;
  std::map<QTreeWidgetItem*, Category> Categories;
};

struct ActiveFilterData
{
  Options* Source;
  Options Current;
};
struct DummyData {};

class FilterSelector : public QWidget
{
  Q_OBJECT
public:
  FilterSelector(QWidget* parent = nullptr);

  void setInputTraces(Options& inList);
  void setFilters(FilterDetectors* filters);
  void setAvailableFilters();

public slots:
  void dialogClosed(int code);

signals:
  void newFilterSettings();

private slots:
  void editFilter();
  void addNewFilter();
  void deleteFilter();

private:
  void openEditFilterDialog(TreeWrapper<ActiveFilterData>::Category& category);
  void openAddFilterDialog(TreeWrapper<DummyData>::Category& category);

private:
  TreeWrapper<DummyData> AvailableFilters;
  TreeWrapper<ActiveFilterData> ActiveFilters;
  TreeWrapper<DummyData> AvailableInputs;

  FilterDetectors* FilterList;
  std::vector<int> DeleteList;
};

void assignGeneralFilterOptions(Options* options);

}

#endif // FILTERSELECTOR_H

