//
// Created by LCT on 2024/3/19 0019.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NovelWindow.h" resolved

#include "novelwindow.h"
#include "ui_NovelWindow.h"
#include <QSettings>
#include <QFile>
#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QShortcut>
#include <QTextCodec>
#include "qxtglobalshortcut.h"
#include <QMenu>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
NovelWindow::NovelWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::NovelWindow),
        novel(nullptr),
        currentPos(),
        moveWindow(false),
        settings("JinR", "My novel"),
        pageSize(10)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    setGraphicsEffect(effect);
    effect->setOpacity(1.0);
    refreshParam();
    openFile();

    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(QKeySequence(Qt::ALT + Qt::Key_W), this);
    connect(shortcut, &QxtGlobalShortcut::activated, [this]() {
        nextPage();
    });
    QxtGlobalShortcut *shortcut2 = new QxtGlobalShortcut(QKeySequence(Qt::ALT + Qt::Key_Q), this);
    QObject::connect(shortcut2, &QxtGlobalShortcut::activated, [this]() {
        lastPage();
    });

    QxtGlobalShortcut *shortcut3 = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_M), this);
    QObject::connect(shortcut3, &QxtGlobalShortcut::activated, [this]() {
        if(isHidden())
            show();
        else
            hide();
    });

    QxtGlobalShortcut *shortcut4 = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_N), this);
    QObject::connect(shortcut4, &QxtGlobalShortcut::activated, [this]() {
        setGeometry(0,0,50,ui->label->height());
    });
}

NovelWindow::~NovelWindow()
{
    delete ui;
}

void NovelWindow::displayPage(bool pos)
{
    if(!novel)
        return;
    if(!pos)
    {
        auto fpos = textStream->pos();
        auto rpos = fpos;
        for(int i = 0;i < pageSize * 2;i++)
        {
            bool flag = false;
            for(int k = 1;k < 10;k++)
            {
                auto offset =  fpos - k;
                if(offset < 0)
                    offset = 0;
                auto text = novel->map(offset,k);
                auto str = QString::fromLocal8Bit((char*)text,k);

                auto ss = str.size();
                if(ss == 1)
                {
                    fpos = fpos - k;
                    break;
                }
            }
        }
        textStream->seek(fpos);
    }

    settings.setValue("RecentPage",textStream->pos());

    auto str = textStream->read(pageSize);
    //QString str(arr);
    str.replace('\r',' ');
    str.replace('\n',' ');
    //QTextCodec codec
    ui->label->setText(str);
    ui->label->setFixedWidth(str.size() * 20);
}

void NovelWindow::openFile(bool change)
{
    if(novel)
    {
        delete novel;
    }
    auto file = new QFile(novelPath);
    file->open(QFile::OpenModeFlag::ReadOnly);
    if(file->isOpen())
    {
        novel = file;
        textStream = new QTextStream(novel);
        settings.setValue("OpenNovelPath",novelPath);
        if(change)
        {
            currentPos = 0;
            settings.setValue("RecentPage",currentPos);
        }
        else{
            textStream->seek(currentPos);
        }
    }
    else
    {
        ui->label->setText("******************");
        return;
    }
    displayPage();

}

void NovelWindow::nextPage()
{
    //currentPage++;
    displayPage();
}

void NovelWindow::lastPage()
{
    displayPage(false);
}

void NovelWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(moveWindow)
    {
        auto currentPos = event->globalPos();
        auto geo = geometry();
        auto vec = currentPos - startPos;

        auto curWindow = event->windowPos();

        auto x = currentPos.x() - windowPos.x();
        auto y = currentPos.y() - windowPos.y();

        //auto vecW = curWindow - windowPos;
        //windowPos = curWindow;
        setGeometry(QRect(x,y,geo.width(),geo.height()));
        startPos = currentPos;
    }
}

void NovelWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        startPos = event->globalPos();
        windowPos = event->windowPos();
        moveWindow = true;
    }
}

void NovelWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        moveWindow = false;
    }
}

void NovelWindow::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
}

void NovelWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu = new QMenu(this);

    auto pageSizeAction = new QAction("Page Size");
    connect(pageSizeAction,&QAction::triggered,this,&NovelWindow::changePageSize);

    auto changeNovelPathAction = new QAction("Change Novel Path");
    connect(changeNovelPathAction,&QAction::triggered,this,&NovelWindow::changeNovelPath);

    auto exitAction = new QAction("Exit");
    connect(exitAction,&QAction::triggered,this,&NovelWindow::close);

    auto searchAction = new QAction("Search");
    connect(searchAction,&QAction::triggered,this,&NovelWindow::search);

    menu->addAction(pageSizeAction);
    menu->addAction(changeNovelPathAction);
    menu->addAction(searchAction);
    menu->addAction(exitAction);

    menu->exec(QCursor::pos());
}

void NovelWindow::refreshParam()
{
    novelPath = settings.value("OpenNovelPath").value<QString>();
    currentPos = settings.value("RecentPage").value<int>();
    pageSize = settings.value("PageSize").value<int>();

    auto x = settings.value("GeometryX").value<int>();
    auto y = settings.value("GeometryY").value<int>();
    auto geo = geometry();
    //auto w = settings.value("GeometryW").value<int>();
    //if(w == 0)
    //    w = ui->label->width();
    //auto h = settings.value("GeometryH").value<int>();
    //if(h == 0)
    //    w = ui->label->height();

    setGeometry(QRect(x,y,geo.width(),geo.height()));
    if(pageSize == 0)
    {
        pageSize = 10;
    }
}

void NovelWindow::changePageSize()
{
    bool isOk = false;
    auto val = QInputDialog::getInt(this,"Page Size","Size",pageSize,10,9999,1,&isOk);
    if(isOk)
    {
        pageSize = val;
        settings.setValue("PageSize",val);
    }
}

void NovelWindow::changeNovelPath()
{
    auto fileName = QFileDialog::getOpenFileName(this,"Open","./","Text (*.txt)");
    novelPath = fileName;
    openFile();
}

void NovelWindow::closeEvent(QCloseEvent *event)
{
    auto geo = geometry();
    settings.setValue("GeometryX",geo.x());
    settings.setValue("GeometryY",geo.y());
    settings.setValue("GeometryW",geo.width());
    settings.setValue("GeometryH",geo.height());
}

void NovelWindow::search()
{
    if(!novel)
        return;
    bool isOk = false;
    auto curPos = textStream->pos();

    auto str = QInputDialog::getText(this,"Search","Text",QLineEdit::EchoMode::Normal,"",&isOk);
    if(!isOk)
        return;
    auto searchS = str.toLocal8Bit();

    textStream->seek(0);
    auto all = textStream->readAll().toLocal8Bit();

    auto idx = all.indexOf(searchS);
    if(idx > 0)
    {
        textStream->seek(idx);
        displayPage();
    }
    else{
        textStream->seek(curPos);
        QMessageBox::warning(this,"Warn","Not found");
    }
}
/*
void NovelWindow::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();
}
 */

