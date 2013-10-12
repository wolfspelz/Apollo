// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MainWindow_H_INCLUDED)
#define MainWindow_H_INCLUDED

#include "Local.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  MainWindow()
  :_pApp(0)
  ,_pTextEdit(0)
  ,_pExitAction(0)
  ,_pFileMenu(0)
  ,_nWaitFinishingModules(0)
  ,_nFinishedModules(0)
  {}

  void Setup(QApplication* pApp);

private slots:
  void Quit();
  void Terminate();

private:
  QApplication* _pApp;
  QTextEdit *_pTextEdit;
  QAction *_pExitAction;
  QMenu *_pFileMenu;

  int _nWaitFinishingModules;
  int _nFinishedModules;
};

#endif // MainWindow_H_INCLUDED
