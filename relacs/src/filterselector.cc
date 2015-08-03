/*
  filterselector.cc
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

#include <list>
#include <QString>
#include <QListWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <relacs/filter.h>
#include <relacs/filterselector.h>

namespace relacs {

namespace {

/*! Defines all managed options */
Options initTemplate()
{
  Options opt;
  opt.addText("name");
  opt.addText("filter");
  opt.addBoolean("save", false);
  opt.addBoolean("savemeanquality", false);
  opt.addBoolean("plot", true);
  opt.addBoolean("trigger", false);
  opt.addBoolean("center", false);
  opt.addText("inputtrace").setStyle(Parameter::MultipleSelection);
  opt.addText("othertrace").setStyle(Parameter::MultipleSelection);
  opt.addInteger("buffersize");
  opt.addBoolean("storesize", false);
  opt.addBoolean("storewidth", false);
  opt.addText("panel");
  opt.addInteger("linewidth");
  return std::move(opt);
}

static const Options OPTION_TEMPLATE = initTemplate();

}

FilterSelector::FilterSelector(QWidget *parent) :
  QWidget(parent),
  AvailableFilters({"Name", "Plugin"}),
  ActiveFilters({"Name", "Plugin"}),
  AvailableInputs({"Name"}),
  FilterList(nullptr)
{
  QHBoxLayout* layout = new QHBoxLayout(parent);

  {
    QGroupBox* box = new QGroupBox("Available Filters");
    QVBoxLayout* lay = new QVBoxLayout(box);

    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* button = new QPushButton("Add");
    connect(button, SIGNAL(clicked()), this, SLOT(addNewFilter()));
    buttons->addWidget(button);

    lay->addWidget(AvailableFilters.Widget);
    lay->addLayout(buttons);
    layout->addWidget(box);
  }
  {
    QGroupBox* filterBox = new QGroupBox("Active Filters");
    QVBoxLayout* filterBoxLayout = new QVBoxLayout(filterBox);

    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* button = new QPushButton("Edit");
    connect(button, SIGNAL(clicked()), this, SLOT(editFilter()));
    buttons->addWidget(button);

    button = new QPushButton("Delete");
    connect(button, SIGNAL(clicked()), this, SLOT(deleteFilter()));
    buttons->addWidget(button);


    filterBoxLayout->addWidget(ActiveFilters.Widget);
    filterBoxLayout->addLayout(buttons);
    layout->addWidget(filterBox);
  }
  {
    QGroupBox* inputBox = new QGroupBox("Inputs");
    QVBoxLayout* inputBoxLayout = new QVBoxLayout(inputBox);

    inputBoxLayout->addWidget(AvailableInputs.Widget);
    layout->addWidget(inputBox);
  }

  setAvailableFilters();

  setLayout(layout);
  setMinimumSize(500,350);
}

void FilterSelector::setInputTraces(Options& inList)
{
  Parameter& param = *(inList.find("inputtraceid"));
  for (int i = 0; i < param.size(); ++i)
    AvailableInputs.addCategory({param.text(i).c_str()});
}

void FilterSelector::setFilters(FilterDetectors *filters)
{
  FilterList = filters;

  for (FilterData* filter : filters->filterList())
  {
    Filter* flt = filter->FilterDetector;

    // read options into local object
    Options opts = OPTION_TEMPLATE;
    opts.read(*filter->GeneralOptions);

    TreeWrapper<ActiveFilterData>::Category& category = ActiveFilters.addCategory({flt->ident().c_str(), flt->name().c_str()}, {filter->GeneralOptions, opts});
    for (const std::string& name : filter->In)
      category.add(name);
  }
}

void FilterSelector::setAvailableFilters()
{
  for (int i = 0; i < Plugins::size(); ++i)
    if (Plugins::type(i) & RELACSPlugin::FilterId)
    {
      // Structure: plugin[package]
      size_t idx = Plugins::ident(i).find('[');
      std::string name = Plugins::ident(i).substr(0, idx);
      std::string plugin = Plugins::ident(i).substr(idx + 1, Plugins::ident(i).size() - idx - 2);
      AvailableFilters.addCategory({QString::fromStdString(name), QString::fromStdString(plugin)});
    }
}

void FilterSelector::editFilter()
{
  QList<QTreeWidgetItem*> items = ActiveFilters.Widget->selectedItems();
  if (items.empty() || items.size() > 1)
    return;

  if (ActiveFilters.Categories.find(items.front()) == ActiveFilters.Categories.end())
    return;

  TreeWrapper<ActiveFilterData>::Category& categoy = ActiveFilters.Categories[items.front()];

  openEditFilterDialog(categoy);
}

void FilterSelector::openEditFilterDialog(TreeWrapper<ActiveFilterData>::Category& category)
{
  OptDialog* od = new OptDialog(true, this);
  od->setCaption("Device options");
  od->addOptions(category.Data.Current);
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "&Cancel" );
  int code = od->exec();
  if (code != 1)
    return;

  category.Item->setText(0, category.Data.Current.text("name").c_str());
}

void FilterSelector::addNewFilter()
{
  QList<QTreeWidgetItem*> items = AvailableFilters.Widget->selectedItems();
  if (items.empty() || items.size() > 1)
    return;

  TreeWrapper<DummyData>::Category& categoy = AvailableFilters.Categories[items.front()];

  openAddFilterDialog(categoy);
}

void FilterSelector::openAddFilterDialog(TreeWrapper<DummyData>::Category &category)
{
  Options copy = OPTION_TEMPLATE;
  copy.setText("filter", category.Item->text(0).toStdString());

  OptDialog* od = new OptDialog(true, this);
  od->setCaption("Device options");
  od->addOptions(copy);
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "&Cancel" );
  int code = od->exec();
  if (code != 1)
    return;

  ActiveFilters.addCategory({copy.text("name").c_str(), copy.text("filter").c_str()}, {nullptr, copy});

}

void FilterSelector::deleteFilter()
{
  QList<QTreeWidgetItem*> items = ActiveFilters.Widget->selectedItems();
  if (items.empty() || items.size() > 1)
    return;

  auto itr = ActiveFilters.Categories.find(items.front());
  if (itr == ActiveFilters.Categories.end())
    return;

  // save deleted id (e.g. Filter1 -> 1)
  if (itr->second.Data.Source)
    DeleteList.push_back(std::atoi(itr->second.Data.Source->name().substr(6).c_str()));

  delete items.front();
  ActiveFilters.Categories.erase(items.front());
}


void FilterSelector::dialogClosed(int code)
{
  if (code < 2)
  {
    delete this;
    return;
  }

  int count =  0;

  // save options, to allow sorted traversal
  std::vector<ActiveFilterData*> toSave;
  for (auto&& itr : ActiveFilters.Categories)
    toSave.push_back(&itr.second.Data);

  // sort by old filter id, new entries are appendned at the end
  std::sort(toSave.begin(), toSave.end(), [](ActiveFilterData* a, ActiveFilterData* b) {
    if (!a->Source)
      return false;
    if (!b->Source)
      return true;
    std::string a1 = a->Source->name().substr(6);
    std::string b1 = b->Source->name().substr(6);
    int cmp = a1.compare(b1);
    return cmp < 0;
  });

  for (int i : DeleteList)
    FilterList->erase(FilterList->findSection("Filter" + Str(i)));

  for (ActiveFilterData* data : toSave)
  {
    if (data->Source)
    {
      data->Source->clear();
      data->Source->addSection(data->Current, "Filter" + Str(++count));
    }
    else  // new item
    {
      FilterList->newSection(data->Current, "Filter" + Str(++count));
    }
  }

  emit newFilterSettings();

  if (code != 2)
    delete this;
}

}

#include "moc_filterselector.cc"
