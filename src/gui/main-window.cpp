#include <QtGui>
#include "main-window.h"

// if we include <QtGui> there is no need to include every class used: <QString>, <QFileDialog>,...

MainWindow::MainWindow(QMainWindow *parent, Qt::WFlags f) : QMainWindow(parent, f)
{
    setupUi(this); // this sets up GUI
	setWindowFlags(f & (~Qt::WindowMaximizeButtonHint));

	/* Cria tela de opcoes */
	opcoes = new Opcoes(parent, f);
	opcoes->setMundo(widget);
	opcoes->setBarras(horizontalScrollBar, verticalScrollBar);

	/* Cria tela de ajuda */
	ajuda = new Ajuda(parent, f);

	/* Cria tela de informacoes */
	info = new Info(parent, f);
	widget->setInfo(info);

	/* Cria tela de estatisticas */
	stat = new Stat(parent, f);
	widget->setStat(stat);

	/* Cria toolbares */
	QToolBar *toolBar1 = new QToolBar(tr("Tool bar"), this);
	toolBar1->addAction(abrirSimulacaoAct);
	toolBar1->addAction(salvarSimulacaoAct);
	toolBar1->addSeparator();
	toolBar1->addAction(limparAct);
	toolBar1->addAction(reiniciarAct);
	toolBar1->addAction(pauseAct);
	toolBar1->addSeparator();
	toolBar1->addAction(estatisticasAct);
	toolBar1->addAction(opcoesAct);
	toolBar1->addSeparator();
	toolBar1->addSeparator();
	toolBar1->addAction(ajudaAct);
	toolBar1->addAction(sairAct);

	QToolBar *toolBar2 = new QToolBar(tr("Search Biot"), this);
	toolBar2->addAction(maisVelhoAct);
	toolBar2->addAction(maisEnergiaAct);
	toolBar2->addAction(geracaoMaisNovaAct);
	toolBar2->addAction(geracaoMaisAntigaAct);
	toolBar2->addAction(maisFilhosAct);
	toolBar2->addSeparator();
	toolBar2->addAction(fixarSelecionadoAct);
	toolBar2->addAction(autoSelecionarAct);

	/* Inclui toolbares */
	addToolBar(Qt::TopToolBarArea, toolBar1);
	addToolBar(Qt::LeftToolBarArea, toolBar2);

	/* Cria status bar */
	QStatusBar *barraStatus = new QStatusBar(this);
	setStatusBar(barraStatus);

	/* Associa movimento das barras de rolagem */
    QObject::connect(horizontalScrollBar, SIGNAL(valueChanged(int)), widget, SLOT(offsetxChanged(int)));
    QObject::connect(verticalScrollBar, SIGNAL(valueChanged(int)), widget, SLOT(offsetyChanged(int)));
    QObject::connect(widget, SIGNAL(changeVerticalBar(int)), verticalScrollBar, SLOT(setValue(int)));
    QObject::connect(widget, SIGNAL(changeHorizontalBar(int)), horizontalScrollBar, SLOT(setValue(int)));

	/* Associa acoes */
    QObject::connect(limparAct, SIGNAL(triggered()), widget, SLOT(limpar()));
    QObject::connect(reiniciarAct, SIGNAL(triggered()), widget, SLOT(reiniciar()));
	QObject::connect(pauseAct, SIGNAL(triggered()), this, SLOT(startStop()));
	QObject::connect(sairAct, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ajudaAct, SIGNAL(triggered()), ajuda, SLOT(show()));
	QObject::connect(opcoesAct, SIGNAL(triggered()), opcoes, SLOT(show()));
	QObject::connect(estatisticasAct, SIGNAL(triggered()), stat, SLOT(show()));
	QObject::connect(salvarSimulacaoAct, SIGNAL(triggered()), widget, SLOT(salvarSimulacao()));
	QObject::connect(abrirSimulacaoAct, SIGNAL(triggered()), widget, SLOT(abrirSimulacao()));
	QObject::connect(widget, SIGNAL(estadoModificado(QString)), barraStatus, SLOT(showMessage(QString)));

	QObject::connect(maisVelhoAct, SIGNAL(triggered()), widget, SLOT(maisVelho()));
	QObject::connect(maisEnergiaAct, SIGNAL(triggered()), widget, SLOT(maisEnergia()));
	QObject::connect(geracaoMaisNovaAct, SIGNAL(triggered()), widget, SLOT(geracaoMaisNova()));
	QObject::connect(geracaoMaisAntigaAct, SIGNAL(triggered()), widget, SLOT(geracaoMaisAntiga()));
	QObject::connect(maisFilhosAct, SIGNAL(triggered()), widget, SLOT(maisFilhos()));
	QObject::connect(fixarSelecionadoAct, SIGNAL(triggered()), widget, SLOT(fixarSelecionado()));
	QObject::connect(autoSelecionarAct, SIGNAL(triggered()), widget, SLOT(autoSelecionar()));

	/* Cria menu popup de biota */
	QMenu *menuBiota = new QMenu(tr("Biot"), widget);
	menuBiota->addAction(salvarBiotaAct);
	menuBiota->addAction(mutacaoAct);
	menuBiota->addAction(matarBiotaAct);
	menuBiota->addAction(informacoesAct);

	/* Associa conexoes */
	QObject::connect(mutacaoAct, SIGNAL(triggered()), widget, SLOT(mutacaoBiota()));
	QObject::connect(salvarBiotaAct, SIGNAL(triggered()), widget, SLOT(salvarBiota()));
	QObject::connect(matarBiotaAct, SIGNAL(triggered()), widget, SLOT(removerBiota()));
	QObject::connect(informacoesAct, SIGNAL(triggered()), info, SLOT(show()));

	/* Cria menu popup vazio */
	QMenu *menuVoid = new QMenu(tr("Options"), widget);
	menuVoid->addAction(novaParedeAct);
	menuVoid->addSeparator();
	menuVoid->addAction(novoBiotaAct);
	menuVoid->addAction(carregarBiotaAct);
	menuVoid->addSeparator();
	menuVoid->addAction(novoGraoAct);

	/* Associa conexoes */
	QObject::connect(novaParedeAct, SIGNAL(triggered()), widget, SLOT(novaParede()));
	QObject::connect(novoBiotaAct, SIGNAL(triggered()), widget, SLOT(novoBiota()));
	QObject::connect(carregarBiotaAct, SIGNAL(triggered()), widget, SLOT(abrirBiota()));
	QObject::connect(novoGraoAct, SIGNAL(triggered()), widget, SLOT(novoGrao()));

	/* Cria menu popup do grao */
	QMenu *menuGrao = new QMenu(tr("Grain"), widget);
	menuGrao->addAction(removerGraoAct);

	/* Associa conexao */
	QObject::connect(removerGraoAct, SIGNAL(triggered()), widget, SLOT(removerGrao()));

	/* Cria menu popup da parede */
	QMenu *menuParede  = new QMenu(tr("Wall"), widget);
	menuParede->addAction(removerParedeAct);

	/* Associa conexao */
	QObject::connect(removerParedeAct, SIGNAL(triggered()), widget, SLOT(removerParede()));

	/* Associa menus ao widget */
	widget->setMenuBiota(menuBiota);
	widget->setMenuGrao(menuGrao);
	widget->setMenuParede(menuParede);
	widget->setMenuVoid(menuVoid);

	/* Muda tamanho das barras */
	horizontalScrollBar->setRange(0, widget->propriedades.tamanho_x);
	verticalScrollBar->setRange(0, widget->propriedades.tamanho_y);

	horizontalScrollBar->setSliderPosition(widget->propriedades.tamanho_x/2);
	verticalScrollBar->setSliderPosition(widget->propriedades.tamanho_y/2);

	/* Seta mutex */
	widget->setMutex(new QMutex(QMutex::Recursive));

	/* Seta timers para atualizar, pintar e atualizar estatistica */
	timerA = new QTimer(this);
	QObject::connect(timerA, SIGNAL(timeout()), widget, SLOT(atualizar()));

	timerP = new QTimer(this);
	QObject::connect(timerP, SIGNAL(timeout()), widget, SLOT(repaint()));

	timerS = new QTimer(this);
	QObject::connect(timerS, SIGNAL(timeout()), widget, SLOT(estatistica()));

	/* Inicia timers */
	timerA->start(50);
	timerP->start(50);
	timerS->start(500);
}

void MainWindow::startStop()
{
	if (timerP->isActive())
		timerP->stop();
	else
		timerP->start();

	if (timerA->isActive())
		timerA->stop();
	else
		timerA->start();

	if (timerS->isActive())
		timerS->stop();
	else
		timerS->start();

	widget->executando = !widget->executando;
}
