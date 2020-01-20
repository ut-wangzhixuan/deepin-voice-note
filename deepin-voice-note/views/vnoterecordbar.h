#ifndef VNOTERECORDBAR_H
#define VNOTERECORDBAR_H

#include <QWidget>

#include <DAnchors>

DWIDGET_USE_NAMESPACE

class VNoteRecordWidget;
class VNoteIconButton;

class VNoteRecordBar : public QWidget
{
    Q_OBJECT
public:
    explicit VNoteRecordBar(QWidget *parent = nullptr);

    static constexpr int REC_BTN_W = 68;
    static constexpr int REC_BTN_H = 68;

    void initUI();
    void initConnections();
signals:
    void sigStartRecord();
    void sigFinshRecord(const QString &voicePath,qint64 voiceSize);

public slots:
    void onStartRecord();
    void onFinshRecord(const QString &voicePath,qint64 voiceSize);
    void OnMicrophoneAvailableChanged(bool isAvailable);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
protected:
    VNoteRecordWidget *m_recordPanel {nullptr};
    VNoteIconButton   *m_recordBtn {nullptr};
    QScopedPointer<DAnchorsBase> m_recBtnAnchor;

    QString          m_recordPath {""};
};

#endif // VNOTERECORDBAR_H