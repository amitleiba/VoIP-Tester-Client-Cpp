//#pragma once
//#include <QComboBox>
//#include <QStandardItemModel>
//#include <QAbstractItemView>

//class CheckableComboBox : public QComboBox {
//public:
//    CheckableComboBox(QWidget* parent = nullptr)
//        : QComboBox(parent)
//    {
//        connect(view(), &QAbstractItemView::pressed, this, &CheckableComboBox::handleItemPressed);
//        setModel(new QStandardItemModel(this));
//    }

//signals:
//    void filterAutoTestLabelSignal(const std::string&);

//private:
//    void handleItemPressed(const QModelIndex& index) {
//        QList<QStandardItem*> items;
//        for (int i = 0; i < count(); i++) {
//            items.append(model()->itemData(i));
//        }
//        QStandardItem* allItem = nullptr;
//        for (QStandardItem* item : items) {
//            if (item->text() == "ALL") {
//                allItem = item;
//                break;
//            }
//        }
//        QStandardItem* item = model()->itemData(index);
//        if (item->checkState() == Qt::Checked) {
//            if (item->text() == "ALL") {
//                for (QStandardItem* itemInList : items) {
//                    if (itemInList->checkState() == Qt::Checked) {
//                        itemInList->setCheckState(Qt::Unchecked);
//                        if (itemInList->text() != "ALL") {
//                            selectedItems.removeAll(itemInList->text());
//                        }
//                    }
//                }
//            }
//            else {
//                item->setCheckState(Qt::Unchecked);
//                allItem->setCheckState(Qt::Unchecked);
//                selectedItems.removeAll(item->text());
//                // doAction()
//            }
//           emit filterAutoTestLabel("remove");
//        }
//        else {
//            if (item->text() == "ALL") {
//                for (QStandardItem* itemInList : items) {
//                    if (itemInList->checkState() != Qt::Checked) {
//                        itemInList->setCheckState(Qt::Checked);
//                        if (itemInList->text() != "ALL") {
//                            selectedItems.append(itemInList->text());
//                        }
//                    }
//                }
//            }
//            else {
//                item->setCheckState(Qt::Checked);
//                allItem->setCheckState(Qt::Checked);
//                for (QStandardItem* itemInList : items) {
//                    if (itemInList->text() != "ALL" && itemInList->checkState() != Qt::Checked) {
//                        allItem->setCheckState(Qt::Unchecked);
//                        break;
//                    }
//                }
//                selectedItems.append(item->text());
//            }
//            emit filterAutoTestLabel(item->text());
//        }
//    }
//private:
//    QStringList selectedItems;
//};
