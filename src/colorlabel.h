#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QColor>
#include <QPixmap>
#include <QColorDialog>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    ColorLabel(int width, int height, const QColor& color, QWidget* parent = nullptr) : QLabel(parent) {
        if (width > 0) {
            this->setFixedWidth(width);
        }
        if (height > 0) {
            this->setFixedHeight(height);
        }
        setColor(color);
    }

    const QColor& getColor() const { return color; }

    void setColor(int r, int g, int b) {
        setColor(qRgb(r, g, b));
    }

    void setColor(const QColor& c) {
        color = c;
        QPixmap pixmap(this->size());
        pixmap.fill(color);
        this->setPixmap(pixmap);
    }

signals:
    void colorChanged(QColor color);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev) override {
        QColor c = QColorDialog::getColor(color, nullptr, "Select Color",
                                          QColorDialog::DontUseNativeDialog);

        if (c.isValid()) {
            setColor(c);
            emit colorChanged(c);
        }
    }

private:
    QColor color;
};

#endif // COLORLABEL_H
