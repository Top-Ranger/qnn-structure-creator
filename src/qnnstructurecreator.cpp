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

#include "qnnstructurecreator.h"
#include "ui_qnnstructurecreator.h"

#include <network/genericgene.h>
#include <network/lengthchanginggene.h>

#include <network/feedforwardnetwork.h>
#include <network/continuoustimerecurrenneuralnetwork.h>
#include <network/gasnet.h>
#include <network/modulatedspikingneuronsnetwork.h>

#include <simulation/genericsimulation.h>
#include <simulation/tmazesimulation.h>
#include <simulation/rebergrammarsimulation.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QStringListModel>
#include <QStringList>
#include <QFile>
#include <QDir>

QnnStructureCreator::QnnStructureCreator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QnnStructureCreator)
{
    ui->setupUi(this);
    enableButtons();

    QStringListModel *nn_model = new QStringListModel(this);
    ui->listView->setModel(nn_model);
    QStringListModel *sim_model = new QStringListModel(this);
    ui->listViewSimulation->setModel(sim_model);

    QStringList nn;
    nn << "FeedForwardNeuralNetwork";
    nn << "FeedForwardNeuralNetwork (tanh)";
    nn << "ContinuousTimeRecurrenNeuralNetwork";
    nn << "ContinuousTimeRecurrenNeuralNetwork (tanh)";
    nn << "ContinuousTimeRecurrenNeuralNetwork (size changing)";
    nn << "ContinuousTimeRecurrenNeuralNetwork (size changing, tanh)";
    nn << "GasNet";
    nn << "ModulatedSpikingNeuronsNetwork (a)";
    nn << "ModulatedSpikingNeuronsNetwork (b)";
    nn << "ModulatedSpikingNeuronsNetwork (c)";
    nn << "ModulatedSpikingNeuronsNetwork (d)";
    nn << "ModulatedSpikingNeuronsNetwork (full)";
    nn << "ModulatedSpikingNeuronsNetwork (none)";

    nn_model->setStringList(nn);

    QStringList sim;
    sim << "GenericSimulation";
    sim << "TMazeSimulation";
    sim << "ReberGrammarSimulation (DetectGrammar)";
    sim << "ReberGrammarSimulation (CreateWords)";
    sim << "ReberGrammarSimulation (embedded, DetectGrammar)";
    sim << "ReberGrammarSimulation (embedded, CreateWords)";

    sim_model->setStringList(sim);
}

QnnStructureCreator::~QnnStructureCreator()
{
    delete ui;
}

void QnnStructureCreator::enableButtons()
{
    ui->fileButton->setEnabled(ui->file->text() != "");
    ui->folderButton->setEnabled(ui->folder->text() != "");
}

void QnnStructureCreator::showError(QString filename)
{
    QMessageBox::critical(this,
                          tr("Error"),
                          QString(tr("Can not convert file '%1'")).arg(filename));
}

void QnnStructureCreator::convertOneGene(QString filename)
{
    GenericGene *gene = NULL;

    // load gene
    QFile gene_file(filename);
    if(GenericGene::canLoadThisGene(&gene_file))
    {
        gene = GenericGene::loadThisGene(&gene_file);
    }
    else if(LengthChangingGene::canLoadThisGene(&gene_file))
    {
        gene = LengthChangingGene::loadThisGene(&gene_file);
    }
    else
    {
        QMessageBox::critical(this,
                              tr("Broken gene file"),
                              tr("Can not load any gene from file."));
        return;
    }


    // save network
    AbstractNeuralNetwork *network = getNetwork();

    if(network != NULL)
    {
        network->initialise(gene);
        filename.replace(".gene", ".xml");
        QFile file(filename);
        network->saveNetworkConfig(&file);
    }

    // cleanup
    delete network;
    delete gene;
}

AbstractNeuralNetwork *QnnStructureCreator::getNetwork()
{
    AbstractNeuralNetwork *network = NULL;
    GenericSimulation *simulation = NULL;

    QString selection = ui->listViewSimulation->currentIndex().data().toString();
    if(selection == "")
    {
        QMessageBox::information(this,
                                 tr("Invalid selection"),
                                 tr("Please select a simulation"));
        return network;
    }

    if(selection == "GenericSimulation")
    {
        simulation = new GenericSimulation();
    }
    else if(selection == "TMazeSimulation")
    {
        TMazeSimulation::config config;
        config.trials = 1;
        simulation = new TMazeSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (DetectGrammar)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (CreateWords)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.mode = ReberGrammarSimulation::CreateWords;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (embedded, DetectGrammar)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.embedded = true;
        simulation = new ReberGrammarSimulation(config);
    }
    else if(selection == "ReberGrammarSimulation (embedded, CreateWords)")
    {
        ReberGrammarSimulation::config config;
        config.trials_create = 1;
        config.trials_detect = 1;
        config.mode = ReberGrammarSimulation::CreateWords;
        config.embedded = true;
        simulation = new ReberGrammarSimulation(config);
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Unknown selection"),
                             QString(tr("Unknown selection: &1")).arg(selection));
        return network;
    }

    selection = ui->listView->currentIndex().data().toString();

    if(selection == "")
    {
        QMessageBox::information(this,
                                 tr("Invalid selection"),
                                 tr("Please select a network"));
        return network;
    }

    if(selection == "FeedForwardNeuralNetwork")
    {
        network = new FeedForwardNetwork(simulation->needInputLength(), simulation->needOutputLength());
    }
    else if(selection == "FeedForwardNeuralNetwork (tanh)")
    {
        FeedForwardNetwork::config config;
        config.activision_function = &tanh;
        network = new FeedForwardNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (tanh)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.activision_function = &tanh;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (size changing)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.size_changing = true;
        config.network_default_size_grow = 1;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ContinuousTimeRecurrenNeuralNetwork (size changing, tanh)")
    {
        ContinuousTimeRecurrenNeuralNetwork::config config;
        config.size_changing = true;
        config.network_default_size_grow = 1;
        config.activision_function = &tanh;
        network = new ContinuousTimeRecurrenNeuralNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "GasNet")
    {
        GasNet::config config;
        network = new GasNet(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (a)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = true;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = false;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (b)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = true;
        config.c_modulated = false;
        config.d_modulated = false;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (c)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = true;
        config.d_modulated = false;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (d)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = true;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (full)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = true;
        config.b_modulated = true;
        config.c_modulated = true;
        config.d_modulated = true;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork (none)")
    {
        ModulatedSpikingNeuronsNetwork::config config;
        config.a_modulated = false;
        config.b_modulated = false;
        config.c_modulated = false;
        config.d_modulated = false;
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength(), config);
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Unknown selection"),
                             QString(tr("Unknown selection: &1")).arg(selection));
    }

    delete simulation;
    return network;
}

void QnnStructureCreator::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("About"),
                       tr("QnnStructureCreator is a simple tool to create network structures out of gene files.\nAuthor: Marcus Soll\nLicense: GPL3+\nThis program uses qnn, which is licensed under the LGPL3+"));
}

void QnnStructureCreator::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void QnnStructureCreator::on_actionQuit_triggered()
{
    QApplication::quit();
}

void QnnStructureCreator::on_fileSelectButton_clicked()
{
    ui->file->setText(QFileDialog::getOpenFileName(this, tr("Select gene file")));
    enableButtons();
}

void QnnStructureCreator::on_folderSelectButton_clicked()
{
    ui->folder->setText(QFileDialog::getExistingDirectory(this, "Select folder", ""));
    enableButtons();
}

void QnnStructureCreator::on_fileButton_clicked()
{
    QString filename = ui->file->text();
    if(!QFile::exists(filename))
    {
        QMessageBox::information(this,
                                 tr("File not existing"),
                                 QString(tr("Gene file '%1' does not exists.")).arg(filename));
        return;
    }

    convertOneGene(filename);

    QMessageBox::information(this,
                             tr("Finished"),
                             tr("Conversion finished"));
}

void QnnStructureCreator::on_folderButton_clicked()
{
    QString folder_path = ui->folder->text();

    QStringList filter;
    filter << "*.gene";

    QDir dir(folder_path);
    QStringList files = dir.entryList(filter);
    foreach (QString file_name, files)
    {
        convertOneGene(QString("%1/%2").arg(folder_path).arg(file_name));
    }

    QMessageBox::information(this,
                             tr("Finished"),
                             tr("Conversion finished"));
}
