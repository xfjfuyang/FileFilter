#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QtDebug>

QString outputPath;
QString inputPath;
QStringList filter;

void OutputFileInfo(const QString info)
{
     qDebug() << info;
}

void ProcessFileInfo(const QFileInfo info)
{
    QString path = outputPath;
    for (int i = 0; i < filter.size(); i++) {
        QString suffix = filter.at(i).split('.').at(1);
        if (suffix == info.suffix()) {
            path += "/" + suffix;
            QDir subDir(path);
            if (!subDir.exists()) {
                subDir.mkdir(path);
            }
            QString srcPath = info.filePath();
            QString dstPath = path+ "/" +info.fileName();
            bool b = QFile::copy(srcPath, dstPath);
            QString result = QString("%1").arg(b);
            OutputFileInfo("[Copy][" + result + "] " + srcPath + "  ->  " + dstPath);
            continue;
        }
    }
}

void FindAllFiles(const QString path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    QStringList ft;
    QList<QFileInfo> fileInfoList(dir.entryInfoList(ft));
    for(int i = 0;i<fileInfoList.count(); i++) {
        QFileInfo item = fileInfoList.at(i);
        if (item.fileName() == "." || item.fileName() == ".." || item.fileName().at(0) == '.') {
            continue;
        }
        if (item.isDir()) {
            FindAllFiles(item.filePath());
        } else {
            ProcessFileInfo(item);
        }
    }

}

void ConfigInit()
{
    QFile file("config.cfg");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    filter.clear();
    QTextStream infile(&file);
    if (!infile.readLineInto(&inputPath)) {
        return;
    }
    if (!infile.readLineInto(&outputPath)) {
        return;
    }
    QString line;
    while(infile.readLineInto(&line)) {
        filter << line;
    }
    file.close();

    OutputFileInfo(inputPath);
    OutputFileInfo(outputPath);
    return;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConfigInit();
    FindAllFiles(inputPath);
    return a.exec();
}
