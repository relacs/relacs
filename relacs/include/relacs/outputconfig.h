#ifndef OUTPUTCONFIG_H
#define OUTPUTCONFIG_H
 
#include <QtGui>
#include<QStringList>
#include<QTableWidget>
#include<QHBoxLayout>
#include<QPushButton>

using namespace std;


namespace relacs {

class OutputConfig : public QWidget
{
    Q_OBJECT
 
public:
  OutputConfig(QWidget *parent = 0);
  ~OutputConfig();
  void fillCells(int, bool);
  
public slots:
  void addRow();
  void deleteRow();
  void accept();
 
private:
  QTableWidget *widget;
  QHBoxLayout *layout;
  QVBoxLayout *buttonlayout;
  QStringList list;
  QPushButton *addButton;
  QPushButton *removeButton;
  QPushButton *acceptButton;
};


}; /* namespace relacs */
 
#endif // OUTPUTCONFIG_H
