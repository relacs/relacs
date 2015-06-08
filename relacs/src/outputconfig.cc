/*
  outputconfig.cc
  Configures analog output traces.

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

#include <relacs/outputconfig.h>
#include <QtGui>
#include <QTableWidget>
#include <QStringList>
#include <QPushButton>
#include <iostream>

#include <relacs/doublespinbox.h>

namespace relacs {

const std::string OutputConfig::OptionNames::GROUP_NAME = "output data";
const std::string OutputConfig::OptionNames::ID         = "outputtraceid";
const std::string OutputConfig::OptionNames::DEVICE     = "outputtracedevice";
const std::string OutputConfig::OptionNames::CHANNEL    = "outputtracechannel";
const std::string OutputConfig::OptionNames::SCALE      = "outputtracescale";
const std::string OutputConfig::OptionNames::UNIT       = "outputtraceunit";
const std::string OutputConfig::OptionNames::MAX_RATE   = "outputtracemaxrate";
const std::string OutputConfig::OptionNames::MODALITY   = "outputtracemodality";

OutputConfig::OutputConfig(Options& ops, QWidget *parent)
  : QWidget(parent),
    OutputOptions(ops)
{
  TableWidget = new QTableWidget(this);
  TableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
  initTable();

  // explicitly set table size to show all columns
  int w = TableWidget->verticalHeader()->width() + TableWidget->frameWidth()*2+40;
  for ( int i = 0; i < TableWidget->columnCount(); i++ )
    w += TableWidget->columnWidth( i );
  TableWidget->setMinimumWidth( w );
  TableWidget->setMinimumHeight( 8*TableWidget->rowHeight( 0 ) );
  TableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );

  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(TableWidget);

  QVBoxLayout* buttons = new QVBoxLayout();
  layout->addLayout(buttons);

  QPushButton* button = new QPushButton("&Insert", this);
  buttons->addWidget(button);
  QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(insertRow()));
  button = new QPushButton("&Erase", this);
  buttons->addWidget(button);
  QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(deleteRows()));
  button = new QPushButton("&Fill", this);
  buttons->addWidget(button);
  QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(copyRow()));

  setLayout(layout);
}

void OutputConfig::initTable()
{
  QStringList headers;
  headers << "Name" << "Device" << "Channel" << "Scale" << "Unit" << "Max rate" << "Modality";

  TableWidget->setColumnCount(headers.count());
  TableWidget->setHorizontalHeaderLabels(headers);

  int count = OutputOptions.size(OptionNames::ID);
  TableWidget->setRowCount(count);

  for (int i = 0; i < count; ++i)
  {
    RowData data;

    data.name = OutputOptions.text(OptionNames::ID, i, "");
    data.device = OutputOptions.text(OptionNames::DEVICE, i, "");
    data.channel = OutputOptions.integer(OptionNames::CHANNEL, i, 0);
    data.scale = OutputOptions.number(OptionNames::SCALE, i, 1.f);
    data.unit = OutputOptions.text(OptionNames::UNIT, i, "");
    data.maxRate = OutputOptions.integer(OptionNames::MAX_RATE, i, "") / 1000;
    data.modality = OutputOptions.text(OptionNames::MODALITY, i, "");

    addRow(i, data);
  }
}

void OutputConfig::addRow(int row, const RowData& data)
{
  TableWidget->setItem(row, 0, new QTableWidgetItem(data.name.c_str()));
  TableWidget->setItem(row, 1, new QTableWidgetItem(data.device.c_str()));

  QSpinBox* channelBox = new QSpinBox();
  channelBox->setRange(0, 1024);
  channelBox->setValue(data.channel);
  TableWidget->setCellWidget(row, 2, channelBox);

  DoubleSpinBox* scaleBox = new DoubleSpinBox();
  scaleBox->setFormat("%g");
  scaleBox->setRange(-10000000.0, 10000000.0);
  scaleBox->setSingleStep(0.1);
  scaleBox->setValue(data.scale);
  TableWidget->setCellWidget(row, 3, scaleBox);

  QComboBox* unitbox = new QComboBox;
  unitbox->setEditable( true );
  unitbox->addItem( "V" );
  unitbox->addItem( "mV" );
  unitbox->addItem( "uV" );
  unitbox->addItem( "kV" );
  unitbox->addItem( "A" );
  unitbox->addItem( "mA" );
  unitbox->addItem( "uA" );
  unitbox->addItem( "nA" );
  unitbox->addItem( "pA" );
  unitbox->addItem( "kA" );
  for (int i = 0; i < unitbox->count(); ++i)
    if (unitbox->itemText(i) == QString::fromStdString(data.unit))
    {
      unitbox->setCurrentIndex(i);
      break;
    }
  TableWidget->setCellWidget( row, 4, unitbox );

  QSpinBox* maxRateSpin = new QSpinBox();
  maxRateSpin->setRange(0.0, 10000000.0);
  maxRateSpin->setValue(data.maxRate);
  maxRateSpin->setSuffix("kHz");
  TableWidget->setCellWidget(row, 5, maxRateSpin);

  QComboBox* modalityCombo = new QComboBox();
  modalityCombo->addItem("current");
  modalityCombo->addItem("voltage");
  modalityCombo->addItem("electric");
  modalityCombo->addItem("acoustic");
  for (int i = 0; i < modalityCombo->count(); ++i)
    if (modalityCombo->itemText(i) == QString::fromStdString(data.modality))
    {
      modalityCombo->setCurrentIndex(i);
      break;
    }
  TableWidget->setCellWidget(row, 6, modalityCombo);
}

void OutputConfig::setRow(int row, const RowData& data)
{
  TableWidget->item(row, 0)->setText(data.name.c_str());
  TableWidget->item(row, 1)->setText(data.device.c_str());
  dynamic_cast<QSpinBox*>(TableWidget->cellWidget(row, 2))->setValue(data.channel);
  dynamic_cast<DoubleSpinBox*>(TableWidget->cellWidget(row, 3))->setValue(data.scale);

  QComboBox* box = dynamic_cast<QComboBox*>(TableWidget->cellWidget(row, 4));
  for (int i = 0; i < box->count(); ++i)
    if (box->itemText(i) == QString::fromStdString(data.unit))
      box->setCurrentIndex(i);

  dynamic_cast<QSpinBox*>(TableWidget->cellWidget(row, 5))->setValue(data.maxRate);

  box = dynamic_cast<QComboBox*>(TableWidget->cellWidget(row, 6));
  for (int i = 0; i < box->count(); ++i)
    if (box->itemText(i) == QString::fromStdString(data.modality))
      box->setCurrentIndex(i);
}

OutputConfig::RowData OutputConfig::getRow(int row) const
{
  RowData data;

  data.name = TableWidget->item(row, 0)->text().toStdString();
  data.device = TableWidget->item(row, 1)->text().toStdString();
  data.channel = dynamic_cast<QSpinBox*>(TableWidget->cellWidget(row, 2))->value();
  data.scale = dynamic_cast<DoubleSpinBox*>(TableWidget->cellWidget(row, 3))->value();
  data.unit = dynamic_cast<QComboBox*>(TableWidget->cellWidget(row, 4))->currentText().toStdString();
  data.maxRate = dynamic_cast<QSpinBox*>(TableWidget->cellWidget(row, 5))->value();
  data.modality = dynamic_cast<QComboBox*>(TableWidget->cellWidget(row, 6))->currentText().toStdString();

  return data;
}

void OutputConfig::deleteRows()
{
  QList<QTableWidgetSelectionRange> selections = TableWidget->selectedRanges();
  if (selections.empty() || selections.front().columnCount() < 7)
    return;

  for (int i = 0; i < selections.front().rowCount(); ++i)
    TableWidget->removeRow(selections.front().topRow());
}

void OutputConfig::adjustUniqueOptions(RowData& data, int excludeBegin, int excludeEnd)
{
  for (int i = 0; i < TableWidget->rowCount(); ++i)
  {
    if (i >= excludeBegin && i <= excludeEnd)
      continue;

    RowData existing = getRow(i);

    if (existing.name == data.name)
    {
      // name = (name without suffix)-(suffix + 1)
      data.name = data.name.substr(0, data.name.find_last_of("-") + 1)
                  + Str(static_cast<int>(Str(existing.name.substr(existing.name.find_last_of("-") + 1)).number(0.0)) + 1);
    }

    if (existing.device == data.device)
    {
      if (existing.channel == data.channel)
        ++data.channel;
    }
  }
}

void OutputConfig::insertRow()
{
  int index = TableWidget->rowCount() - 1;
  QList<QTableWidgetSelectionRange> selections = TableWidget->selectedRanges();
  if (!selections.empty() && selections.front().columnCount() == 7)
    index = selections.front().bottomRow();

  RowData data;
  if (index >= 0)
    data = getRow(index);
  else
  {
    data.name = "Output-" + Str(TableWidget->rowCount() + 1);
    data.device = "ao-1";
    data.channel = 0;
    data.scale = 1.0;
    data.unit = "V";
    data.maxRate = 0;
    data.modality = "voltage";
  }

  adjustUniqueOptions(data);

  TableWidget->insertRow(index + 1);
  addRow(index + 1, data);
}

void OutputConfig::copyRow()
{
  QList<QTableWidgetSelectionRange> selection = TableWidget->selectedRanges();
  if ( selection.empty() || selection.front().rowCount() <= 1 || selection.front().columnCount() < 1 )
    return;

  RowData reference = getRow(selection.front().topRow());
  for (int row = selection.front().topRow() + 1; row <= selection.front().bottomRow(); ++row)
  {
    adjustUniqueOptions(reference, row, selection.front().bottomRow());
    setRow(row, reference);
  }
}

void OutputConfig::dialogClosed( int code )
{
  if (code < CODE_APPLY || TableWidget->rowCount() == 0)
  {
    if (code != CODE_APPLY)
      delete this;
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  Parameter* id = NULL;
  Parameter* device = NULL;
  Parameter* channel = NULL;
  Parameter* scale = NULL;
  Parameter* unit = NULL;
  Parameter* rate = NULL;
  Parameter* modality = NULL;

  for (int i = 0; i < TableWidget->rowCount(); ++i)
  {
    RowData data = getRow(i);
    if (i == 0)
    {
      id = &OutputOptions.setText(OptionNames::ID, data.name);
      device = &OutputOptions.setText(OptionNames::DEVICE, data.device);
      channel = &OutputOptions.setInteger(OptionNames::CHANNEL, data.channel);
      scale = &OutputOptions.setNumber(OptionNames::SCALE, data.scale);
      unit = &OutputOptions.setText(OptionNames::UNIT, data.unit);
      rate = &OutputOptions.setInteger(OptionNames::MAX_RATE, data.maxRate * 1000);
      modality = &OutputOptions.setText(OptionNames::MODALITY, data.modality);
    }
    else
    {
      id->addText(data.name);
      device->addText(data.device);
      channel->addInteger(data.channel);
      scale->addNumber(data.scale);
      unit->addText(data.unit);
      rate->addInteger(data.maxRate * 1000);
      modality->addText(data.modality);
    }
  }

  emit newOutputSettings();

  QApplication::restoreOverrideCursor();

  if (code != CODE_APPLY)
    delete this;
}

} /* namespace relacs */


#include "moc_outputconfig.cc"
