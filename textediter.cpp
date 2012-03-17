#include "textediter.h"
#include "ui_textediter.h"
#include <QtGui>

TextEditer::TextEditer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextEditer)
{
    ui->setupUi(this);
	initMenu();
	initToolBar();
	initCenterWidget();
	initStatusBar();
	initLocSize();
	initConnection();

	textEdit->setFont(QFont("FreeMono", 10));	/* 默认字体 */
	textEdit->setTabStopWidth(40);	/* Tab键跳格参数, 系统默认为80, 修改为40 */

	isUntitled = true;	/* 新文件未命名 */
	isSaved = false;	/* 新文件未保存 */
	curFile = "";
	hasFile = false;	/* 没有文件打开 */
	setWindowTitle(tr("TextEditer"));
}

/* 初始化相应的信号与槽 */
void TextEditer::initConnection()
{
	/* menuFile里的信号与槽 */
	connect(actNew, SIGNAL(triggered()), this, SLOT(doNew()));
	connect(actOpen, SIGNAL(triggered()), this, SLOT(doOpen()));
	connect(actClose, SIGNAL(triggered()), this, SLOT(doClose()));
	connect(actSave, SIGNAL(triggered()), this, SLOT(doSave()));
	connect(actASave, SIGNAL(triggered()), this, SLOT(doASave()));
	connect(actExit, SIGNAL(triggered()), this, SLOT(doExit()));

	/* menuEdit里的信号与槽 */
	connect(actUndo, SIGNAL(triggered()), this, SLOT(doUndo()));
	connect(actRedo, SIGNAL(triggered()), this, SLOT(doRedo()));
	connect(actCut, SIGNAL(triggered()), this, SLOT(doCut()));
	connect(actCopy, SIGNAL(triggered()), this, SLOT(doCopy()));
	connect(actPast, SIGNAL(triggered()), this, SLOT(doPast()));
	connect(actAll, SIGNAL(triggered()), this, SLOT(doSelectAll()));

	/* menuTool里的信号与槽 */
	connect(actFont, SIGNAL(triggered()), this, SLOT(setFontForText()));

	/* 当当前文本内容改变后, 自动调用doModified() */
	connect(textEdit->document(), SIGNAL(contentsChanged()),
				this, SLOT(doModified()));

	/* 当文档修改后, 刷新光标所在的位置 */
	connect(textEdit->document(), SIGNAL(contentsChanged()),
			this, SLOT(doCursorChanged()));
}

/* 撤消 */
void TextEditer::doUndo()
{
	textEdit->undo();
}

/* 取消上一个撤消 */
void TextEditer::doRedo()
{
	textEdit->redo();
}

/* 剪切 */
void TextEditer::doCut()
{
	textEdit->cut();
}

/* 复制文本 */
void TextEditer::doCopy()
{
	textEdit->copy();
}

/* 粘贴文本 */
void TextEditer::doPast()
{
	textEdit->paste();
}

/* 全选文本 */
void TextEditer::doSelectAll()
{
	textEdit->selectAll();
}

/* 设置文本的字体 */
void TextEditer::setFontForText()
{
	bool ok;
	const QFont &font = QFontDialog::getFont(&ok, textEdit->font(),
					this, (tr("字体对话框 ")));
	if (ok)
	{
		textEdit->setFont(font);
	}
}
/* 光标改变时设置光标的们位置 */
void TextEditer::doCursorChanged()
{
	int row, col, tmp;
	QTextCursor cursor;
	QString str = "";

	cursor = textEdit->textCursor();	/* 获取文本中的输入光标 */
	col = cursor.columnNumber();	/* 获取列号 */
	row = textEdit->document()->blockCount();

	if (col == 0)
	{
		str = '0' + str;
	}
	while (col)
	{
		tmp = col % 10;
		col /= 10;
		str = (tmp + '0')  + str;
	}
	str = ',' + str;

	if (row == 0)
	{
		str = '0' + str;
	}
	while (row)
	{
		tmp = row % 10;
		row /= 10;
		str = (tmp + '0') + str;
	}

	statusLabelOne->setText(str);
}

/* 响应文本textEdit的contentsChanged() */
void TextEditer::doModified()
{
	QString tmp = curFile;
	int len;

	len = tmp.length();
	if (tmp[len - 1] != '*')
	{
		tmp += '*';
	}
	if (!hasFile)
	{
		curFile = "";
		tmp = "untitled*";
	}
	this->setWindowTitle(tmp);
	statusLabelTwo->setText(tr("正在修改"));
	isSaved = false;	/* 文本文件已经修改 */
}

/* 当前文本文件可能要被保存 */
void TextEditer::mayBeSaved()
{
	if (!hasFile) return;
	if (!isSaved)	/* 当前文件没有保存 */
	{
		QMessageBox box;
		box.setWindowTitle(tr("警告!"));
		box.setIcon(QMessageBox::Warning);
		box.setText(tr("文件尚未保存, 是否保存?"));
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

		if (box.exec() == QMessageBox::Yes)
		{
			doSave();
		}
	}
}

/* 新建文件 */
void TextEditer::doNew()
{
	QString filename;

	mayBeSaved();	/* 当编辑的文本文件可能要保存 */
	textEdit->clear();
	filename = QFileDialog::getSaveFileName(this, tr("新建"),
					"./untitled.txt", tr("Text files (*.txt)"));
	if (!filename.isEmpty() && filename != "")
	{
		saveFile(filename);
		curFile = filename;	/* 当前编辑的文件为filename */
		this->setWindowTitle(curFile);
		isSaved = true;
	}
}

/* 打开文件 */
void TextEditer::doOpen()
{
	QString filename = QFileDialog::getOpenFileName(this);
	curFile = filename;

	qDebug() << filename;

	if (!filename.isEmpty())
	{
		mayBeSaved();	/* 当编辑的文本文件可能要保存 */
		if (loadFile(filename))
		{
			statusLabelTwo->setText(tr("已经读取"));
		}
	}
	textEdit->setVisible(true);
}

/* 把文本文件装载到文本编辑器中 */
bool TextEditer::loadFile(const QString &filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("读取文件"),
			tr("无法读取文件 %1: \n%2.").arg(filename).arg(file.errorString()));

		return false;
	}

	QTextStream in(&file);
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	textEdit->setText(in.readAll());
	//QApplication::restoreOverrideCursor();
	this->setWindowTitle(filename);
	isUntitled = false;
	file.close();;
	isSaved = true;	/* 文体文件已保存 */

	return true;
}

/* 关闭文件 */
void TextEditer::doClose()
{
	mayBeSaved();
	textEdit->setVisible(false);
	curFile = "TextEditer";
}

/* 保存文件 */
void TextEditer::doSave()
{
	qDebug() << curFile;
	if (isUntitled)	/* 未曾保存过 */
	{
		doASave();
	}
	else
	{
		saveFile(curFile);
	}

	isSaved = true;	/* 当前文件已经被保存了 */
	hasFile = true;	/* 有文本文件打开了 */
}

/* 当前文件已修改过, 保存最新的 */
bool TextEditer::saveFile(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		QMessageBox::warning(this, tr("保存文件"),
			tr("无法保存文件 %1: \n%2.").arg(filename).arg(file.errorString()));

		return false;
	}

	QTextStream out(&file);
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	out << textEdit->toPlainText();
	//QApplication::restoreOverrideCursor();

	statusLabelTwo->setText(tr("已经保存"));
	this->setWindowTitle(filename);
	curFile = filename;
	isUntitled = false;
	file.close();
	isSaved = true;	/* 文件已保存 */

	return true;
}

/* 另存为文件 */
void TextEditer::doASave()
{
	QString filename =
			QFileDialog::getSaveFileName(this, tr("另存为..."),
			"./untitled.txt", tr("All files (*.*)"));
	if (!filename.isEmpty())
	{
		saveFile(filename);	/* 保存文件名为filename的文件 */
	}

	isSaved = true;
}

/* 退出文本编辑器 */
void TextEditer::doExit()
{
	doClose();
	curFile = "TextEditer";
}

/* 初始化窗口位置和大小 */
void TextEditer::initLocSize()
{
	this->move(380, 225);
	this->resize(750, 550);
}

/* 初始化状态栏 */
void TextEditer::initStatusBar()
{
	stusBar = new QStatusBar(this);
	statusLabelOne = new QLabel(this);
	statusLabelOne->setMinimumSize(200, 25);
	statusLabelOne->setFrameShape(QFrame::WinPanel);	/* 设置标签的形状 */
	statusLabelOne->setFrameShadow(QFrame::Sunken);	/* 设置标签外框阴影 */

	statusLabelTwo = new QLabel(this);
	statusLabelTwo->setMinimumSize(200, 25);
	statusLabelTwo->setFrameShape(QFrame::WinPanel);	/* 设置标签的形状 */
	statusLabelTwo->setFrameShadow(QFrame::Sunken);	/* 设置标签外框阴影 */

	stusBar->addWidget(statusLabelOne);
	stusBar->addWidget(statusLabelTwo);
	this->setStatusBar(stusBar);
}

/* 初始化中心部件 */
void TextEditer::initCenterWidget()
{
	textEdit = new QTextEdit(this);
	setCentralWidget(textEdit);
}

/* 初始化工具栏 */
void TextEditer::initToolBar()
{
	/* 工具栏默认可放在所有可放工具栏的地方 */
	fileToolBar = new QToolBar(tr("文件栏"), this);	/* 文件工具栏 */
	/* 默认水平管理所添加的Action */
	fileToolBar->addAction(actNew);
	fileToolBar->addAction(actOpen);
	fileToolBar->addAction(actSave);
	fileToolBar->addAction(actASave);

	editToolBar = new QToolBar(tr("编辑栏"), this);	/* 编辑栏 */
	editToolBar->addAction(actUndo);
	editToolBar->addAction(actRedo);
	editToolBar->addAction(actCut);
	editToolBar->addAction(actCopy);
	editToolBar->addAction(actPast);

	this->addToolBar(fileToolBar);
	this->addToolBar(editToolBar);
}

/* 初始化菜单栏 */
void TextEditer::initMenu()
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QMenuBar *bar = this->menuBar();	/* 获取窗口的菜单栏*/

	/* 文件菜单 */
	menuFile = new QMenu(tr("文件(&F)"), this);
	actNew = menuFile->addAction(tr("新建(&N)"));
	actNew->setIcon(QIcon(":/images/new.png"));
	actNew->setShortcut(QKeySequence(tr("Ctrl+N")));	/* 设置新建的快捷键 */
	actOpen = menuFile->addAction(tr("打开(&O)"));
	actOpen->setIcon(QIcon(":/images/open.png"));
	actOpen->setShortcut(QKeySequence(tr("Ctrl+O")));	/* 设置打开的快捷键 */
	actClose = menuFile->addAction(tr("关闭(&C)"));
	mSepOne = menuFile->addSeparator();
	actSave = menuFile->addAction(tr("保存(&S)"));
	actSave->setShortcut(QKeySequence(tr("Ctrl+S")));	/* 设置保存的快捷键 */
	actSave->setIcon(QIcon(":/images/save.png"));
	actASave = menuFile->addAction(tr("另存为...(&A)"));
	actASave->setIcon(QIcon(":/images/asave.png"));
	mSepTwo = menuFile->addSeparator();
	actExit = menuFile->addAction(tr("退出(&X)"));
	bar->addMenu(menuFile);	/* 将生成的文件菜单添加到菜单栏中 */

	/* 编辑菜单 */
	menuEdit = new QMenu(tr("编辑(&E)"), this);
	actUndo = menuEdit->addAction(tr("撤消(&U)"));
	actUndo->setIcon(QIcon(":/images/undo.png"));
	actUndo->setShortcut(QKeySequence(tr("Ctrl+Z")));	/* 设置撤消的快捷键 */
	actRedo = menuEdit->addAction(tr("重做(&R)"));
	actRedo->setShortcut(QKeySequence(tr("Ctrl+R")));	/* 设置重做的快捷键 */
	actRedo->setIcon(QIcon(":/images/editor.png"));
	actCut = menuEdit->addAction(tr("剪切(&T)"));
	actCut->setIcon(QIcon(":/images/cut.png"));
	actCut->setShortcut(QKeySequence(tr("Ctrl+X")));	/* 设置剪切的快捷键 */
	actCopy = menuEdit->addAction(tr("复制(&P)"));
	actCopy->setIcon(QIcon(":/images/copy.png"));
	actCopy->setShortcut(QKeySequence(tr("Ctrl+C")));	/* 设置复制的快捷键 */
	actPast = menuEdit->addAction(tr("粘贴(&P)"));
	actPast->setIcon(QIcon(":/images/paste.png"));
	actPast->setShortcut(QKeySequence(tr("Ctrl+P")));	/* 设置粘贴的快捷键 */
	menuEdit->addSeparator();
	actAll = menuEdit->addAction(tr("全选(&A)"));
	actAll->setShortcut(QKeySequence(tr("Ctrl+A")));	/* 设置全选的快捷键 */
	bar->addMenu(menuEdit);

	/* 工具菜单 */
	menuTool = new QMenu(tr("工具(&T)"), this);
	actFont = menuTool->addAction(tr("字体(&F)"));
	bar->addMenu(menuTool);
}

TextEditer::~TextEditer()
{
	delete ui;
}
