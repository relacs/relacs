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

/*! Helper class to allow easy control of tree views */
template<typename T>
class TreeWrapper
{
public:
  /*! Represents a top level item in an tree*/
  class Category
  {
  public:
    /*! adds a sub item */
    void add(const std::string& name)
    {
      auto w = new QTreeWidgetItem(Item);
      w->setText(0, name.c_str());
    }

  public:
    QTreeWidgetItem* Item;  ///< tree widget item
    T Data;                 ///< arbitrary data associated with this item
  };

public:
  /*! Constructs a new tree view with given headings */
  TreeWrapper(const std::initializer_list<QString>& headings)
  {
    Widget = new QTreeWidget();
    Widget->setColumnCount(headings.size());
    Widget->setHeaderLabels(headings);
  }

  /*! Adds a category with optionally data */
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
  QTreeWidget* Widget;                              ///< root widget
  std::map<QTreeWidgetItem*, Category> Categories;  ///< stores interal structure
};

/*! Holds options for active filters */
struct ActiveFilterData
{
  Options* Source;
  Options Current;
};
/*! Dummy for no data in tree */
struct DummyData {};

/*!
\class FilterSelector
\brief Dialog to allow configuration of available and active filters/detectors
\author Philipp Kuhlmann

All available filters/detectors are extracted through loaded plugins.
Active devices are loaded from FilterDetectors list (indirectely out of config).

All configureable options are defined here, other options aren't displayed in the dialogs and aren't saved.
*/
class FilterSelector : public QWidget
{
  Q_OBJECT
public:
  FilterSelector(QWidget* parent = nullptr);

  /*! Loads available input traces (analog/digital and events) */
  void setInputTraces(Options& deviceIn);
  /*! Loads active filters */
  void setFilters(FilterDetectors* filters);
  /*! Loads available filters */
  void setAvailableFilters();

public slots:
  /*! Called when dialog is closed, triggers saving */
  void dialogClosed(int code);

signals:
  /*! Called when options were changed, allows reloading filters */
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
}

#endif // FILTERSELECTOR_H

