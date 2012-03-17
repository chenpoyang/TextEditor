#ifndef TEXTEDITER_H
#define TEXTEDITER_H

#include <QMainWindow>
#include <QtGui>
#include <qtextedit.h>

namespace Ui {
    class TextEditer;
}

class TextEditer : public QMainWindow
{
    Q_OBJECT

public:
	explicit TextEditer(QWidget *parent = 0);
    ~TextEditer();

private:
	void initMenu();	/* 初始化菜单栏 */
	void initToolBar();	/* 初始化工具栏 */
	void initCenterWidget();	/* 初始化中心部件 */
	void initStatusBar();	/* 初始化状态栏 */
	void initLocSize();	/* 初始化窗口位置和大小 */
	void initConnection();	/* 初始化相应的信号与槽 */
	bool saveFile(const QString &filename);	/* 保存文件 */
	void mayBeSaved();	/* 当前文本文件可能要被保存 */
	bool loadFile(const QString &filename);	/* 把文本文件装载到文本编辑器中 */

private slots:
	/* 文件菜单 */
	void doNew();	/* 新建文件 */
	void doOpen();	/* 打开文件 */
	void doClose();	/* 关闭文件 */
	void doSave();	/* 保存文件 */
	void doASave();	/* 另存为文件 */
	void doExit();	/* 退出文本编辑器 */

	/* 编辑菜单 */
	void doUndo();	/* 撤消 */
	void doRedo();	/* 取消上一个撤消 */
	void doCut();	/* 剪切 */
	void doCopy();	/* 复制文本 */
	void doPast();	/* 粘贴文本 */
	void doSelectAll();	/* 全选文本 */

	/* 工具菜单栏 */
	void setFontForText();	/* 设置文本的字体 */

	/* 光标改变时设置光标的们位置 */
	void doCursorChanged();

	/* 响应文本textEdit的contentsChanged() */
	void doModified();

private:
	/* 菜单 */
	QMenu *menuFile;
	QMenu *menuEdit;
	QMenu *menuTool;

	/* 工具栏 */
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QStatusBar *stusBar;

	/* 文本输入框 */
	QTextEdit *textEdit;

	/* 动作 */
	QAction *actNew, *actOpen, *actClose, *actSave, *actASave, *actExit, *mSepOne, *mSepTwo;
	QAction *actUndo, *actRedo, *actCut, *actCopy, *actPast, *actAll;
	QAction *actFont;

	/* 状态栏下的标签 */
	QLabel *statusLabelOne, *statusLabelTwo;

	bool isUntitled;	/* 当前文件是否命名 */
	bool isSaved;		/* 判断当前文件是否已保存 */
	bool hasFile;		/* 是否有文件打开 */
	QString curFile;	/* 目前正在编辑的文件的文件名 */

private:
    Ui::TextEditer *ui;
};

#endif // TEXTEDITER_H
