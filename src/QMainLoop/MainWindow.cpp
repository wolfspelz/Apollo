// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MainWindow.h"

void MainWindow::Setup(QApplication* pApp)
{  
  _pApp = pApp;

  _pExitAction = new QAction("E&xit", this);
  if (_pExitAction == 0) { throw ApException(LOG_CONTEXT, "new QAction failed"); }

  connect(_pExitAction, SIGNAL(triggered()), this, SLOT(Quit()));

  _pFileMenu = menuBar()->addMenu("&File");
  if (_pFileMenu == 0) { throw ApException(LOG_CONTEXT, "menuBar()->addMenu failed"); }

  _pFileMenu->addAction(_pExitAction);

  _pTextEdit = new QTextEdit();
  if (_pTextEdit == 0) { throw ApException(LOG_CONTEXT, "new QTextEdit failed"); }

  setCentralWidget(_pTextEdit);
  setWindowTitle("MainWindow");
}

void MainWindow::Quit()
{
  int nDelay = Apollo::getModuleConfig(MODULE_NAME, "ShutdownDelay", 2);
  {
    Msg_MainLoop_EventLoopDelayEnd msg;
    msg.nDelaySec = nDelay;
    msg.nWaitCount = 0;
    msg.Filter();

    _nWaitFinishingModules = msg.nWaitCount;
    if (_nWaitFinishingModules == 0) {
      nDelay = 0;
    } else {
      nDelay = msg.nDelaySec;
    }
  }

  QTimer::singleShot(1000 * nDelay, this, SLOT(Terminate()));
}

void MainWindow::Terminate()
{
  _pApp->quit();
}
