#ifndef RT_TITLE_H
#define RT_TITLE_H

#include <QWidget>
#include <QRectF>
#include <QDebug>
class rt_title : public QWidget
{
    Q_OBJECT
public:
    explicit rt_title(QWidget *parent = nullptr);
public:
    void setHSizeHint(int hhh);
    void setHSpeed_Scale(const int scale);
signals:
    void H_Scale_changed();
public slots:

private:
    int margin;
    int HSizeHint;      // Widget�߶�
    int H_speed_scale;  // ˮƽ����scale��������ˮƽ���ٱ������ı�����ɫ
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_TITLE_H
