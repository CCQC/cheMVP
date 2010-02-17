#include "mainwindow.h"

// TODO Add a warning if the current step is not the final one.
void MainWindow::save()
{
    if (currentSaveFile.isEmpty()) {
        saveAs();
    } else {
        saveImage(currentSaveFile);
    }
}

void MainWindow::saveAs()
{
    currentSaveFile = QFileDialog::getSaveFileName(this);
    if (currentSaveFile.isEmpty())
        return;

    saveImage(currentSaveFile);
}

void MainWindow::saveAndExit()
{
    if(currentSaveFile.size()){
        std::cout<<"Saving file..."<<currentSaveFile.toStdString()<<std::endl;
        save();
        exit(0);
    }
}

void MainWindow::saveImage(const QString &fileName)
{
    canvas->unselectAll();
    
    FileType fileType = determineFileType(fileName);
    QSize imageDimension(canvas->sceneRect().width(), canvas->sceneRect().height());
    // The vector graphics formats still seem to rasterize radial gradients, so I
    // use antialiasing to keep them looking pretty
    if(fileType == SVG){
        QPainter *painter = new QPainter();
        QSvgGenerator *svgGen = new QSvgGenerator();
        svgGen->setSize(5.0*imageDimension);
        svgGen->setFileName(fileName);
        painter->begin(svgGen);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        delete painter;
        delete svgGen;
    }else if(fileType == PNG || fileType == TIFF){
        QPainter *painter = new QPainter();
        QImage *image = new QImage(5.0*imageDimension, QImage::Format_ARGB32);
        painter->begin(image);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        image->save(fileName);
        delete painter;
        delete image;
    }else if(fileType == PDF){
        QPrinter *printer = new QPrinter;
        QPainter *painter = new QPainter;
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setPaperSize(5.0*imageDimension, QPrinter::Point);
        printer->setFullPage(true);
        printer->setOutputFileName(fileName);
        painter->begin(printer);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        delete painter;
        delete printer;
    }else if(fileType == PostScript){
        QPrinter *printer = new QPrinter;
        QPainter *painter = new QPainter;
        printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setPaperSize(5.0*imageDimension, QPrinter::Point);
        printer->setFullPage(true);
        printer->setOutputFileName(fileName);
        painter->begin(printer);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        delete painter;
        delete printer;
    }else if(fileType == CHMVP){
        //processProjectFile(fileName, true); // HPS
    }else{
        QString message("Unsupported file type:\n\n");
        message += fileName;
        message += "\n\nSupported extensions are\n.pdf, .svg, .ps, .eps, .png, .tiff, .tif";
        error(message, __FILE__, __LINE__);
    }
}

MainWindow::FileType MainWindow::determineFileType(const QString &fileName)
{
    QRegExp re(".*\\.png", Qt::CaseInsensitive, QRegExp::RegExp2);
    //	if(re.exactMatch(fileName)) return PNG;
    re.setPattern(".*\\.pdf");
    if(re.exactMatch(fileName)) return PDF;
    re.setPattern(".*\\.svg");
    if(re.exactMatch(fileName)) return SVG;
    re.setPattern(".*\\.ps");
    if(re.exactMatch(fileName)) return PostScript;
    re.setPattern(".*\\.eps");
    if(re.exactMatch(fileName)) return PostScript;
    re.setPattern(".*\\.tif");
    if(re.exactMatch(fileName)) return TIFF;
    re.setPattern(".*\\.tiff");
    if(re.exactMatch(fileName)) return TIFF;
    re.setPattern(".*\\.png");
    if(re.exactMatch(fileName)) return PNG;
    re.setPattern(".*\\.cvp");
    if(re.exactMatch(fileName)) return CHMVP;

    return Unknown;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());
    if(fileName != 0) {
		parser->setFileName(fileName);
    	loadFile();
		recentlyOpenedFiles.removeAll(fileName);
		recentlyOpenedFiles.prepend(fileName);
		updateRecentFiles();
    }
}

void MainWindow::updateRecentFiles()
{
    QMutableStringListIterator i(recentlyOpenedFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }	
    for (int j = 0; j < MAX_RECENT_FILES; j++) {
        if (j < recentlyOpenedFiles.size()) {
#ifdef Q_WS_WIN			
			QString front = recentlyOpenedFiles[j].left(recentlyOpenedFiles[j].lastIndexOf("\\"));
			QString folder = front.right(front.length() - front.lastIndexOf("\\")-1);
			QString file = recentlyOpenedFiles[j].right(recentlyOpenedFiles[j].length()
														-recentlyOpenedFiles[j].lastIndexOf("\\")-1);
			QString text = tr("&%1 %2").arg(j + 1).arg(file + " - " + folder);
#endif
#ifndef Q_WS_WIN
			QString front = recentlyOpenedFiles[j].left(recentlyOpenedFiles[j].lastIndexOf("/"));
			QString folder = front.right(front.length() - front.lastIndexOf("/")-1);
			QString file = recentlyOpenedFiles[j].right(recentlyOpenedFiles[j].length()
														-recentlyOpenedFiles[j].lastIndexOf("/")-1);
			QString text = tr("&%1 %2").arg(j + 1).arg(file + " - " + folder);
#endif
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentlyOpenedFiles.at(j));
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentlyOpenedFiles.isEmpty());
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
	if(action) {
		QString fileName = action->data().toString();
		if(QFile::exists(fileName)) {	
			parser->setFileName(fileName);
			loadFile();
			recentlyOpenedFiles.removeAll(fileName);
			recentlyOpenedFiles.prepend(fileName);
			updateRecentFiles();		
		} else {
			error("Unable to open " + fileName + ". The file has been moved.");
			updateRecentFiles();
		}
	}
}
							
void MainWindow::loadFile()
{
    if (!parser->fileName().isEmpty()) {
        parser->readFile();
        canvas->clearAll();
        canvas->loadFromParser();
        setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));

        // Enable the widgets in the animation tab if there are multiple geometries
        if (parser->numMolecules() <= 1)
            animationWidget->setEnabled(false);
        else
            animationWidget->setEnabled(true);
        
        // Set the sliders range and current value.
        animationSlider->setRange(0, parser->numMolecules() - 1);
        animationSlider->setValue(parser->current());
    }
    else
    {
        std::cout << "FAILURE IN FILENAME" << std::endl;
    }
}

//void MainWindow::processProjectFile(const QString &fileName, bool saveFile)
//{
//    // This function will call all of the objects in turn to get/put settings
//    // The read and write functions are written together for convenience
//
//    QSettings settings(fileName, QSettings::IniFormat);
//    // DrawingInfo
//    settings.beginGroup("drawingInfo");
//    drawingInfo->processProjectFile(settings, saveFile);
//    settings.endGroup();
//
//    // The drawing canvas settings
//    settings.beginGroup("canvas");
//    canvas->processProjectFile(settings, saveFile);
//    settings.endGroup();
//}

void MainWindow::saveProject()
{
	QString filename = QFileDialog::getSaveFileName(this);
    if(filename.isEmpty())
        return;
	if(!filename.endsWith(".chmvp"))
		filename += ".chmvp";
	
	QFile file(filename);
	if(file.exists())
    {
	    // Some prompt for overwrite?
		return;
    }
	if(!file.open(QIODevice::WriteOnly))
	{
		// error
	    return;
	}

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("cheMVP");
	writer.writeAttribute("version", CHEMVP_VERSION);
	parser->serialize(&writer);
	drawingInfo->serialize(&writer);
	canvas->serialize(&writer);
	writer.writeEndDocument();

	file.close();
}

void MainWindow::openProject()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open cheMVP Project File"), QDir::homePath(), "*.chmvp");
	if(filename.isEmpty())
        return;
	if(!filename.endsWith(".chmvp"))
		return;

	QFile file(filename);
	if(!file.exists())
    {
	    // error
		return;
    }
	if(!file.open(QIODevice::ReadOnly))
	{
		// error
	    return;
	}
	
	QXmlStreamReader reader(&file);
	reader.readNextStartElement();
	if(reader.attributes().value("version").toString() != CHEMVP_VERSION)
		error("Invalid Version Number!");
	
	parser = FileParser::deserialize(&reader);
	drawingInfo = DrawingInfo::deserialize(&reader);
	canvas = DrawingCanvas::deserialize(&reader, itemMenu, drawingInfo, parser);
	
	if(reader.hasError())
		error("Reader error");
}