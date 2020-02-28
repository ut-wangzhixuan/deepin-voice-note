#ifndef LeftViewDelegate_H
#define LeftViewDelegate_H

#include <DStyledItemDelegate>
DWIDGET_USE_NAMESPACE

class LeftViewDelegate : public DStyledItemDelegate
{
public:
    LeftViewDelegate(QAbstractItemView *parent = nullptr);
    void handleChangeTheme();

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
private:
    void init();
    void paintNoteRoot(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    void paintNoteItem(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    QAbstractItemView *m_treeView {nullptr};
    DPalette m_parentPb;
};

#endif // LeftViewDelegate_H