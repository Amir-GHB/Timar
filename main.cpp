#include "tmrhome.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <qluent/style/QluentStyle.h>
#include <QFontDatabase>
#include <QFont>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle(new qluent::QluentStyle(&a));

    // ============================================================
    // Fonts
    // ============================================================

    const int regularId =
        QFontDatabase::addApplicationFont(
            ":/fonts/Vazirmatn-Regular.ttf");

    qDebug() << "Font ID:" << regularId;

    if (regularId >= 0) {
        const QStringList families =
            QFontDatabase::applicationFontFamilies(regularId);

        qDebug() << "Font families:" << families;

        if (!families.isEmpty()) {
            QFont font(families.first());
            font.setPointSize(10);
            a.setFont(font);
        }
    } else {
        qWarning() << "Could not load Vazirmatn from resources";
    }

    Tmrhome w;
    w.show();
    return QApplication::exec();
}
