#include "imageworker.h"
#include "setting.h"
#include "ui_setting.h"
#include "settingmanager.h"
#include "tgridlayout.h"
#include "backgroundframe.h"

QList<CustomToolButton*> toolList;


Setting::Setting(SettingsManager* manager,QWidget *parent)
    : TWidget(parent)
    , ui(new Ui::Setting),
    m_Manager(manager)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    ui->stackedWidget->setBlurEffectEnabled(false);
    ui->stackedWidget->setAnimationType(TStackedWidget::VerticalSlide);
    ui->userPathLineEdit->setReadOnly(true);
    ui->backgroundScroll->setWidget(ui->backgroundDisplay);

    Init();

    if(!m_Manager) return;
    build();
}

Setting::~Setting()
{
    delete ui;
    delete labelLayout;
    delete effect;
    m_Manager=nullptr;
    delete m_Manager;
    /*for(auto &label : m_list){
        if(label) delete label;
    }*/
}

void printGridLayoutInfo(QGridLayout *layout) {
    if (!layout) {
        qDebug() << "The layout is null.";
        return;
    }

    int totalRows = layout->rowCount();
    int totalColumns = layout->columnCount();
    int totalItems = totalRows * totalColumns;
    int index = 0;

    // Use a single while loop to iterate over the grid layout
    while (index < totalItems) {
        int row = index / totalColumns;
        int col = index % totalColumns;

        QLayoutItem *item = layout->itemAtPosition(row, col);
        if (item) {
            QWidget *widget = item->widget();

            if (widget) {
                // Get the widget's position in the layout
                int startRow, startColumn, rowSpan, columnSpan;
                layout->getItemPosition(layout->indexOf(widget), &startRow, &startColumn, &rowSpan, &columnSpan);

                // Display the information
                qDebug() << "Widget:" << widget
                         << "Position: (" << startRow << "," << startColumn << ")"
                         << "RowSpan:" << rowSpan
                         << "ColumnSpan:" << columnSpan;
            }
        }
        ++index;
    }
}


void Setting::addBackground() {
    QString filepath = QFileDialog::getOpenFileName(nullptr, "Choisir une image", "", "Images (*.png *.jpg *.jpeg)");
    emit startLoading("selecting image");

    if (filepath.isEmpty()) {
        emit stopLoading();
        return;
    }

    emit startLoading("applying Blur....");

    QThread *workerThread= new QThread(nullptr);
    ImageWorker* worker= new ImageWorker(filepath,m_Manager->userDataPath(),nullptr);
    worker->moveToThread(workerThread);
    worker->setWorkToDO(ImageWorker::gaussianblur);

    if(ui->blurCheckBox->isChecked()){
        worker->setRadius(ui->blurSlider->value());
    }

    connect(workerThread,&QThread::started,worker,&ImageWorker::run);

    connect(worker, &ImageWorker::imageReady, this, [this](const QString &fileName) {
        BackgroundFrame* label = new BackgroundFrame(ui->backgroundDisplay);
        label->setPixmap(QPixmap(fileName));
        label->setText(fileName);
        label->setBaseSize(160,80);
        label->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);

        labelLayout->addWidget(label);

        connect(label, &BackgroundFrame::Clicked, this, [=] {
            QFileInfo inf(fileName);
            ui->backgroundImageNameLabel->setText(inf.fileName());
            labelLayout->rearrangeWidgets();
        });
        connect(label, &BackgroundFrame::kill, this, [label,this] {
            labelLayout->removeWidgetAnimated(label);
        });
    });

    connect(workerThread, &QThread::finished, this, [this,worker, workerThread]() {

        emit stopLoading();
        worker->deleteLater();
        workerThread->deleteLater();
    });

    workerThread->start(QThread::HighestPriority);

}

void Setting::showPanel()
{
    QParallelAnimationGroup* group=new QParallelAnimationGroup;
    ui->frame->show();
    QPropertyAnimation* panelAnim=new QPropertyAnimation(ui->frame);
    panelAnim->setDuration(m_Manager->animationDuration());
    panelAnim->setStartValue(QRect(ui->frame->x()-ui->frame->width(),ui->frame->y(),ui->frame->width(),ui->frame->height()));
    panelAnim->setEndValue(QRect(ui->frame->x()+ui->frame->width(),ui->frame->y(),ui->frame->width(),ui->frame->height()));
    panelAnim->setEasingCurve(m_Manager->easingCurve());
    group->addAnimation(panelAnim);

    QPropertyAnimation* widgetAnim=new QPropertyAnimation(ui->stackedWidget,"geometry");
    widgetAnim->setDuration(m_Manager->animationDuration());
    widgetAnim->setStartValue(ui->stackedWidget->geometry());
    widgetAnim->setEndValue(QRect(ui->stackedWidget->x()+ui->frame->width()+5,ui->stackedWidget->y(),ui->stackedWidget->width()-ui->frame->width()-5,ui->frame->height()));
    widgetAnim->setEasingCurve(m_Manager->easingCurve());
    group->addAnimation(widgetAnim);

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Setting::hidePanel()
{
    QParallelAnimationGroup* group=new QParallelAnimationGroup;

    QPropertyAnimation* panelAnim=new QPropertyAnimation(ui->frame,"geometry");
    panelAnim->setDuration(m_Manager->animationDuration()-150);
    panelAnim->setStartValue(ui->frame->geometry());
    panelAnim->setEndValue(QRect(ui->frame->x()-ui->frame->width(),ui->frame->y(),ui->frame->width(),ui->frame->height()));
    panelAnim->setEasingCurve(m_Manager->easingCurve());
    group->addAnimation(panelAnim);

    QPropertyAnimation* widgetAnim=new QPropertyAnimation(ui->stackedWidget,"geometry");
    widgetAnim->setDuration(m_Manager->animationDuration()-150);
    widgetAnim->setStartValue(ui->stackedWidget->geometry());
    widgetAnim->setEndValue(QRect(ui->stackedWidget->x()-ui->frame->width()-5,ui->stackedWidget->y(),ui->stackedWidget->width()+ui->frame->width()+5,ui->frame->height()));
    widgetAnim->setEasingCurve(m_Manager->easingCurve());
    group->addAnimation(widgetAnim);

    connect(group,&QParallelAnimationGroup::finished,this,[this]{
        ui->stackedWidget->adjustSize();
        ui->frame->hide();
        ui->gridLayout->update();
    });
    group->start(QAbstractAnimation::DeleteWhenStopped);
}


void Setting::addEasingCurvesToComboBox(QComboBox *comboBox) {
    if (!comboBox) return;
    for (auto it = easingTypes.begin(); it != easingTypes.end(); ++it) {
        comboBox->addItem(it.value(), it.key());
    }
}

void Setting::showNotification(){
    manage(ui->notifButton);
    if(ui->stackedWidget->currentIndex()==1) return;
    ui->stackedWidget->setCurrentIndex(1);
}

void Setting::showAppareance(){
    manage(ui->appareanceButton);
    if(ui->stackedWidget->currentIndex()==0) return;
    ui->stackedWidget->setCurrentIndex(0);
}

void Setting::showUtilities(){
    manage(ui->utilitiesButton);
    if(ui->stackedWidget->currentIndex()==2) return;
    ui->stackedWidget->setCurrentIndex(2);
}

void Setting::showUserInfo(){
    manage(ui->userInfoButton);
    if(ui->stackedWidget->currentIndex()==3) return;
    ui->stackedWidget->setCurrentIndex(3);
}

void Setting::setupDesign()
{
    backgroundList = listLargeImages(":/icons");

    ui->coverFrame->setBackgroundImage(":/icons/bg.png");
    ui->usernameLineEdit->setText(m_Manager->getUsername());
    ui->emailLineEdit->setText(m_Manager->getEmail());
    ui->passwordLineEdit->setText("password134");

    ui->usernameLineEdit->setReadOnly(true);
    ui->emailLineEdit->setReadOnly(true);
    ui->passwordLineEdit->setReadOnly(true);

    //addSongs(":/Songs", ui->songComboBox);

    loadNextBackgroundImage(0);
}

void Setting::loadNextBackgroundImage(int index)
{
    if (index >= backgroundList.size()) {
        return; // No more images to load, exit the function
    }
    if(index>=5) return;

    // Create a new BackgroundFrame
    BackgroundFrame *label = new BackgroundFrame(ui->backgroundDisplay);
    QString path = backgroundList.at(index);
    label->setText(path);
    label->setMaximumSize(160,80);
    label->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    label->setPixmap(QPixmap(path));

    labelLayout->addWidget(label);

    // Connect label click signal
    connect(label, &BackgroundFrame::Clicked, this, [this, path] {
        QFileInfo fileInfo(path);
        ui->backgroundImageNameLabel->setText(fileInfo.baseName());
        labelLayout->rearrangeWidgets();
    });

    connect(label, &BackgroundFrame::kill, this, [label, this] {
        labelLayout->removeWidgetAnimated(label);
    });


    // Schedule next background image load
    QTimer::singleShot(10, this, [this, index] { loadNextBackgroundImage(index + 1); });
}


void Setting::play()
{
    QTimer* timer=new QTimer(this);
    ui->playButton->setIcon(QIcon(":/icons/pausewhite"));
    effect->setSource(QUrl::fromLocalFile(":/Songs/"+ui->songComboBox->currentText()));
    effect->setVolume(m_Manager->notificationVolume()); // Réglez le volume selon vos besoins

    effect->play();
    connect(timer, &QTimer::timeout, this, [this,timer] {
        ui->playButton->setIcon(QIcon(":/icons/resumewhite"));
        timer->stop();
        delete timer;
    });
    timer->start(m_Manager->notificationDuration());
}

void Setting::addSongs(const QString &resourcePath, QComboBox *comboBox)
{
    QString path = resourcePath;
    if (!path.startsWith(":/")) {
        path.prepend(":/");
    }

    QDir resourceDir(path);
    if (!resourceDir.exists()) {
        qDebug() << "Resource path does not exist:" << path;
        return;
    }

    QStringList files = resourceDir.entryList(QDir::Files);
    comboBox->clear();

    // Start the recursive file adding process
    addSongRecursive(files, comboBox, 0);
}

void Setting::addSongRecursive(const QStringList &files, QComboBox *comboBox, int index)
{
    if (index >= files.size()) {
        return; // No more files to add
    }

    // Add a song to the combo box
    comboBox->addItem(files.at(index));

    // Schedule adding the next song after a short delay (optional)
    QTimer::singleShot(500, this, [this, files, comboBox, index] {
        addSongRecursive(files, comboBox, index + 1);
    });
}

void Setting::manage(CustomToolButton *button)
{
    short y = button->y();
    short chunkPos = ui->selectWidget->chunkPos();
    // Calcul de la nouvelle position pour le chunk
    short newPos = y-5;
    // Déplacement du chunk à la nouvelle position
    if (newPos < chunkPos) {
        ui->selectWidget->moveUp(chunkPos - newPos);
    } else {
        ui->selectWidget->moveDown(newPos - chunkPos);
    }
}

void Setting::setManager(SettingsManager *m)
{
    if(m){
        m_Manager=m;
        build();
    }
}

bool Setting::checkImageWidth(const QString &resourcePath) {
    QImage image(resourcePath);
    if (image.isNull()) {
        qDebug() << "Failed to load image:" << resourcePath;
        return false;
    }
    return image.width() >= 800;
}

void addImagesFromDirectory(QStringList &fileList, const QString &path){

    QStringList imageExtensions;
    imageExtensions << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.tiff" << "*.svg";

    QDir dir(path);

    if (!dir.exists()) {
        qWarning() << "Directory does not exist:" << path;
        return;
    }

    foreach (const QString &ext, imageExtensions) {
        QStringList imageFiles = dir.entryList(QStringList() << ext, QDir::Files);

        foreach (const QString &file, imageFiles) {
            QFileInfo fileInfo(dir.filePath(file));
            if (fileInfo.exists() && fileInfo.isFile()) {
                fileList.append(fileInfo.absoluteFilePath());
            }
        }
    }
}

QStringList Setting::listLargeImages(const QString &resourceNamespace) {
    QStringList names;
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        if ((filePath.endsWith(".jpg", Qt::CaseInsensitive) || filePath.endsWith(".png", Qt::CaseInsensitive))
            && filePath.startsWith(resourceNamespace)) {
            if (checkImageWidth(filePath)) {
                names.append(filePath);
            }
        }
    }
    addImagesFromDirectory(names,m_Manager->userDataPath());

    return names;
}

void Setting::setPicture(QPixmap p){
    ui->pictureLabel->setPixmap(p);
}

void Setting::loadSettingToUi()
{

    ui->transparencyButton->setChecked(m_Manager->transparency());
    ui->disableAnimationButton->setChecked(m_Manager->animationEnabled());
    ui->animationDurationSpinBox->setValue(m_Manager->animationDuration());

    ui->animationFunctionComboBox->setCurrentText(easingTypes.value(m_Manager->easingCurve().type()));
    ui->fontComboBox->setCurrentFont(m_Manager->fontFamily());
    ui->fontSizeSpinBox->setValue(m_Manager->fontSize());

    if(m_Manager->customColor()){
        ui->customBackgroundColorRadio->click();
    }
    if(m_Manager->customImage()){
        ui->customBackgroundImageRadio->click();
    }
    ui->customBackgroundColorRadio->setChecked(m_Manager->customColor());
    ui->customBackgroundImageRadio->setChecked(m_Manager->customImage());
    ui->backgroundImageNameLabel->setText(m_Manager->backgroundImage().replace(":/icons/",""));

    //notif
    ui->useSystemNotifButton->setChecked(m_Manager->useSystemNotificationPage());

    ui->popoutNotifPageButton->setChecked(m_Manager->poppoutNotificationPage());

    ui->enableNotifCheck->setChecked(m_Manager->enableNotification());
    ui->notifAlertFrame->setVisible(m_Manager->enableNotification());

    ui->songComboBox->setCurrentText(m_Manager->notificationSon());
    ui->durationSlider->setValue(m_Manager->notificationDuration()/1000);
    ui->volumeSlider->setValue(m_Manager->notificationVolume()*100);

    //utility
    ui->deleteAfterUploadButton->setChecked(m_Manager->deleteAfterUpload());
    ui->daystoDeleteSpinBox->setValue(m_Manager->deleteFileAfter());
    ui->trashMaxSizeSpinbox->setValue(m_Manager->trashMaxSize());
    ui->synchronizedSettingButton->setChecked(m_Manager->synchronize());
    ui->startLazyDownloadCheck->setChecked(m_Manager->startDownloadStartup());
    ui->uploadSpeedLimitSpinBox->setValue(m_Manager->uploadSpeedLimit());
    ui->downloadSpeedLimitSpinBox->setValue(m_Manager->downloadSpeedLimit());
    ui->uploadLimitSpinBox->setValue(m_Manager->uploadLimit());
    ui->downloadLimitSpinBox->setValue(m_Manager->downloadLimit());

    ui->disableTrashCheck->setChecked(m_Manager->enableTrash());
    ui->trashFrame->setVisible(m_Manager->enableTrash());

    ui->launchOnStartupButton->setChecked(m_Manager->launchOnStartUp());
    ui->automaticallyDeleteThan5->setChecked(m_Manager->deleteGreaterThan5());    

    //usersection
    ui->changeUserDataPathCheckbox->setChecked(m_Manager->changeUserDataPath());
    ui->userPathLineEdit->setText(m_Manager->userDataPath());
    ui->pictureLabel->setPixmap(QPixmap(m_Manager->defaultProfilePicture()));

}

void Setting::Init()
{
    effect= new QSoundEffect(this);

    ui->selectedColorFrame->SetSimpleType(true);
    addEasingCurvesToComboBox(ui->animationFunctionComboBox);
    ui->blurFrame->setVisible(ui->blurCheckBox->isChecked());
    connect(ui->blurCheckBox,&QCheckBox::clicked,ui->blurFrame,&QFrame::setVisible);

    labelLayout=new TGridLayout(ui->backgroundDisplay);
    ui->backgroundDisplay->setLayout(labelLayout);

    connect(this,&TWidget::resizing,this,[this]{
        labelLayout->rearrangeWidgets();
    });

    connect(ui->closesetting,&QToolButton::clicked,this,[&]{
        Close();
    });

    connect(ui->animationFunctionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),this,[&] {
        QEasingCurve::Type selectedType = static_cast<QEasingCurve::Type>(ui->animationFunctionComboBox->currentData().toInt());
        ui->stackedWidget->setCurve(selectedType);
    });

    connect(ui->appareanceButton,&QToolButton::clicked,this,&Setting::showAppareance);
    connect(ui->notifButton,&QToolButton::clicked,this,&Setting::showNotification);
    connect(ui->utilitiesButton,&QToolButton::clicked,this,&Setting::showUtilities);
    connect(ui->userInfoButton,&QToolButton::clicked,this,&Setting::showUserInfo);

    connect(ui->appareanceB,&QPushButton::clicked,ui->appareanceButton,&QToolButton::clicked);
    connect(ui->utilitiesB,&QPushButton::clicked,ui->utilitiesButton,&QToolButton::clicked);
    connect(ui->accountB,&QPushButton::clicked,ui->userInfoButton,&QToolButton::clicked);
    connect(ui->notificationB,&QPushButton::clicked,ui->notifButton,&QToolButton::clicked);

    connect(this,&Setting::resizing,this,[this]{
        if(width()<smallestWidth){
            if(ui->frame->isVisible()){
                ui->frame->hide();
                ui->gridLayout->removeWidget(ui->frame);
                ui->gridLayout->removeWidget(ui->stackedWidget);
                ui->gridLayout->addWidget(ui->stackedWidget,1,0,2,5);
                ui->gridLayout->update();

                ui->utilitiesB->show();
                ui->appareanceB->show();
                ui->accountB->show();
                ui->notificationB->show();

            }
        }else{
            if(!ui->frame->isVisible()){
                ui->gridLayout->removeWidget(ui->stackedWidget);
                ui->gridLayout->addWidget(ui->stackedWidget,1,1,2,4);
                ui->gridLayout->addWidget(ui->frame,1,0,1,1);
                ui->frame->show();
                ui->gridLayout->update();

                ui->utilitiesB->hide();
                ui->appareanceB->hide();
                ui->accountB->hide();
                ui->notificationB->hide();

            }
        }
    });
    connect(ui->editEmail,&HoverButton::clicked,ui->emailLineEdit,[this]{
        ui->emailLineEdit->setReadOnly(false);
    });
    connect(ui->editPassword,&HoverButton::clicked,ui->passwordLineEdit,[this]{
        ui->passwordLineEdit->setReadOnly(false);
    });
    connect(ui->editUsername,&HoverButton::clicked,ui->usernameLineEdit,[this]{
        ui->usernameLineEdit->setReadOnly(false);
    });
    connect(ui->seeButton,&HoverButton::clicked,this,[this](bool checked){
        if(checked){
            ui->seeButton->setIcon(QIcon(":/icons/eyewhite.svg"));
            ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->seeButton->setIcon(QIcon(":/icons/eyeclosewhite.svg"));
            ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        }
    });

    connect(ui->transparencySlider,&QSlider::valueChanged,this,[this](int value){
        transparency=value;
        m_color.setAlpha(transparency);
        ui->selectedColorFrame->setStyleSheet(QString("background:"+m_color.name(QColor::HexArgb)+";border:4px solid #479ef5;border-radius:10px;"));

        ui->colorLabel->setText(m_color.name(QColor::HexArgb));
        ui->transparencyLabel->setText(QString("%1%").arg(value*100/255));
    });

    connect(ui->selectedColorFrame,&BackgroundFrame::Clicked,this,[&]{
        QColorDialog* colorDialog= new QColorDialog(this);
        connect(colorDialog, &QColorDialog::currentColorChanged, this, [&](QColor color){
            ui->selectedColorFrame->SetSimpleType(true);
            color.setAlpha(transparency);
            m_color=color;
            ui->selectedColorFrame->setStyleSheet(QString("background:"+color.name(QColor::HexArgb)+";border:4px solid #479ef5;border-radius:10px;"));

            ui->colorLabel->setText(color.name(QColor::HexArgb));
        });
        colorDialog->exec();
        colorDialog->deleteLater();

    });
    connect(ui->disableTrashCheck,&QCheckBox::clicked,ui->trashFrame,&QFrame::setVisible);

    ui->utilitiesB->hide();
    ui->appareanceB->hide();
    ui->accountB->hide();
    ui->notificationB->hide();

    ui->selectWidget->setOrientation(Qt::Vertical);
    ui->selectWidget->raise();
    ui->appareanceButton->click();

    connect(ui->blurSlider,&QSlider::valueChanged,this,[this](int newValue){
        ui->blurLabel->setText(QString("%1%").arg(newValue*100/64));
    });
    connect(ui->enableNotifCheck,&QCheckBox::clicked,ui->notifAlertFrame,&QFrame::setVisible);

}

void Setting::addLogic()
{

    m_connections << connect(ui->fontComboBox,&QFontComboBox::currentFontChanged,this,[this](QFont font){
        m_Manager->setFontFamily(font.toString());
    });

    m_connections << connect(ui->fontSizeSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setFontSize);
    m_connections << connect(ui->save,&QToolButton::clicked,this,[&]{
        m_Manager->save();
    });
    m_connections << connect(ui->playButton,&QPushButton::clicked,this,&Setting::play);
    m_connections << connect(ui->changePicture, &QPushButton::clicked, this, [&]() {
        emit startLoading("selecting Image.... ⏳");
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Choisir une image", "", "Images (*.png *.jpg *.jpeg *.avif)");
        if (fileName.isEmpty()) {
            emit stopLoading(); // Handle case when no file is selected
            return;
        }
        QThread *workerThread=new QThread(nullptr);
        workerThread->setObjectName("ImageProcessingThread");

        ImageWorker* worker=new ImageWorker(fileName,m_Manager->userDataPath());
        worker->setWorkToDO(ImageWorker::processimage);
        worker->moveToThread(workerThread);
        connect(workerThread,&QThread::started,worker,&ImageWorker::run);
        connect(worker,&ImageWorker::pixmapReady,m_Manager,&SettingsManager::setDefaultProfilePicture);
        connect(worker,&ImageWorker::imageReady,this,&Setting::imageReady);
        connect(workerThread,&QThread::finished,this,[this,worker,workerThread]{
            worker->deleteLater();
            workerThread->deleteLater();
            emit stopLoading();
        });

        workerThread->start(QThread::HighPriority);
    });

    m_connections << connect(m_Manager,&SettingsManager::defaultProfilePictureChanged,this,&Setting::setPicture);

    m_connections << connect(ui->applyImage,&QPushButton::clicked,this,[&]{
        for(auto b : std::as_const(BackgroundFrame::m_list)){
            if(b->isSelected()){
                QString h=b->text();
                m_Manager->setBackgroundImage(h);
                break;
            }
        }
    });

    m_connections << connect(ui->applyColor, &QPushButton::clicked, this, [&]() {
        QString colorText = ui->colorLabel->text();
        // Create a worker thread and ImageWorker
        QThread *workerThread = new QThread(nullptr);
        workerThread->setObjectName("ImageProcessingThread");
        ImageWorker *worker = new ImageWorker(nullptr);
        worker->setOutputFolder(m_Manager->userDataPath());
        worker->setColor(QColor(colorText));
        worker->setWorkToDO(ImageWorker::createS);
        worker->moveToThread(workerThread);

        // When the thread starts, run the ImageWorkerconnect(workerThread, &QThread::started, worker, &ImageWorker::run);
        connect(workerThread,&QThread::started,worker,&ImageWorker::run);
        connect(worker, &ImageWorker::imageColorReady, this, [this](const QString &savedImagePath) {
            m_Manager->setCustomColor(true);
            m_Manager->setBackgroundColor(savedImagePath);
            m_Manager->setBackgroundImage(savedImagePath);
        });

        connect(workerThread, &QThread::finished, this, [worker, workerThread]() {
            delete worker;
            delete workerThread;
        });
        workerThread->start(QThread::HighPriority);
    });

    m_connections << connect(this,&Setting::backgroundChanged,this,[this](const QString& path){
        setBackgroundImage(path);
    });

    m_connections << connect(ui->addBackgroundImage,&QPushButton::clicked,this,&Setting::addBackground);



    //connecting signal .
    m_connections << connect(ui->animationDurationSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setAnimationDuration);
    m_connections << connect(ui->transparencyButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setTransparency);
    m_connections << connect(ui->changeUserDataPathCheckbox,&QCheckBox::clicked,m_Manager,&SettingsManager::setChangeUserDataPath);
    m_connections << connect(ui->deleteAfterUploadButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setDeleteAfterUpload);
    m_connections << connect(ui->synchronizedSettingButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setSynchronize);
    m_connections << connect(ui->startLazyDownloadCheck,&QCheckBox::clicked,m_Manager,&SettingsManager::setStartDownloadStartup);
    m_connections << connect(ui->disableTrashCheck,&QCheckBox::clicked,m_Manager,&SettingsManager::setEnableTrash);
    m_connections << connect(ui->launchOnStartupButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setLaunchOnStartUp);
    m_connections << connect(ui->automaticallyDeleteThan5,&QCheckBox::clicked,m_Manager,&SettingsManager::setDeleteGreaterThan5);
    m_connections << connect(ui->disableAnimationButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setAnimationEnabled);
    m_connections << connect(ui->useSystemNotifButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setUseSystemNotificationPage);
    m_connections << connect(ui->popoutNotifPageButton,&ToggleButton::toggled,m_Manager,&SettingsManager::setPoppoutNotificationPage);
    m_connections << connect(ui->enableNotifCheck,&QCheckBox::clicked,m_Manager,&SettingsManager::setEnableNotification);

    m_connections << connect(ui->customBackgroundImageRadio,&QRadioButton::clicked,this,[&]{
        m_Manager->setCustomColor(false);
        m_Manager->setCustomImage(true);
        ui->backgroundImageFrame->show();
        ui->backgroundColorFrame->hide();
    });
    m_connections << connect(ui->customBackgroundColorRadio,&QRadioButton::clicked,this,[&]{
        m_Manager->setCustomColor(true);
        m_Manager->setCustomImage(false);
        ui->backgroundColorFrame->show();
        ui->backgroundImageFrame->hide();
    });
    m_connections << connect(ui->volumeSlider,&QSlider::valueChanged,this,[this](int value){
        m_Manager->setNotificationVolume(value/100.0);
        ui->volumeLabel->setText(QString("%1%").arg(value));
    });
    m_connections << connect(ui->durationSlider,&QSlider::valueChanged,this,[this](int value){
        m_Manager->setNotificationDuration(value*1000);
        ui->durationLabel->setText(QString("%1s").arg(value));
    });


    m_connections << connect(ui->resetAllButton,&QPushButton::clicked,this,[this]{
        m_Manager->reset();
        loadSettingToUi();
    });
    m_connections << connect(ui->uploadSpeedLimitSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setSpeedUploadLimit);
    m_connections << connect(ui->downloadSpeedLimitSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setSpeedDownloadLimit);
    m_connections << connect(ui->uploadLimitSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setUploadLimit);
    m_connections << connect(ui->downloadLimitSpinBox,&QSpinBox::valueChanged,m_Manager,&SettingsManager::setDownloadLimit);


}


void Setting::breakLogic() {
    // Disconnect all stored connections
    for (const auto &connection : std::as_const(m_connections)) {
        QObject::disconnect(connection);
    }
    m_connections.clear();
}


void Setting::Close()
{
    if (!m_Manager->edited()) {
        emit aboutToClose();
    } else {
        QMessageBox msgBox(nullptr);
        msgBox.setPalette(this->palette());
        msgBox.setText("The settings has been modified !");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            if(m_Manager->save()){
                emit aboutToClose();
            }else{
                qDebug() << "an error occured ";
            }
            break;
        case QMessageBox::Discard:
            m_Manager->setEdited(false);
            emit aboutToClose();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
        }
    }
}

void Setting::build()
{
    spacing=buttonH+ui->gridLayout_4->verticalSpacing();


    loadSettingToUi();
    setupDesign();


    addLogic();
    setEnableBackground(false);
    setEnableBackground(false);

}


///customToolButton
CustomToolButton::CustomToolButton(QWidget *parent) : QToolButton(parent), m_hovered(false) {
    setAttribute(Qt::WA_Hover);

    connect(this,&CustomToolButton::clicked,this,[&]{
        if(toolList.isEmpty()) return;
        setSelected(true);
        for(auto &b : toolList){
            if(b==this){
                continue;
            }
            b->setSelected(false);
        }
    });
    toolList.append(this);
}

void CustomToolButton::setSelected(bool slected)
{
    this->selected=slected;
    if(selected){
        setStyleSheet(R"(QToolButton{
border-radius:6px;
background:rgba(30,30,30,0.6);
color:rgba(71, 158, 245, 1);
}
)");
    }else{
        setStyleSheet(R"(QToolButton{
background:transparent;
}
QToolButton:hover{
color:rgba(71, 158, 245, 1);
border-radius:6px;
background:rgba(30,30,30,0.6);
})");
    }
}

bool CustomToolButton::isSelected()
{
    return selected;
}

void CustomToolButton::paintEvent(QPaintEvent *event) {
    QToolButton::paintEvent(event);
}

void CustomToolButton::enterEvent(QEnterEvent *event) {
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void CustomToolButton::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    m_hovered = false;
    update();
}
