#include "mainwindow.h"
#include <QPrinter>

void MainWindow::save()
{
    if (currentSaveFile.isEmpty()) {
        saveAs();
    } else {
        if (currentSaveFile.endsWith(".chmvp")) {
            saveProject(currentSaveFile);
        } else {
            saveImage(currentSaveFile);
        }
    }
}

void MainWindow::saveAs()
{
    QFileDialog *saveAsDialog = new QFileDialog(this,
                                                "Save File As",
                                                QDir::homePath(),
                                                "CheMVP Project File (*.chmvp);;Encapsulated "
                                                "PostScript (*.eps);;Portable Document Format "
                                                "(*.pdf);;Portable Network Graphic "
                                                "(*.png);;PostScript (*.ps);;Scalable Vector "
                                                "Graphic (*.svg);;Tagged Image File Format "
                                                "(*.tiff)");
    saveAsDialog->setAcceptMode(QFileDialog::AcceptSave);
    saveAsDialog->setFileMode(QFileDialog::AnyFile);
    if (saveAsDialog->exec()) {
        currentSaveFile = saveAsDialog->selectedFiles().at(0);
        if (currentSaveFile.isEmpty()) {
            return;
        }

        QString selectedExtension = saveAsDialog->selectedNameFilter();
        selectedExtension = selectedExtension.left(selectedExtension.length() - 1);
        selectedExtension = selectedExtension.right(selectedExtension.length() -
                                                    selectedExtension.lastIndexOf(".") - 1);

        if (currentSaveFile.indexOf(".") == -1) {
            currentSaveFile += selectedExtension;
        } else {
            QString extension = currentSaveFile.right(currentSaveFile.length() -
                                                      currentSaveFile.lastIndexOf(".") - 1);

            if (extension != QString("pdf") && extension != QString("svg") &&
                extension != QString("ps") && extension != QString("tif") &&
                extension != QString("tiff") && extension != QString("eps") &&
                extension != QString("png") && extension != QString("chmvp")) {
                QMessageBox *error = new QMessageBox(this);
                error->setText("The specified file type is invalid!");
                error->setIcon(QMessageBox::Critical);
                error->setInformativeText("Please choose a valid file type.");
                error->setStandardButtons(QMessageBox::Ok);
                error->exec();
                delete error;
                MainWindow::saveAs();
                delete saveAsDialog;
                return;
            } else if (extension != selectedExtension) {
                QMessageBox *error = new QMessageBox(this);
                error->setText(QString("You cannot save this document with extension \".%1\" at "
                                       "the end of the name. The required extension is \".%2\".")
                                   .arg(extension)
                                   .arg(selectedExtension));
                error->setIcon(QMessageBox::Critical);
                error->setInformativeText(
                    QString(
                        "You can choose to use both, so that your file name ends in \".%1.%2.\"")
                        .arg(extension)
                        .arg(selectedExtension));
                error->setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                                          QMessageBox::Cancel);
                error->setButtonText(QMessageBox::Save, "Use both");
                error->setButtonText(QMessageBox::Discard,
                                     QString("Use .%1").arg(selectedExtension));
                int answer = error->exec();
                delete error;
                if (answer == QMessageBox::Save) {
                    currentSaveFile += "." + selectedExtension;
                } else if (answer == QMessageBox::Discard) {
                    currentSaveFile = currentSaveFile.left(currentSaveFile.lastIndexOf("."));
                    currentSaveFile += "." + selectedExtension;
                } else {
                    return;
                }
            }
        }

        if (currentSaveFile.endsWith(".chmvp")) {
            saveProject(currentSaveFile);
        } else {
            saveImage(currentSaveFile);
        }
    }
    delete saveAsDialog;
}

void MainWindow::saveAndExit()
{
    if (currentSaveFile.size()) {
        std::cout << "Saving file..." << currentSaveFile.toStdString() << std::endl;
        save();
        exit(0);
    }
}

void MainWindow::saveImage(const QString &fileName)
{
    canvas->unselectAll();

    FileType fileType = determineFileType(fileName);
    QSize imageDimension(canvas->sceneRect().width(), canvas->sceneRect().height());

    QPainter *painter = new QPainter();
    QPrinter *printer = new QPrinter();
    printer->setPaperSize(5.0 * imageDimension, QPrinter::Point);
    printer->setFullPage(true);
    printer->setOutputFileName(fileName);

    // The vector graphics formats still seem to rasterize radial gradients, so I
    // use antialiasing to keep them looking pretty
    if (fileType == SVG) {
        QSvgGenerator *svgGen = new QSvgGenerator();
        svgGen->setSize(5.0 * imageDimension);
        svgGen->setFileName(fileName);
        painter->begin(svgGen);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        delete svgGen;
    } else if (fileType == PNG || fileType == TIFF) {
        QImage *image = new QImage(5.0 * imageDimension, QImage::Format_ARGB32);
        painter->begin(image);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
        image->save(fileName);
        delete image;
    } else if (fileType == PDF) {
        printer->setOutputFormat(QPrinter::PdfFormat);
        painter->begin(printer);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
    } else if (fileType == PostScript) {
        // printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFormat(QPrinter::PdfFormat);
        painter->begin(printer);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        canvas->render(painter);
        painter->end();
    } else {
        QString message("Unsupported file type:\n\n");
        message += fileName;
        message += "\n\nSupported extensions are\n.pdf, .svg, .ps, .eps, .png, .tiff, .tif, .chmvp";
        error(message, __FILE__, __LINE__);
        return;
    }

    delete painter;
    delete printer;
}

MainWindow::FileType MainWindow::determineFileType(const QString &fileName)
{
    QRegExp re(".*\\.pdf", Qt::CaseInsensitive, QRegExp::RegExp2);
    if (re.exactMatch(fileName)) {
        return PDF;
    }
    re.setPattern(".*\\.svg");
    if (re.exactMatch(fileName)) {
        return SVG;
    }
    re.setPattern(".*\\.ps");
    if (re.exactMatch(fileName)) {
        return PostScript;
    }
    re.setPattern(".*\\.eps");
    if (re.exactMatch(fileName)) {
        return PostScript;
    }
    re.setPattern(".*\\.tif");
    if (re.exactMatch(fileName)) {
        return TIFF;
    }
    re.setPattern(".*\\.tiff");
    if (re.exactMatch(fileName)) {
        return TIFF;
    }
    re.setPattern(".*\\.png");
    if (re.exactMatch(fileName)) {
        return PNG;
    }

    return Unknown;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath());
    if (fileName != 0) {
        if (fileName.endsWith(".chmvp")) {
            openProject(fileName);
            currentSaveFile = fileName;
        } else {
            parser->setFileName(fileName);
            loadFile();
            currentSaveFile = "";
        }
        recentlyOpenedFiles.removeAll(fileName);
        recentlyOpenedFiles.prepend(fileName);
        updateRecentFiles();
    }
}

void MainWindow::updateRecentFiles()
{
    QMutableStringListIterator i(recentlyOpenedFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next())) {
            i.remove();
        }
    }
    for (int j = 0; j < MAX_RECENT_FILES; j++) {
        if (j < recentlyOpenedFiles.size()) {
#ifdef Q_WS_WIN
            QString s = "\\";
#endif
#ifndef Q_WS_WIN
            QString s = "/";
#endif
            QString front = recentlyOpenedFiles[j].left(recentlyOpenedFiles[j].lastIndexOf(s));
            QString folder = front.right(front.length() - front.lastIndexOf(s) - 1);
            QString file = recentlyOpenedFiles[j].right(recentlyOpenedFiles[j].length() -
                                                        recentlyOpenedFiles[j].lastIndexOf(s) - 1);
            QString text = tr("&%1 %2").arg(j + 1).arg(file + " - " + folder);
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
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString fileName = action->data().toString();
        if (QFile::exists(fileName)) {
            if (fileName.endsWith(".chmvp")) {
                openProject(fileName);
                currentSaveFile = fileName;
            } else {
                parser->setFileName(fileName);
                loadFile();
                currentSaveFile = "";
            }
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
        if (parser->fileName().endsWith(".chmvp")) {
            openProject(parser->fileName());
            return;
        }
        parser->readFile();
        canvas->clearAll();

        DrawingCanvas *old_canvas = canvas;
        DrawingInfo *old_info = drawingInfo;
        QGraphicsView *old_view = view;

        drawingInfo = new DrawingInfo();
        // Includes loading canvas from parser
        canvas = new DrawingCanvas(this->drawingInfo, this->parser);

        setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));

        this->view = new DrawingDisplay(canvas, drawingInfo);
        view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        view->setGeometry(
            0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
        //		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Causes display issues
        // on load
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        resetToolBox(NULL);
        resetSignalsOnFileLoad();

        // Enable the widgets in the animation tab if there are multiple geometries
        if (parser->numMolecules() <= 1) {
            animationWidget->setEnabled(false);
        } else {
            animationWidget->setEnabled(true);
        }

        // Set the sliders range and current value.
        animationSlider->setRange(0, parser->numMolecules() - 1);
        animationSlider->setValue(parser->current());

        QHBoxLayout *layout = new QHBoxLayout;
        QByteArray state = splitter->saveState();
        QSplitter *old_splitter = splitter;
        splitter = new QSplitter(Qt::Horizontal);
        splitter->addWidget(view);
        splitter->addWidget(toolBox);
        splitter->restoreState(state);
        layout->addWidget(splitter);

        QWidget *widget = new QWidget;
        widget->setLayout(layout);
        this->setCentralWidget(widget);

        delete old_info;
        delete old_view;
        delete old_splitter;
        delete old_canvas;

        activateToolBar();
    }
}

void MainWindow::saveProject(QString filename)
{
    if (filename.isEmpty()) {
        return;
    }
    if (!filename.endsWith(".chmvp")) {
        filename += ".chmvp";
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
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

    if (file.exists()) {
        recentlyOpenedFiles.removeAll(filename);
        recentlyOpenedFiles.prepend(filename);
        updateRecentFiles();
    }
}

void MainWindow::openProject(QString filename, bool onNewMainWindow)
{
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    if (!file.exists()) {
        // error
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        // error
        return;
    }

    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    if (reader.attributes().value("version").toString() != CHEMVP_VERSION) {
        error("Invalid Version Number!");
        return;
    }

    DrawingCanvas *old_canvas = canvas;
    DrawingInfo *old_info = drawingInfo;
    QGraphicsView *old_view = view;
    FileParser *old_parser = parser;

    // Deserialize
    this->parser = FileParser::deserialize(&reader);
    this->drawingInfo = DrawingInfo::deserialize(&reader);
    this->canvas = DrawingCanvas::deserialize(&reader, drawingInfo, parser);

    setWindowTitle(tr("%1 - cheMVP").arg(filename));

    this->view = new DrawingDisplay(canvas, drawingInfo);
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    view->setGeometry(
        0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QMap<QString, QString> *options = new QMap<QString, QString>();

    // Appearance
    options->insert("FOGGING_ON", QString("%1").arg(drawingInfo->getUseFogging()));
    options->insert("FOGGING_SCALE", QString("%1").arg(drawingInfo->getFoggingScale()));
    options->insert("X_ROTATION", "0"); // Where are these stored?
    options->insert("Y_ROTATION", "0"); // They aren't.
    options->insert("Z_ROTATION", "0"); // Why not? // No idea.
    options->insert("BACKGROUND_OPACITY", QString("%1").arg(canvas->getBackgroundOpacity()));
    options->insert("ZOOM", QString("%1").arg(drawingInfo->getZoom()));

    // Bonds and Angles
    options->insert("BOND_LABEL_PRECISION", QString("%1").arg(drawingInfo->getBondPrecision()));
    options->insert("ANGLE_LABEL_PRECISION", QString("%1").arg(drawingInfo->getAnglePrecision()));

    // Atoms
    options->insert("ATOM_DRAWING_STYLE", QString("%1").arg(drawingInfo->getDrawingStyle()));
    options->insert("ATOM_LABEL_SIZE", QString("%1").arg(Atom::SmallLabel));

    resetToolBox(options);

    animationSlider->blockSignals(true);

    // Update toolbox widgets
    if (parser->numMolecules() <= 1) {
        animationWidget->setEnabled(false);
    } else {
        animationWidget->setEnabled(true);
    }
    animationSlider->setRange(0, parser->numMolecules() - 1);
    animationSlider->setValue(parser->current());

    animationSlider->blockSignals(false);

    // Refresh layout
    QHBoxLayout *layout = new QHBoxLayout;
    QByteArray state = splitter->saveState();
    QSplitter *old_splitter = splitter;
    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(view);
    splitter->addWidget(toolBox);
    if (!onNewMainWindow) {
        splitter->restoreState(state);
    }
    layout->addWidget(splitter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    this->setCentralWidget(widget);

    resetSignalsOnFileLoad();

    reader.readNextStartElement();
    if (reader.hasError()) {
        error("Reader: " + reader.errorString());
    } else if (reader.name() != "cheMVP") {
        error("Full document not parsed!");
    }

    delete old_parser;
    delete old_info;
    delete old_view;
    delete old_splitter;
    delete old_canvas;

    activateToolBar();
}
