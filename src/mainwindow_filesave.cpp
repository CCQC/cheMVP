#include "mainwindow.h"

void MainWindow::save()
{
    if (currentSaveFile.isEmpty()) {
        saveAs();
    } else {
        saveImage(currentSaveFile);
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
	
	return Unknown;
}


void MainWindow::saveAs()
{
    currentSaveFile = QFileDialog::getSaveFileName(this);
    if (currentSaveFile.isEmpty())
        return;

    saveImage(currentSaveFile);
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
		QImage *image = new QImage(3.0*imageDimension, QImage::Format_ARGB32);
		// Resolution of the image is 600 dpi by default
		image->setDotsPerMeterX(600*254);
		image->setDotsPerMeterY(600*254);
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
	}else{
		QString message("Unsupported file type:\n\n");
		message += fileName;
		message += "\n\nSupported extensions are\n.pdf, .svg, .ps, .eps";
		error(message, __FILE__, __LINE__);
	}
}


