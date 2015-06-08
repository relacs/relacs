/*
  outputconfig.h
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

#ifndef OUTPUTCONFIG_H
#define OUTPUTCONFIG_H

#include <QtGui>
#include <QStringList>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include <relacs/options.h>

namespace relacs {

/*!
 * \class OutputConfig
 * \brief Configures analog output traces
 * \author Philipp Kuhlmann
 */
class OutputConfig : public QWidget
{
  Q_OBJECT

public:
  /*! Dialog button control codes */
  enum Codes
  {
    CODE_APPLY = 1,
    CODE_OK = 2,
  };

  /*! Stores mapped configuration options */
  struct OptionNames
  {
    static const std::string GROUP_NAME;

    static const std::string ID;
    static const std::string DEVICE;
    static const std::string CHANNEL;
    static const std::string SCALE;
    static const std::string UNIT;
    static const std::string MAX_RATE;
    static const std::string MODALITY;
  };

private:
  /*! Represents a table row and its option values */
  struct RowData
  {
    std::string name;
    std::string device;
    int         channel;
    double      scale;
    std::string unit;
    int         maxRate;
    std::string modality;
  };

public:
  /*! Constructs Output configurations dialog depending on current options*/
  OutputConfig(Options& ops, QWidget *parent = 0);

private slots:
  /*! Inserts a new cofiguration row*/
  void insertRow();
  /*! Deletes currently selected rows*/
  void deleteRows();
  /*! Copys options from first row to all other selected rows */
  void copyRow();

public slots:
  /*!
     * \brief Dialog callback, handles saving
     * \see Codes
    */
  void dialogClosed( int code );

signals:
  /*! Signaled when settings were changed */
  void newOutputSettings();

private:
  /*! Creates table and populates it with current configuration */
  void initTable();
  /*! Adds a concrete row to the table */
  void addRow(int row, const RowData& data);
  /*! Get options for row */
  RowData getRow(int row) const;
  /*! Sets row data */
  void setRow(int row, const RowData& data);
  /*! Checks if unique fields are duplicated and attempts to dedulicate them */
  void adjustUniqueOptions(RowData& data, int excludeBegin = 1000, int excludeEnd = -1000);

private:
  Options& OutputOptions;
  QTableWidget* TableWidget;
};


} /* namespace relacs */

#endif // OUTPUTCONFIG_H
