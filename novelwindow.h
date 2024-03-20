//
// Created by LCT on 2024/3/19 0019.
//

#ifndef THIEFQT_NOVELWINDOW_H
#define THIEFQT_NOVELWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTextStream>
class QFile;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class NovelWindow;
}
QT_END_NAMESPACE

class NovelWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit NovelWindow(QWidget *parent = nullptr);

    ~NovelWindow() override;
    void openFile(bool change = false);
    void nextPage();
    void lastPage();
    void refreshParam();
    void displayPage(bool pos = true);
    void changePageSize();
    void changeNovelPath();
    void search();
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    //void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::NovelWindow *ui;
    QFile* novel;
    bool moveWindow;
    QPoint startPos;
    QPointF windowPos;
    int currentPos;
    int pageSize;
    QString novelPath;
    QSettings settings;
    QTextStream* textStream;
};


#endif //THIEFQT_NOVELWINDOW_H
