#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H
 
#include <QtGui>
#include<QStringList>
#include<QTableWidget>
#include<QHBoxLayout>
#include<QPushButton>

using namespace std;


namespace relacs {

class InputConfig : public QWidget
{
    Q_OBJECT
 
public:
  InputConfig(QWidget *parent = 0);
  ~InputConfig();
  void fillCells(int, bool);
  
public slots:
  void addRow();
  void deleteRow();
  void accept();
 
private:
  QTableWidget *Widget;
  QHBoxLayout *Layout;
  QVBoxLayout *Buttonlayout;
  QStringList List;
  QPushButton *AddButton;
  QPushButton *RemoveButton;
  QPushButton *AcceptButton;
  QItemSelectionModel * Selection;
  QModelIndexList Indexes;
};


}; /* namespace relacs */
 
#endif // INPUTCONFIG_H
