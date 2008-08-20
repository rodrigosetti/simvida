#ifndef _MUNDO_QT_H
#define _MUNDO_QT_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QMutex>
#include <QMenu>
#include "stat.h"
#include "info.h"
#include "../core/mundo.h"

/* Constates para valores padroes */
#define TAMANHO_X 8192
#define TAMANHO_Y 6144

#define ENERGIA_GRAO 200
#define TETO_ENERGETICO ((TAMANHO_X*TAMANHO_Y)/900)

#define PROBABILIDADE_MUTACAO 60
#define INTENSIDADE_MUTACAO 1

class MundoQT : public QWidget, public Mundo
{
    Q_OBJECT

public:

	/* Constructor */
     MundoQT(QWidget *parent = 0, Qt::WFlags f = 0);
     ~MundoQT();

	/* Controle de execucao */
	QMutex* getMutex();
	void setMutex(QMutex *m);
	bool executando;

	/* Seta menus popup */
	void setMenuBiota(QMenu *m);
	void setMenuGrao(QMenu *m);
	void setMenuVoid(QMenu *m);
	void setMenuParede(QMenu *m);

	/* Seta telas */
	inline void setStat(Stat *s) { stat = s; }
	inline void setInfo(Info *i) { info = i; }

	/* getters */
	inline Biota getSelecionado()
	{
		return selecionado->biota;
	}

signals:

	void changeHorizontalBar(int);
	void changeVerticalBar(int);
	void estadoModificado(QString texto);

public slots:

	void offsetxChanged(int);
	void offsetyChanged(int);

	void atualizar();
	void estatistica();

	void limpar();
	void reiniciar();

	void novaParede();
	void removerParede();
	void novoGrao();
	void removerGrao();
	void novoBiota();
	void salvarBiota();
	void abrirBiota();
	void removerBiota();
	void mutacaoBiota();

	inline void fixarSelecionado() {fixar = !fixar;}
	inline void autoSelecionar() {autoSelec = !autoSelec;}

	void maisVelho();
	void maisEnergia();
	void geracaoMaisNova();
	void geracaoMaisAntiga();
	void maisFilhos();

	void abrirSimulacao();
	void salvarSimulacao();

protected:

	/* Operacoes */
	void posicionarParede(Vetor<float> pos);
	bool selecionarParede(Vetor<float> pos);

	/* Desenhar */
	void paintEvent(QPaintEvent *event);

	/* Eventos do mouse */
	void mouseDoubleClickEvent ( QMouseEvent * event );
	void mouseMoveEvent ( QMouseEvent * event );
	void mousePressEvent ( QMouseEvent * event );
	void wheelEvent ( QWheelEvent * event );
	void mouseReleaseEvent ( QMouseEvent * event );

private:

	/* Paredes */
	struct NohParede *parede_selecionada;

	#define PONTA_INICIO 0
	#define PONTA_FIM 1
	#define MEIO 2

	int pontaSelecionada;
	Vetor<float> ancora_parede;

	/* Representacao grafica */
	float zm;
	float offsetx;
	float offsety;

	/* Interação com usuario */
	Vetor<float> lastPos;
	QMenu *menuBiota;
	QMenu *menuGrao;
	QMenu *menuVoid;
	QMenu *menuParede;
	void mudaBarraStatus();
	bool fixar;
	bool autoSelec;

	/* Controle de execucao */
	QMutex *mutex;

	/* telas */
	Info *info;
	Stat *stat;
};

#endif
