#include "autocompletelistview.h"

#include <QDebug>
#include <QPainter>
#include <QTextDocument>

AutocompleteListView::AutocompleteListView(QWidget *parent) :
    QListView(parent)
{
    setFixedWidth(320);
    setViewMode(QListView::ListMode);
    setUniformItemSizes(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setItemDelegateForColumn(0, new AutoCompleteItemDelegate(this));

    connect(this, &QListView::activated, [=](const QModelIndex &index){
        emit selected(qvariant_cast<AutocompleteView*>(index.data(Qt::UserRole)));
        setVisible(false);
    });
    connect(this, &QListView::clicked, [=](const QModelIndex &index){
        emit selected(qvariant_cast<AutocompleteView*>(index.data(Qt::UserRole)));
    });
}

void AutocompleteListView::paintEvent(QPaintEvent *e) {
    QListView::paintEvent(e);
}

QModelIndex AutocompleteListView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers) {
    switch(cursorAction) {
    case MoveDown:
    case MoveRight:
    case MoveEnd:
    case MovePageDown:
    case MoveNext:
        for (int i = currentIndex().row() + 1; i < model()->rowCount(); i++) {
            auto index = model()->index(i, 0);
            if (index.isValid() && index.flags() & Qt::ItemIsEnabled) {
                return model()->index(i, 0);
            }
        }
        return QModelIndex();
    case MoveUp:
    case MoveLeft:
    case MoveHome:
    case MovePageUp:
    case MovePrevious: {
        for (int i = currentIndex().row() - 1; i >= 0; i--) {
            auto index = model()->index(i, 0);
            if (index.isValid() && index.flags() & Qt::ItemIsEnabled) {
                return model()->index(i, 0);
            }
        }
        return QModelIndex();
    }
    }
    return QModelIndex();
}

void AutocompleteListView::showEvent(QShowEvent *event) {
    emit visibleChanged();
    QListView::showEvent(event);
}

void AutocompleteListView::hideEvent(QHideEvent *event) {
    emit visibleChanged();
    QListView::hideEvent(event);
}

void AutocompleteListView::keyPressEvent(QKeyEvent *e)
{
    bool modifiers = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (modifiers) {
        //emit keyPress(e);
        return;
    }

    switch (e->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
        QListView::keyPressEvent(e);
        return;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        QListView::keyPressEvent(e);
        return;
    case Qt::Key_Down:
        QListView::keyPressEvent(e);
        return;
    case Qt::Key_Up:
        QListView::keyPressEvent(e);
        return;
    }

    //emit keyPress(e);
}

AutoCompleteItemDelegate::AutoCompleteItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void AutoCompleteItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QTextDocument doc;
    doc.setTextWidth(319);
    auto view = getCurrentView(index);
    doc.setHtml(format(view));

    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    painter->translate(option.rect.left(), option.rect.top());
    QRect clip(0, 0, option.rect.width(), option.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize AutoCompleteItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return { 320, 24 };
}

QString AutoCompleteItemDelegate::format(const AutocompleteView *view) const {
    QString label;
    QString transparent = "background-color: transparent;";

    // Format is: Description - TaskName · ProjectName - ClientName

    switch (view->Type) {
    case 13: // Workspace row
        return "<div width=320px style='width:320px;font-size:13px;color:#9e9e9e;text-align:center;background-color:transparent;font-weight:bold;margin:2px;padding:2px;'>" + view->Description + "</div><div width=320px height=1px style='background-color:#ececec'/>";
    case 11: // Category row
        return "<div width=320px style='width:320px;font-size:12px;color:#9e9e9e;background-color:transparent;font-weight:500'>" + view->Description + "</div>";
    case 12: { // Client row / no project row
        return "<div width=320px style='width:320px;font-size:12px;color:#9e9e9e;background-color:transparent;font-weight:500;margin-left:9px;'>" + view->Description + "</div>";
    }
    case 2: { // Project items rows
        label.append("<div style='font-size:12px;margin-left:18px;color:" + view->ProjectColor + ";'>• " + view->ProjectLabel + "</div>");
        return label;
    }
    case 1: // Task row
        return "<div style='margin-left:18px;background-color:transparent;'>" + view->Description + "</span>";
    case 0: { // Item rows (projects/time entries)
        QString table("<div style='margin-left:9px;font-size:12px;'>");
        if (!view->Description.isEmpty())
            table.append(view->Description + " ");
        if (view->TaskID)
            table.append(view->TaskLabel + " ");
        //table.append("<br>");
        if (view->ProjectID)
            table.append("<span style='color:" + view->ProjectColor + ";'> • " + view->ProjectLabel + " </span>");
        if (view->ClientID)
            table.append("<span style='color:#9e9e9e;'> " + view->ClientLabel + "</span>");
        table.append("</div>");
        return table;
    }
    default:
        //ui->label->setStyleSheet(transparent + "padding-top:7px;padding-left:15px;font-size:9pt;");
        return "<span style='background-color: transparent;font-size:13px;'>" + view->ProjectLabel + view->TaskLabel + view->ClientLabel + view->Description + "</span>";
    }
}

AutocompleteView *AutoCompleteItemDelegate::getCurrentView(const QModelIndex &index) const {
    return qvariant_cast<AutocompleteView*>(index.data(Qt::UserRole));
}
