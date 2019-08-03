#include "SlicerGUI.h"
#include "ui_SimpleGUI.h"

#include "Converter.h"
#include "Histology.h"
#include "DirIO.h"
#include "OpencvViewer.h"
#include <QTextStream>
#include <QFileDialog>
#include <opencv2/imgcodecs/imgcodecs.hpp>

SlicerGUI::SlicerGUI() : viewer_(nullptr), dataset_histology_(nullptr)
{
    this->ui = new Ui_SimpleGUI;
    ui->setupUi(this);

    ui->actionLoad_Directory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(ui->class1ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onChange_ComboBox1(QString)));
    connect(ui->class2ComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onChange_ComboBox2(QString)));
    connect(ui->class1List, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_ClassList1()));
    connect(ui->class2List, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_ClassList2()));
    connect(ui->slice1NameEdit, SIGNAL(editingFinished()), this, SLOT(onEditSliceName()));
    connect(ui->slice2NameEdit, SIGNAL(editingFinished()), this, SLOT(onEditSliceName()));
    connect(ui->pairsList, SIGNAL(itemSelectionChanged()), this, SLOT(onChange_PairsList()));
}

SlicerGUI::~SlicerGUI()
{
    qApp->exit();
}

void SlicerGUI::on_actionExit_triggered()
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
    if (ui->pairsList->currentItem()) 
    {
        const auto name = ui->pairsList->currentItem()->text();
        auto& slicePair = slicePairNames_[name];
        ui->slice1View->setPixmap(QPixmap::fromImage(Mat2QImage(slices_[slicePair.name1].slice)));
        ui->histogram1->setPixmap(QPixmap::fromImage(Mat2QImage(slices_[slicePair.name1].histogram)));
        delete viewedSlice1_;
        viewedSlice1_ = new Slice(slices_[slicePair.name1]);

        ui->slice2View->setPixmap(QPixmap::fromImage(Mat2QImage(slices_[slicePair.name2].slice)));
        ui->histogram2->setPixmap(QPixmap::fromImage(Mat2QImage(slices_[slicePair.name2].histogram)));
        delete viewedSlice2_;
        viewedSlice2_ = new Slice(slices_[slicePair.name1]);
    }
}

void SlicerGUI::onEditSliceName()
{
    nameEdited_ = true;
}

void SlicerGUI::on_keepBtn_clicked()
{
    if (flagKept_) return;
    ui->class1List->currentItem()->setBackground(QBrush(QColor(253, 241, 38)));
    ui->class2List->currentItem()->setBackground(QBrush(QColor(253, 241, 38)));

    auto name1 = ui->slice1NameEdit->text() + "_0";
    auto name2 = ui->slice2NameEdit->text() + "_1";

    if (slices_.find(name1) == slices_.end()) {
        slices_[name1] = *viewedSlice1_;
    }
    if (slices_.find(name2) == slices_.end()) {
        slices_[name2] = *viewedSlice2_;
    }
    auto slicepair = SlicePairNames(std::move(name1), std::move(name2));
    ui->pairsList->addItem(slicepair.getCSVLine());
    slicePairNames_[slicepair.getCSVLine()] = slicepair;
    //ui->pairsList->sortItems();

    ui->pairsCount->setValue(ui->pairsList->count());

    flagSlice1Changed_ = false;
    flagSlice2Changed_ = false;
    flagKept_ = true;
}

void SlicerGUI::on_removeBtn_clicked()
{
    auto currentRow = ui->pairsList->currentRow();
    auto item = ui->pairsList->takeItem(currentRow);
    auto it = slicePairNames_.find(item->text());
    if (it != slicePairNames_.end())
    {
        slicePairNames_.erase(it);
    }
    ui->pairsCount->setValue(ui->pairsList->count());
    delete item;
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
    if (!nameEdited_ || ui->slice1NameEdit->text().isEmpty() || ui->slice2NameEdit->text().isEmpty())
    {
        ui->slice1NameEdit->setText(file1_info + "_" + QString::number(ui->id1->value()));
        ui->slice2NameEdit->setText(file2_info + "_" + QString::number(ui->id2->value()));
        nameEdited_ = false;
    }
}

void SlicerGUI::recieveSlice(cv::Mat img, cv::Mat histogram, ClickSlice slice)
{
    //cv::Mat img8 = img / 65535.f * 255.f;
    if (slice.imageWindow == WINDOW_1) 
    {
        ui->slice1View->setPixmap(QPixmap::fromImage(Mat2QImage(img)));;
        ui->histogram1->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));
        delete viewedSlice1_;
        viewedSlice1_ = new Slice(slice, img, histogram);
        viewedSlice1_->type = ui->class1ComboBox->currentText();
        if (flagKept_ && !flagSlice1Changed_ || !flagSlice1Changed_ && flagSlice2Changed_) ui->id1->setValue(lastID1 + 1);
        flagSlice1Changed_ = true;
        
    }
    else if (slice.imageWindow == WINDOW_2)
    {
        ui->slice2View->setPixmap(QPixmap::fromImage(Mat2QImage(img)));
        ui->histogram2->setPixmap(QPixmap::fromImage(Mat2QImage(histogram)));
        delete viewedSlice2_;
        viewedSlice2_ = new Slice(slice, img, histogram);
        viewedSlice2_->type = ui->class2ComboBox->currentText();
        if (flagKept_ && !flagSlice2Changed_ || flagSlice1Changed_ && !flagSlice2Changed_) ui->id2->setValue(lastID2 + 1);
        flagSlice2Changed_ = true;
    }
    updateNameEdit();
    lastID1 = ui->id1->value();
    lastID2 = ui->id2->value();
    flagKept_ = false;
}

void SlicerGUI::on_actionLoad_Directory_triggered()
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

void SlicerGUI::on_saveAll_clicked()
{
    if (ui->confirmSave->isChecked())
    {
        auto save_to = QFileDialog::getExistingDirectory(nullptr, tr("Save to - root"), dataset_histology_->lastLocation(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (save_to.isEmpty())
        {
            return;
        }

        auto dir = QDir(save_to);
    
        dir.mkpath(save_to + tr("/similar"));
        dir.mkpath(save_to + tr("/dissimilar"));
        QDir similar(save_to + tr("/similar"));
        QDir dissimilar(save_to + tr("/similar"));

        QFile csvFile(dir.path() + tr("/pairs.csv"));
        csvFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream stream(&csvFile);
        for (auto& slice : slicePairNames_)
        {
            QString& name1 = slice.second.name1;
            QString& name2 = slice.second.name2;

            stream << slice.second.getCSVLineTif() << endl;

            if (slices_[name1].type == slices_[slice.second.name2].type)
            {
                if (!similar.exists(name1)) cv::imwrite((similar.absolutePath() + tr("/") + name1).toStdString() + ".tif", slices_[name1].slice);
                if (!similar.exists(name2)) cv::imwrite((similar.absolutePath() + tr("/") + name2).toStdString() + ".tif", slices_[name2].slice);
            } else
            {
                if (!dissimilar.exists(name1)) cv::imwrite((dissimilar.absolutePath() + tr("/") + name1).toStdString() + ".tif", slices_[name1].slice);
                if (!dissimilar.exists(name2)) cv::imwrite((dissimilar.absolutePath() + tr("/") + name2).toStdString() + ".tif", slices_[name2].slice);
            }
        }
        ui->pairsList->clear();
        slicePairNames_.clear();
        slices_.clear();
        delete viewedSlice1_;
        viewedSlice1_ = nullptr;
        delete viewedSlice2_;
        viewedSlice2_ = nullptr;
        ui->pairsCount->setValue(ui->pairsList->count());
    }
}