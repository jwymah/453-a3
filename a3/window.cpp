#include "window.h"
#include "renderer.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Creature Loader");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 300);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

}

void Window::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Control :
        renderer->setCtrlStatus(true);
        break;
    case Qt::Key_Shift :
        renderer->setShiftStatus(true);
        break;
    }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
//    cout << event->key() << " Released" << endl;
    switch(event->key()){
    case Qt::Key_Shift :
        renderer->setShiftStatus(false);
        break;
    case Qt::Key_Control :
        renderer->setCtrlStatus(false);
        break;
    }
}

// destructor
Window::~Window()
{
    delete renderer;
}
