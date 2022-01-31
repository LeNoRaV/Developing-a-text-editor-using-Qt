#include "exit.h"

ExitWindow::ExitWindow(QStringList* nameFiles, QWidget* parent):QDialog(parent){
    setWindowTitle(tr("Save edits"));
    setModal(true);
    QTableWidget* tableFiles=new QTableWidget(nameFiles->count(),2,this);
    for(int i=0;i<nameFiles->count();++i){
        if(nameFiles->at(i).indexOf(tr("New file"))!=-1){
            QTableWidgetItem* newItem1=new QTableWidgetItem(nameFiles->at(i));
            QTableWidgetItem* newItem2=new QTableWidgetItem(tr("No way"));
            tableFiles->setItem(i,0,newItem1);
            tableFiles->setItem(i,1,newItem2);
        }
        else{
            QFileInfo file(nameFiles->at(i));
            QTableWidgetItem* newItem1=new QTableWidgetItem(file.fileName());
            QTableWidgetItem* newItem2=new QTableWidgetItem(file.absolutePath());
            tableFiles->setItem(i,0,newItem1);
            tableFiles->setItem(i,1,newItem2);
        }
    }
    QPushButton* save = new QPushButton(tr("Save"),this);
    QPushButton* noSave = new QPushButton(tr("Not save"),this);

    QHBoxLayout* layout_buttons=new QHBoxLayout();
    layout_buttons->addWidget(save);
    layout_buttons->addWidget(noSave);

    QLabel* text=new QLabel(this);
    text->setText(tr("Unsaved files:"));

    QVBoxLayout* layout_result=new QVBoxLayout();
    layout_result->addWidget(text);
    layout_result->addWidget(tableFiles);
    layout_result->addLayout(layout_buttons);
    setLayout(layout_result);

    connect(save,SIGNAL(clicked()),this,SLOT(slotPushButton()));
    connect(noSave,SIGNAL(clicked()),this,SLOT(slotPushButton()));
}

void ExitWindow::slotPushButton(){
    QPushButton* button=qobject_cast<QPushButton*>(sender());
    if(button==nullptr) {
        done(0);
        close();
        return;
    }
    if(button->text()==tr("Save")){
        done(1);
        close();
        return;
    }
    if(button->text()==tr("Not save")){
        done(2);
        close();
        return;
    }
    done(0);
    close();
    return;
}
