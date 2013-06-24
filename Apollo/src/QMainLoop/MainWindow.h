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
  void Setup(QApplication* pApp);

private slots:
  void Quit();

private:
  QTextEdit *_pTextEdit;
  QAction *_pExitAction;
  QMenu *_pFileMenu;
};

#endif // MainWindow_H_INCLUDED
