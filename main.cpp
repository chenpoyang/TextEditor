#include <QtGui/QApplication>
#include <QTextCodec>
#include "textediter.h"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(mainwindow);	/* 初始化资源文件 */
	QApplication app(argc, argv);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

	TextEditer textEdit;
	textEdit.show();

	return app.exec();
}
