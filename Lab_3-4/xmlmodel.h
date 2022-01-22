#ifndef XMLMODEL_H
#define XMLMODEL_H

#include <QApplication>

#include <QtXml>
#include <QDomDocument> //The QDomDocument class represents the entire XML document
#include <QAbstractItemModel>
#include <QTextEdit>

class XmlModel:public QAbstractItemModel{
    Q_OBJECT

    QObject* Root;
    QObject* ActiveObj=Q_NULLPTR;
    QModelIndex CurrentIndex = QModelIndex();

    QTextEdit* TextErrors;

public:
    XmlModel(QObject* parent=nullptr);
    ~XmlModel();

    QObject* getRoot() const;
    void getTextErrors(QString);
    void setCurrentIndex(QModelIndex);

    void addFiles(const QStringList*);
    friend void addElements(const QDomNode&,QObject*);

    QObject* getObjectByIndex(const QModelIndex&) const;

    //When subclassing QAbstractItemModel, at the very least you must implement
    //index(), parent(), rowCount(), columnCount(), and data(). These functions
    //are used in all read-only models, and form the basis of editable models.

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void makeActiveObj();
};

#endif // XMLMODEL_H
