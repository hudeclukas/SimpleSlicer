#include "SlicerGUI.h"
#include "ui_SimpleGUI.h"

#include "Converter.h"
#include "Histology.h"
#include "DirIO.h"
#include "OpencvViewer.h"
#include <QFileDialog>

SlicerGUI::SlicerGUI() : viewer_(nullptr), dataset_histology_(nullptr)
{
    this->ui = new Ui_SimpleGUI;
    ui->setupUi(this);

    ui->actionLoad_Directory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit()));
    connect(ui->actionLoad_Directory, SIGNAL(triggered()), this, SLOT(on_actionLoad_Directory()));
    connect(ui->actionSave_To, SIGNAL(triggered()), this, SLOT(on_actionSave_To()));
    connect(ui->class1ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onChange_ComboBox1(QString)));
    connect(ui->class2ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onChange_ComboBox2(QString)));
    connect(ui->class1List, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_ClassList1()));
    connect(ui->class2List, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_ClassList2()));
    connect(ui->keepBtn, SIGNAL(pressed()), this, SLOT(onKeepPressed()));
    connect(ui->sliceNameEdit, SIGNAL(editingFinished()), this, SLOT(onEditSliceName()));
    connect(ui->pairsList, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_PairsList()));
}

SlicerGUI::~SlicerGUI()
{
    qApp->exit();
}

void SlicerGUI::on_actionExit()
{
    qApp->exit();
}

void SlicerGUI::onChange_ComboBox1(const QString& text)
{
    ui->class1List->clear();
    ui->class1List->addItems(dataset_histology_->getCancerTypeImagesList(text));
}

void SlicerGUI::onChange_ComboBox2(const QString& text)
{
    ui->class2List->clear();
    ui->class2List->addItems(dataset_histology_->getCancerTypeImagesList(text));
}

void SlicerGUI::onChange_ClassList1()
{
    QString path = ui->class1List->currentItem()->text();
    viewer_->showImageFromFileToView1(path.toStdString(), dataset_histology_->readImage(path));
    if (ui->class2List->currentItem()) {
        QFileInfo file1_info(path);
        QFileInfo file2_info(ui->class2List->currentItem()->text());
        updateNameEdit(file1_info.baseName(), file2_info.baseName());
    }
}

void SlicerGUI::onChange_ClassList2()
{
    QString path = ui->class2List->currentItem()->text();
    viewer_->showImageFromFileToView2(path.toStdString(), dataset_histology_->readImage(path));
    if (ui->class1List->currentItem()) {
        QFileInfo file2_info(path);
        QFileInfo file1_info(ui->class1List->currentItem()->text());
        updateNameEdit(file1_info.baseName(), file2_info.baseName());
    }
}

void SlicerGUI::onChange_PairsList()
{
    const auto name = ui->pairsList->currentItem()->text();
    auto& slicePair = slices_[name];
    ui->slice1View->setPixmap(QPixmap::fromImage(Mat2QImage(slicePair.one->slice)));
    ui->histogram1->setPixmap(QPixmap::fromImage(Mat2QImage(slicePair.one->histogram)));
    delete viewedSlice1_;
    viewedSlice1_ = new Slice(*slicePair.one);

    ui->slice2View->setPixmap(QPixmap::fromImage(Mat2QImage(slicePair.two->slice)));
    ui->histogram2->setPixmap(QPixmap::fromImage(Mat2QImage(slicePair.two->histogram)));
    delete viewedSlice2_;
    viewedSlice2_ = new Slice(*slicePair.two);
}

void SlicerGUI::onEditSliceName()
{
    nameEdited_ = true;
}

void SlicerGUI::onKeepPressed()
{
    if (flagKept_) return;
    ui->class1List->currentItem()->setBackground(QBrush(QColor(253, 241, 38)));
    ui->class2List->currentItem()->setBackground(QBrush(QColor(253, 241, 38)));

    const auto name = ui->sliceNameEdit->text();
    slices_[name] = SlicePair(*viewedSlice1_, *viewedSlice2_, ui->id1->value(), ui->id2->value());

    ui->pairsList->addItem(name);
    //ui->pairsList->sortItems();

    flagSlice1Changed_ = false;
    flagSlice2Changed_ = false;
    flagKept_ = true;
}

void SlicerGUI::updateNameEdit(QString file1_info, QString file2_info)
{
    if (file1_info.isEmpty() && file2_info.isEmpty())
    {
        if (ui->class1List->currentItem() && ui->class2List->currentItem()) {
            file1_info = QFileInfo(ui->class1List->currentItem()->text()).baseName();
            file2_info = QFileInfo(ui->class2List->currentItem()->text()).baseName();
        }
    }
    if (!nameEdited_ || ui->sliceNameEdit->text().isEmpty())
    {
        ui->sliceNameEdit->setText(file1_info + "_" + QString::number(ui->id1->value()) + "_" + file2_info + "_" + QString::number(ui->id2->value()));
    }
}

void SlicerGUI::recieveSlice(cv::Mat img, cv::Mat histogram, ClickSlice slice)
{
    if (slice.imageWindow == WINDOW_1) 
    {
        ui->slice1View->setPixmap(QPixmap::fromImage(Mat2QImage(img)));
        ui->histogram1->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));
        delete viewedSlice1_;
        viewedSlice1_ = new Slice(slice, img, histogram);
        if (flagKept_ && !flagSlice1Changed_) ui->id1->setValue(lastID1 + 1);
        flagSlice1Changed_ = true;
    }
    else if (slice.imageWindow == WINDOW_2)
    {
        ui->slice2View->setPixmap(QPixmap::fromImage(Mat2QImage(img)));
        ui->histogram2->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));
        delete viewedSlice2_;
        viewedSlice2_ = new Slice(slice, img, histogram);
        if (flagKept_ && !flagSlice2Changed_) ui->id2->setValue(lastID2 + 1);
        flagSlice2Changed_ = true;
    }
    updateNameEdit();
    lastID1 = ui->id1->value();
    lastID2 = ui->id2->value();
    flagKept_ = false;
}

void SlicerGUI::on_actionLoad_Directory()
{
    if (dataset_histology_ == nullptr)
    {
        dataset_histology_ = new histology::Dataset;
    }
    else
    {
        dataset_histology_->loadDataset();
        ui->class1ComboBox->clear();
        ui->class2ComboBox->clear();
    }
    ui->class1ComboBox->addItems(dataset_histology_->getCancerTypes());
    ui->class2ComboBox->addItems(dataset_histology_->getCancerTypes());

    viewer_ = new CV_Viewer();
    viewer_->setSize(ui->sliceSize->value());
    connect(ui->sliceSize, SIGNAL(valueChanged(int)), viewer_, SLOT(setSize(int)));
    connect(viewer_, SIGNAL(sliceSelected(cv::Mat, cv::Mat, ClickSlice)), this, SLOT(recieveSlice(cv::Mat, cv::Mat, ClickSlice)));
}

void SlicerGUI::on_actionSave_To()
{
    if (ui->confirmSave->isChecked())
    {
        
    }
}