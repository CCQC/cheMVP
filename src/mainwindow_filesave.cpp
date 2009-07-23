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
	
	if(fileType == SVG){
		QPainter *painter = new QPainter();
		QSvgGenerator *svgGen = new QSvgGenerator();
		svgGen->setSize(imageDimension);
		svgGen->setFileName(fileName);
		painter->begin(svgGen);
		canvas->render(painter);
		painter->end();
		delete painter;
		delete svgGen;
	}else if(fileType == PNG || fileType == TIFF){
		QPainter *painter = new QPainter();
		QImage *image = new QImage(5.0*imageDimension, QImage::Format_ARGB32);
		// Resolution of the image is 600 dpi by default
		painter->begin(image);
		canvas->render(painter);
		painter->end();
		image->save(fileName);
		delete painter;
		delete image;
	}else if(fileType == PDF){
	    QPrinter *printer = new QPrinter;
	    QPainter *painter = new QPainter;
	    printer->setOutputFormat(QPrinter::PdfFormat);
	    printer->setPaperSize(imageDimension, QPrinter::Point);
	    printer->setFullPage(true);
	    printer->setOutputFileName(fileName);
	    painter->begin(printer);
		canvas->render(painter);
		painter->end();
		delete painter;
		delete printer;
	}else if(fileType == PostScript){
	    QPrinter *printer = new QPrinter;
	    QPainter *painter = new QPainter;
	    printer->setOutputFormat(QPrinter::PostScriptFormat);
	    printer->setPaperSize(imageDimension, QPrinter::Point);
	    printer->setFullPage(true);
	    printer->setOutputFileName(fileName);
	    painter->begin(printer);
		canvas->render(painter);
		painter->end();
		delete painter;
		delete printer;
	}else if(fileType == CVP){
		processProjectFile(fileName, true);
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
	if(re.exactMatch(fileName)) return CVP;
	
	return Unknown;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    parser->setFileName(fileName);
    loadFile();
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
}

void MainWindow::processProjectFile(const QString &fileName, bool saveFile)
{
	// This function will call all of the objects in turn to get/put settings
	// The read and write functions are written together for convenience
	
	QSettings settings(fileName, QSettings::IniFormat);
	// DrawingInfo
	settings.beginGroup("drawingInfo");
	drawingInfo->processProjectFile(settings, saveFile);
	settings.endGroup();

	// The drawing canvas settings
	settings.beginGroup("canvas");
	canvas->processProjectFile(settings, saveFile);
	settings.endGroup();
}