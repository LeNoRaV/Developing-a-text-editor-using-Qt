#include "xmlmodel.h"

XmlModel::XmlModel(QObject* parent):QAbstractItemModel(parent){
    Root=new QObject(this);
}

XmlModel::~XmlModel(){
    delete Root;
    delete ActiveObj;
    delete TextErrors;
}

void XmlModel::makeActiveObj(){
    QObject* obj=getObjectByIndex(CurrentIndex);
    if (obj != ActiveObj){
        ActiveObj->setProperty("bold",false);
        obj->setProperty("bold",true);
        ActiveObj=obj;
    }
}

QObject* XmlModel::getRoot() const {
    return Root;
}

void XmlModel::getTextErrors(QString str){
    TextErrors = new QTextEdit();
    TextErrors->setWindowTitle(tr("ERRORS"));
    TextErrors->setText(str);
    TextErrors->setReadOnly(true);
    TextErrors->show();
}

void XmlModel::setCurrentIndex(QModelIndex index){
    CurrentIndex = index;
}

void addElements(const QDomNode& elem,QObject* parent){
    if (!(elem.isNull())) {
       if((!(elem.isText()))&&(elem.isElement())){
            QObject* newNode=new QObject(parent);
            newNode->setProperty("text",elem.toElement().tagName());
            if(elem.hasChildNodes()){
                QDomNodeList ListNodes=elem.childNodes();
                for(int i=0;i<ListNodes.count();++i){
                    addElements(ListNodes.at(i),newNode);
                }
            }
        }
        if(elem.isText()){
            QObject* newNode=new QObject(parent);
            newNode->setProperty("text",elem.toText().data());
        }
    }

}

void XmlModel::addFiles(const QStringList* namesList){
    QString Errors = Q_NULLPTR;
    for(const auto nameFile:*namesList){
        QDomDocument doc;
        QFile file(nameFile);
        if (!file.open(QIODevice::ReadOnly)){
            Errors = Errors + nameFile + tr(" can't be open\n");
        }
        if (!doc.setContent(&file)) {
            Errors = Errors + nameFile + tr(" can't be set as content\n");
        }

        QFileInfo fileInfo(nameFile);
        QString name=fileInfo.fileName();

        bool isNotOpen=true;

        for(const auto child:Root->children()){
            if (child->property("text").toString() == name)
                isNotOpen=false;
        }

        if(isNotOpen){
            QObject* FirstElem=new QObject(Root);
            FirstElem->setProperty("text", name);
            QDomElement element = doc.documentElement(); //represents one element in the DOM tree
            addElements(element, FirstElem);
        }
        else {
            Errors = Errors + nameFile + tr(" is already opened\n");
        }
        file.close();
    }

    if (Errors != Q_NULLPTR) getTextErrors(Errors);

    if (ActiveObj==Q_NULLPTR){
        getObjectByIndex(index(0,0))->setProperty("bold",true);
        ActiveObj=getObjectByIndex(index(0,0));
    }

}

QObject* XmlModel::getObjectByIndex(const QModelIndex& index) const{
    if (!index.isValid()) //A valid index belongs to a model, and has non-negative row and column numbers
        return Root;

    return static_cast<QObject*>(index.internalPointer());
}

QModelIndex XmlModel::index(int row, int column, const QModelIndex &parent) const{
    if(!(hasIndex(row,column,parent))){
        return QModelIndex();
    }

    QObject* objParent = getObjectByIndex(parent);
    return createIndex(row,column,objParent->children().at(row)); //internalPointer
}

QModelIndex XmlModel::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();

    QObject* obj = getObjectByIndex(index);
    QObject* objParent = obj->parent();

    if(!(objParent==Root)){
        QObject* objGrandParent = objParent->parent();
        return createIndex(objGrandParent->children().indexOf(objParent), 0, objParent);
        //indexOf(value)
        //Returns the index position of the first occurrence of value in the list,
        //searching forward from index position from.
    }

    return QModelIndex();
}

int XmlModel::rowCount(const QModelIndex &parent) const{
    return getObjectByIndex(parent)->children().count();
}

int XmlModel::columnCount(const QModelIndex &parent) const{
    return 1;
}

//Returns the data stored under the given role for the item referred to by the index
QVariant XmlModel::data(const QModelIndex &index, int role) const{
    if(!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole){ //The key data to be rendered in the form of text. (QString)
        return getObjectByIndex(index)->property("text");
    }

    if(role==Qt::FontRole){ //The font used for items rendered with the default delegate. (QFont)
        if(getObjectByIndex(index)->property("bold")==true){
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    return QVariant();
}


