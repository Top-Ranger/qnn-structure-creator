/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn-ui.
 *
 * qnn-structure-creator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn-structure-creator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qnn-structure-creator. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QNNSTRUCTURECREATOR_H
#define QNNSTRUCTURECREATOR_H

#include <network/abstractneuralnetwork.h>

#include <QMainWindow>

namespace Ui {
class QnnStructureCreator;
}

class QnnStructureCreator : public QMainWindow
{
    Q_OBJECT

public:
    explicit QnnStructureCreator(QWidget *parent = 0);
    ~QnnStructureCreator();

private slots:
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionQuit_triggered();
    void on_fileSelectButton_clicked();
    void on_folderSelectButton_clicked();

    void on_fileButton_clicked();

    void on_folderButton_clicked();

private:
    void enableButtons();
    void showError(QString filename);
    void convertOneGene(QString filename);
    AbstractNeuralNetwork *getNetwork();

    Ui::QnnStructureCreator *ui;
};

#endif // QNNSTRUCTURECREATOR_H
