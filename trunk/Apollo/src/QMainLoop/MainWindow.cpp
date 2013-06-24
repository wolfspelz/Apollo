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
  _pExitAction = new QAction("E&xit", this);
  if (_pExitAction == 0) { throw ApException(LOG_CONTEXT, "new QAction failed"); }

  connect(_pExitAction, SIGNAL(triggered()), pApp, SLOT(quit()));
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
}
