/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExplort;
    QAction *actionAbout;
    QAction *actionThirdParty_Licenses;
    QAction *actionAbout_Qt;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *main_layout;
    QVBoxLayout *video_player_container;
    QWidget *video;
    QWidget *controls;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *start_cap;
    QPushButton *minus_1;
    QPushButton *minus_01;
    QPushButton *play_btn;
    QPushButton *pause_btn;
    QPushButton *add_01;
    QPushButton *add_1;
    QPushButton *end_cap;
    QWidget *poses_list;
    QHBoxLayout *horizontalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *right_toolbar;
    QVBoxLayout *verticalLayout;
    QTableView *tableView;
    QMenuBar *menubar;
    QMenu *menuArquivo;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1487, 696);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
        actionOpen->setIcon(icon);
        actionExplort = new QAction(MainWindow);
        actionExplort->setObjectName("actionExplort");
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
        actionExplort->setIcon(icon1);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::HelpFaq));
        actionAbout->setIcon(icon2);
        actionThirdParty_Licenses = new QAction(MainWindow);
        actionThirdParty_Licenses->setObjectName("actionThirdParty_Licenses");
        QIcon icon3(QIcon::fromTheme(QIcon::ThemeIcon::DialogWarning));
        actionThirdParty_Licenses->setIcon(icon3);
        actionAbout_Qt = new QAction(MainWindow);
        actionAbout_Qt->setObjectName("actionAbout_Qt");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        centralwidget->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        centralwidget->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        main_layout = new QVBoxLayout();
        main_layout->setObjectName("main_layout");
        video_player_container = new QVBoxLayout();
        video_player_container->setSpacing(8);
        video_player_container->setObjectName("video_player_container");
        video_player_container->setContentsMargins(8, 8, 8, 8);
        video = new QWidget(centralwidget);
        video->setObjectName("video");
        video->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

        video_player_container->addWidget(video);

        controls = new QWidget(centralwidget);
        controls->setObjectName("controls");
        controls->setMaximumSize(QSize(16777215, 80));
        horizontalLayout_4 = new QHBoxLayout(controls);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(16);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(100, -1, 100, -1);
        start_cap = new QPushButton(controls);
        start_cap->setObjectName("start_cap");

        horizontalLayout_5->addWidget(start_cap);

        minus_1 = new QPushButton(controls);
        minus_1->setObjectName("minus_1");

        horizontalLayout_5->addWidget(minus_1);

        minus_01 = new QPushButton(controls);
        minus_01->setObjectName("minus_01");

        horizontalLayout_5->addWidget(minus_01);

        play_btn = new QPushButton(controls);
        play_btn->setObjectName("play_btn");

        horizontalLayout_5->addWidget(play_btn);

        pause_btn = new QPushButton(controls);
        pause_btn->setObjectName("pause_btn");

        horizontalLayout_5->addWidget(pause_btn);

        add_01 = new QPushButton(controls);
        add_01->setObjectName("add_01");

        horizontalLayout_5->addWidget(add_01);

        add_1 = new QPushButton(controls);
        add_1->setObjectName("add_1");

        horizontalLayout_5->addWidget(add_1);

        end_cap = new QPushButton(controls);
        end_cap->setObjectName("end_cap");

        horizontalLayout_5->addWidget(end_cap);


        horizontalLayout_4->addLayout(horizontalLayout_5);


        video_player_container->addWidget(controls);


        main_layout->addLayout(video_player_container);

        poses_list = new QWidget(centralwidget);
        poses_list->setObjectName("poses_list");
        poses_list->setMaximumSize(QSize(16777215, 100));
        horizontalLayout_2 = new QHBoxLayout(poses_list);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        scrollArea = new QScrollArea(poses_list);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1141, 80));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout_2->addWidget(scrollArea);


        main_layout->addWidget(poses_list);


        horizontalLayout->addLayout(main_layout);

        right_toolbar = new QWidget(centralwidget);
        right_toolbar->setObjectName("right_toolbar");
        right_toolbar->setMaximumSize(QSize(300, 16777215));
        verticalLayout = new QVBoxLayout(right_toolbar);
        verticalLayout->setObjectName("verticalLayout");
        tableView = new QTableView(right_toolbar);
        tableView->setObjectName("tableView");

        verticalLayout->addWidget(tableView);


        horizontalLayout->addWidget(right_toolbar);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1487, 21));
        menuArquivo = new QMenu(menubar);
        menuArquivo->setObjectName("menuArquivo");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuArquivo->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuArquivo->addAction(actionOpen);
        menuArquivo->addAction(actionExplort);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionThirdParty_Licenses);
        menuHelp->addAction(actionAbout_Qt);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "BlazeDance Studio Pro", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open V\303\255deo", nullptr));
#if QT_CONFIG(tooltip)
        actionOpen->setToolTip(QCoreApplication::translate("MainWindow", "Load a video file", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExplort->setText(QCoreApplication::translate("MainWindow", "Export to .dance.json", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionThirdParty_Licenses->setText(QCoreApplication::translate("MainWindow", "ThirdParty Licenses", nullptr));
        actionAbout_Qt->setText(QCoreApplication::translate("MainWindow", "About Qt", nullptr));
        start_cap->setText(QCoreApplication::translate("MainWindow", "StartCapture", nullptr));
        minus_1->setText(QCoreApplication::translate("MainWindow", "-1", nullptr));
        minus_01->setText(QCoreApplication::translate("MainWindow", "-0.1", nullptr));
        play_btn->setText(QCoreApplication::translate("MainWindow", "Play", nullptr));
        pause_btn->setText(QCoreApplication::translate("MainWindow", "Pause", nullptr));
        add_01->setText(QCoreApplication::translate("MainWindow", "+0.1", nullptr));
        add_1->setText(QCoreApplication::translate("MainWindow", "+1", nullptr));
        end_cap->setText(QCoreApplication::translate("MainWindow", "EndCapture", nullptr));
        menuArquivo->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
