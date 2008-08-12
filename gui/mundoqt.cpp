#include "mundoqt.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <stdio.h>
#include <stdlib.h>
#include <QFileDialog>

#define RAIO_SELECAO 10
#define RAIO_PAREDE (GROSSURA_PAREDE * 2)

#define COR_MORTE Qt::cyan
#define COR_NASCIMENTO QColor(percorre->biota.genes.cor_cabeca[0],percorre->biota.genes.cor_cabeca[1],percorre->biota.genes.cor_cabeca[2])
#define IDADE_COR 5

/******************************************************************************/
/* Constructor */
MundoQT::MundoQT(QWidget *parent, Qt::WFlags f) : QWidget(parent, f), Mundo()
{
	parede_selecionada = NULL;
	mutex = NULL;

	offsetx = 0;
	offsety = 0;
	zm = 0.5f;
	executando = true;
	fixar = false;
	autoSelec = false;

	/* Verifica se existe arquivo com configuracoes */
	FILE *arq = fopen("simvida.cfg", "r");

	if (arq == NULL)
	{
		/* Nao existe arquivo : coloca valores padrao */
		propriedades.tamanho_x = TAMANHO_X;
		propriedades.tamanho_y =  TAMANHO_Y;
		propriedades.energia_grao = ENERGIA_GRAO;
		propriedades.teto_energetico = TETO_ENERGETICO;
		propriedades.probabilidade_mutacao = PROBABILIDADE_MUTACAO;
		propriedades.intensidade_mutacao = INTENSIDADE_MUTACAO;
		/* Cria arquivo e escreve valores padrao */
		arq = fopen("simvida.cfg", "w");
		fprintf(arq, "%d\n%d\n%d\n%d\n%d\n%d\n",
		TAMANHO_X, TAMANHO_Y, ENERGIA_GRAO,
		TETO_ENERGETICO, PROBABILIDADE_MUTACAO, INTENSIDADE_MUTACAO);
	}
	else
	{
		/* Carrega configuracoes de arquivo */
		fscanf(arq, "%d\n%d\n%d\n%d\n%d\n%d\n",
		&(propriedades.tamanho_x), &(propriedades.tamanho_y),
		&(propriedades.energia_grao),
		&(propriedades.teto_energetico), &(propriedades.probabilidade_mutacao),
		&(propriedades.intensidade_mutacao));
	}
	/* Fecha arquivo */
	fclose(arq);

	reiniciar();
}

/******************************************************************************/

MundoQT::~MundoQT()
{

}

/******************************************************************************/
/* Desenhar */
void MundoQT::paintEvent(QPaintEvent *event)
{

	/* configura o painter */
    QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

	int dx = width()/2;
	int dy = height()/2;

	/* Configura o brush */
	QBrush brush(QColor(200, 220, 255));
	painter.setBrush(brush);

	/* Limpa tela na regiao */
	painter.drawRect(event->rect());
	
	/* Configura pen */
	QPen pen;
	pen.setColor(Qt::black);
	pen.setWidth((int)(zm*GROSSURA_PAREDE*2)+1);
	painter.setPen(pen);

	/* Desenha tela principal */
	brush.setColor(Qt::white);
	painter.setBrush(brush);
	painter.drawRect(
	(int)(-offsetx*zm+dx),(int)(-offsety*zm+dy),
	(int)(propriedades.tamanho_x*zm), (int)(propriedades.tamanho_y*zm));

	/* Desenha paredes */
	mutex->lock();
	brush.setColor(Qt::black);
	painter.setBrush(brush);
	for (struct NohParede *percorre = nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		painter.drawLine(
		(int)((percorre->inicio.X-offsetx)*zm+dx),
		(int)((percorre->inicio.Y-offsety)*zm+dy),
		(int)((percorre->fim.X-offsetx)*zm+dx),
		(int)((percorre->fim.Y-offsety)*zm+dy));

		painter.drawEllipse(
		(int)(((percorre->inicio.X - RAIO_PAREDE)-offsetx)*zm+dx),
		(int)(((percorre->inicio.Y - RAIO_PAREDE)-offsety)*zm+dy),
		(int)(RAIO_PAREDE*2.0f*zm),(int)(RAIO_PAREDE*2.0f*zm));

		painter.drawEllipse(
		(int)(((percorre->fim.X - RAIO_PAREDE)-offsetx)*zm+dx),
		(int)(((percorre->fim.Y - RAIO_PAREDE)-offsety)*zm+dy),
		(int)(RAIO_PAREDE*2.0f*zm),(int)(RAIO_PAREDE*2.0f*zm));
	}
	mutex->unlock();

	/* Seta tamanho e cor da linha */
	pen.setColor(Qt::black);
	pen.setWidth((int)(zm+1));
	painter.setPen(pen);

	/* Desenha graos */
	mutex->lock();
	brush.setColor(Qt::cyan);
	painter.setBrush(brush);
	for (struct NohGrao *percorre = nohCabecaGraos.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* Corrige posicao do grao, caso seja necessario */
		if (percorre->posicao.X > (signed int)propriedades.tamanho_x ||
		    percorre->posicao.Y > (signed int)propriedades.tamanho_y)
			percorre->posicao =
			Vetor<float>(rand() % propriedades.tamanho_x, rand() % propriedades.tamanho_y);

		/* Desenha grao */
		if (
		((percorre->posicao.X + RAIO_GRAO)-offsetx)*zm+dx >= event->rect().x() &&
		((percorre->posicao.X - RAIO_GRAO)-offsetx)*zm+dy <= event->rect().x()+ event->rect().width() &&
		((percorre->posicao.Y + RAIO_GRAO)-offsety)*zm+dx >= event->rect().y() &&
		((percorre->posicao.Y - RAIO_GRAO)-offsety)*zm+dy <= event->rect().y() + event->rect().height())
		{
			if (percorre == grao_selecionado)
			{
				pen.setWidth((int)(zm+3));
				painter.setPen(pen);
			}

			painter.drawEllipse(
			(int)(((percorre->posicao.X - RAIO_GRAO)-offsetx)*zm+dx),
			(int)(((percorre->posicao.Y - RAIO_GRAO)-offsety)*zm+dy),
			(int)(RAIO_GRAO*2.0f*zm), (int)(RAIO_GRAO*2.0f*zm));

			if (percorre == grao_selecionado)
			{
				pen.setWidth((int)(zm+1));
				painter.setPen(pen);
			}
		}
	}
	mutex->unlock();

	/* Desenha biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
		if (
		((percorre->biota.estado.posicao.X + (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsetx)*zm+dx >= event->rect().x() &&
		((percorre->biota.estado.posicao.Y + (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsety)*zm+dy >= event->rect().y() &&
		((percorre->biota.estado.posicao.X - (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsetx)*zm+dx <= event->rect().x()+ event->rect().width() &&
		((percorre->biota.estado.posicao.Y - (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsety)*zm+dy <= event->rect().y() + event->rect().height())
		{
			/* Coloca cor de acordo com energia do biota */
			pen.setColor(
			(percorre->biota.estado.energia <= 1)? COR_MORTE :
			(percorre->biota.estado.idade <= IDADE_COR-1)? COR_NASCIMENTO :
			QColor(0,
			(int)(((float)percorre->biota.estado.energia/(float)percorre->biota.genes.limiar_reproducao)*255.0f) % 255,
			(int)(((float)percorre->biota.estado.energia/(float)percorre->biota.genes.limiar_reproducao)*255.0f) % 255 ));
			painter.setPen(pen);

			/* Desenha cabeca */
			brush.setColor(
			(percorre->biota.estado.energia <= 1)? COR_MORTE :
			(percorre->biota.estado.idade <= IDADE_COR)? COR_NASCIMENTO :
			QColor(percorre->biota.genes.cor_cabeca[0],percorre->biota.genes.cor_cabeca[1],percorre->biota.genes.cor_cabeca[2]));
			painter.setBrush(brush);

			painter.drawEllipse(
			(int)(((percorre->biota.estado.posicao.X - percorre->biota.genes.massa_cabeca)-offsetx)*zm+dx),
			(int)(((percorre->biota.estado.posicao.Y - percorre->biota.genes.massa_cabeca)-offsety)*zm+dy),
			(int)(percorre->biota.genes.massa_cabeca*2.0f*zm), (int)(percorre->biota.genes.massa_cabeca*2.0f*zm));

			/* Desenha linha de direcao */
			painter.drawLine(
			(int)((percorre->biota.estado.posicao.X-offsetx)*zm+dx),
			(int)((percorre->biota.estado.posicao.Y-offsety)*zm+dy),
			(int)(((percorre->biota.estado.posicao.X +
			(cos(percorre->biota.estado.angulo) * percorre->biota.genes.massa_cabeca))-offsetx)*zm+dx),
			(int)(((percorre->biota.estado.posicao.Y +
			(sin(percorre->biota.estado.angulo) * percorre->biota.genes.massa_cabeca))-offsety)*zm+dy));

			/* Desenha brilho na cabeca */
			pen.setStyle(Qt::NoPen);	painter.setPen(pen);
			brush.setColor(Qt::white);	painter.setBrush(brush);
			painter.drawEllipse(
			(int)((((percorre->biota.estado.posicao.X + (percorre->biota.genes.massa_cabeca/2.0f)) - (percorre->biota.genes.massa_cabeca/6.0f))-offsetx)*zm+dx),
			(int)((((percorre->biota.estado.posicao.Y - (percorre->biota.genes.massa_cabeca/2.0f)) - (percorre->biota.genes.massa_cabeca/6.0f))-offsety)*zm+dy),
			(int)((percorre->biota.genes.massa_cabeca/3.0f)*zm), (int)((percorre->biota.genes.massa_cabeca/3.0f)*zm));
			painter.drawEllipse(
			(int)((((percorre->biota.estado.posicao.X + (percorre->biota.genes.massa_cabeca*0.3f)) - (percorre->biota.genes.massa_cabeca/4.0f))-offsetx)*zm+dx),
			(int)((((percorre->biota.estado.posicao.Y - (percorre->biota.genes.massa_cabeca*0.3f)) - (percorre->biota.genes.massa_cabeca/4.0f))-offsety)*zm+dy),
			(int)((percorre->biota.genes.massa_cabeca/2.0f)*zm), (int)((percorre->biota.genes.massa_cabeca/2.0f)*zm));
			pen.setStyle(Qt::SolidLine);	painter.setPen(pen);

			/* Desenha segmentos */
			for (unsigned int c = 0; c < percorre->biota.numero_segmentos; c++)
			{
				//desenha linha do segmento
				painter.drawLine(
				(int)(((cos(percorre->biota.genes.segmentos[c].angulo+percorre->biota.estado.angulo)*
				percorre->biota.genes.massa_cabeca + percorre->biota.estado.posicao.X)-offsetx) * zm+dx),
				(int)(((sin(percorre->biota.genes.segmentos[c].angulo+percorre->biota.estado.angulo)*
				percorre->biota.genes.massa_cabeca + percorre->biota.estado.posicao.Y)-offsety) * zm+dy),
				(int)(((cos(percorre->biota.genes.segmentos[c].angulo+(sin(percorre->biota.estado.posicaoSegmentos[c])*
				(percorre->biota.genes.segmentos[c].arco/1.0f))+percorre->biota.estado.angulo)*
				(percorre->biota.genes.massa_cabeca+percorre->biota.genes.segmentos[c].comprimento+percorre->biota.genes.segmentos[c].massa)+
				percorre->biota.estado.posicao.X)-offsetx) *zm+dx),
				(int)(((sin(percorre->biota.genes.segmentos[c].angulo+(sin(percorre->biota.estado.posicaoSegmentos[c])*
				(percorre->biota.genes.segmentos[c].arco/1.0f))+percorre->biota.estado.angulo)*
				(percorre->biota.genes.massa_cabeca+percorre->biota.genes.segmentos[c].comprimento+percorre->biota.genes.segmentos[c].massa)+
				percorre->biota.estado.posicao.Y)-offsety) *zm+dy));

				/* Desenha circulo do segmento */
				brush.setColor(
				(percorre->biota.estado.energia <= 1)? COR_MORTE :
				(percorre->biota.estado.idade <= IDADE_COR)? COR_NASCIMENTO :
				QColor(percorre->biota.genes.segmentos[c].cor[0],percorre->biota.genes.segmentos[c].cor[1],percorre->biota.genes.segmentos[c].cor[2]));
				painter.setBrush(brush);

				painter.drawEllipse(
				(int)((((cos(percorre->biota.genes.segmentos[c].angulo+(sin(percorre->biota.estado.posicaoSegmentos[c])*
				(percorre->biota.genes.segmentos[c].arco/1.0f))+percorre->biota.estado.angulo)*
				(percorre->biota.genes.massa_cabeca+percorre->biota.genes.segmentos[c].comprimento+percorre->biota.genes.segmentos[c].massa)+
				percorre->biota.estado.posicao.X) - percorre->biota.genes.segmentos[c].massa)-offsetx)*zm+dx),
				(int)((((sin(percorre->biota.genes.segmentos[c].angulo+(sin(percorre->biota.estado.posicaoSegmentos[c])*
				(percorre->biota.genes.segmentos[c].arco/1.0f))+percorre->biota.estado.angulo)*
				(percorre->biota.genes.massa_cabeca+percorre->biota.genes.segmentos[c].comprimento+percorre->biota.genes.segmentos[c].massa)+
				percorre->biota.estado.posicao.Y) - percorre->biota.genes.segmentos[c].massa)-offsety)*zm+dy),
				(int)(percorre->biota.genes.segmentos[c].massa*2.0f*zm), (int)(percorre->biota.genes.segmentos[c].massa*2.0f*zm));
			}
			/* Se biota esta selecionado */
			if (percorre == selecionado)
			{
				brush.setStyle(Qt::NoBrush);	painter.setBrush(brush);

				pen.setColor(Qt::yellow);	painter.setPen(pen);
				painter.drawEllipse(
				(int)(((percorre->biota.estado.posicao.X - (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsetx)*zm+dx),
				(int)(((percorre->biota.estado.posicao.Y - (percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS))-offsety)*zm+dy),
				(int)((percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS)*2.0f*zm),
				(int)((percorre->biota.genes.massa_cabeca * FATOR_PERCEPCAO_GRAOS)*2.0f*zm));

				pen.setColor(Qt::green);	painter.setPen(pen);
				painter.drawEllipse(
				(int)(((percorre->biota.estado.posicao.X - percorre->biota.genes.massa_cabeca - RAIO_SELECAO)-offsetx)*zm+dx),
				(int)(((percorre->biota.estado.posicao.Y - percorre->biota.genes.massa_cabeca - RAIO_SELECAO)-offsety)*zm+dy),
				(int)((percorre->biota.genes.massa_cabeca + RAIO_SELECAO)*2.0f*zm),
				(int)((percorre->biota.genes.massa_cabeca + RAIO_SELECAO)*2.0f*zm));

				brush.setStyle(Qt::SolidPattern);	painter.setBrush(brush);
			}
	}
	mutex->unlock();
}

/******************************************************************************/

void MundoQT::mouseDoubleClickEvent ( QMouseEvent * event )
{
	/* Salva posicao */
	lastPos = Vetor<float>(event->x(), event->y());

	if (event->button() == Qt::LeftButton)
	{
		/* Sincroniza */
		mutex->lock();

		/* Tenta selecionar grao */
		if (selecionarGrao((Vetor<float>(event->x(), event->y()) - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety)))
			/* Remove selecionado */
			removerGrao();
		else			
			novoGrao();
	
		/* Libera */
		mutex->unlock();

		/* redesenha */
		if (!executando)
			repaint();
	}
}

/******************************************************************************/

void MundoQT::mouseMoveEvent ( QMouseEvent * event )
{
	if ((event->buttons() & Qt::MidButton) != 0)
	{
		/* Movimentacao */
		offsetx +=  (lastPos.X - event->x()) / zm;
		offsety +=  (lastPos.Y - event->y())  / zm;

		/* Arruma limites */
		if (offsetx < 0)
			offsetx = 0;
		else if (offsetx > propriedades.tamanho_x)
			offsetx = propriedades.tamanho_x;
		if (offsety < 0)
			offsety = 0;
		else if (offsety > propriedades.tamanho_y)
			offsety = propriedades.tamanho_y;


		/* emite sinais */
		emit changeHorizontalBar((int)offsetx);
		emit changeVerticalBar((int)offsety);

		/* redesenha */
		if (!executando)
			repaint();
	}
	else if ((event->buttons() & Qt::LeftButton) != 0)
	{
		if (selecionado != NULL)
		{
			/* Arrasta biota */
			selecionado->biota.estado.posicao = 
			((Vetor<float>(event->x(), event->y()) - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety));

			/* redesenha */
			if (!executando)
				repaint();
		}
		else if (grao_selecionado != NULL)
		{
			/* Arrasta grao */
			posicionarGrao
			((Vetor<float>(event->x(), event->y()) - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety));

			/* redesenha */
			if (!executando)
				repaint();
		}
		else if (parede_selecionada != NULL)
		{
			/* Arrasta parede */
			posicionarParede(Vetor<float>(event->x(), event->y()));

			/* redesenha */
			if (!executando)
				repaint();
		}
	}

	/* Salva posicao */
	lastPos = Vetor<float>(event->x(), event->y());
}

/******************************************************************************/

void MundoQT::mousePressEvent ( QMouseEvent * event )
{
	/* Salva posicao */
	lastPos = Vetor<float>(event->x(), event->y());

	/* Seleciona ou desceleciona biota, grao ou parede */
	if (event->button() != Qt::MidButton) {

		Vetor<float> pos = (Vetor<float>(event->x(), event->y()) - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);

		if (!selecionarBiota(pos))
			selecionado = NULL;
		if (!selecionarGrao(pos))
			grao_selecionado = NULL;
		if (!selecionarParede((Vetor<float>(event->x(), event->y()))))
			parede_selecionada = NULL;

		/* redesenha */
		if (!executando)
			repaint();

		if (event->button() == Qt::RightButton) {
			if (selecionado)
			{
				info->setBiota(selecionado->biota);
				info->move(event->globalPos());
				menuBiota->exec(event->globalPos());
			}
			else if (grao_selecionado)
				menuGrao->exec(event->globalPos());
			else if (parede_selecionada)
				menuParede->exec(event->globalPos());
			else if (pos.X > 0 && pos.Y > 0 && pos.X < propriedades.tamanho_x && pos.Y < propriedades.tamanho_y)
				menuVoid->exec(event->globalPos());
		}

		mudaBarraStatus();
	}
}

/******************************************************************************/

void MundoQT::wheelEvent ( QWheelEvent * event )
{
	if (event->delta() > 0 && zm > 0.1f)
	{
		zm = (zm * 0.9f) - 0.001f;

		if (!executando)
			repaint();
	}
	else if (event->delta() < 0 && zm < 4.0f)
	{
		zm = (zm * 1.1f) + 0.001f;
		
		if (!executando)
			repaint();
	}
}

/******************************************************************************/

void MundoQT::mouseReleaseEvent ( QMouseEvent * event )
{
	if (selecionado && executando)
	{
		selecionado->biota.acelerar(Vetor<float>(event->x(), event->y()) - lastPos);	
	}
}

/******************************************************************************/

void MundoQT::offsetxChanged(int vx)
{
	offsetx = vx;

	/* redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::offsetyChanged(int vy)
{
	offsety = vy;

	/* redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::setMutex(QMutex *m)
{
	mutex = m;
}


/******************************************************************************/

QMutex* MundoQT::getMutex()
{
	return mutex;
}

/******************************************************************************/

void MundoQT::atualizar()
{
	static int deadCry = 50;	

	mutex->lock();
	Mundo::atualizar();	

	if (selecionado && fixar)
	{
		offsetx = selecionado->biota.estado.posicao.X;
		offsety = selecionado->biota.estado.posicao.Y;

		/* emite sinais */
		emit changeHorizontalBar((int)offsetx);
		emit changeVerticalBar((int)offsety);
	}
	mutex->unlock();

	if (!selecionado && autoSelec)
	{
		if (deadCry >= 50)
		{	
			deadCry = 0;
			maisVelho();
		}
		else
			deadCry++;			
	}

	mudaBarraStatus();
}

/******************************************************************************/



void MundoQT::estatistica()
{
	/* Atualiza formulario com estatisticas */
	/* Lista de cores */

	struct NohLineage
	{
		unsigned int lineage;	
		unsigned int quantidade;		
		int cor[3];
		struct NohLineage *proximo;
	};

	struct NohLineage cabecaLineage;

	cabecaLineage.proximo = NULL;

	/* Somas */
	float numero_segmentos = 0;
	float massa_cabeca = 0;
	float limiar_reproducao = 0;
	float distribuicao_energetica = 0;
	float idade = 0;
	float energia = 0;
	float geracao = 0;
	float filhos = 0;

	/* Maximos */
	unsigned int max_numero_segmentos = 0;
	int max_massa_cabeca = 0;
	int max_limiar_reproducao = 0;
	float max_distribuicao_energetica = 0;
	unsigned int max_idade = 0;
	float max_energia = 0;
	unsigned int max_geracao = 0;
	unsigned int max_filhos = 0;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		/* Soma valores */
		numero_segmentos += percorre->biota.numero_segmentos;
		massa_cabeca += percorre->biota.genes.massa_cabeca;
		limiar_reproducao += percorre->biota.genes.limiar_reproducao;
		distribuicao_energetica += percorre->biota.genes.distribuicao_energia;
		idade += percorre->biota.estado.idade;
		energia += percorre->biota.estado.energia;
		geracao += percorre->biota.estado.geracao;
		filhos += percorre->biota.estado.filhos;

		/* Calcula maximos */
		if (percorre->biota.numero_segmentos > max_numero_segmentos)
			max_numero_segmentos = percorre->biota.numero_segmentos;
		if (percorre->biota.genes.massa_cabeca > max_massa_cabeca)
			max_massa_cabeca = percorre->biota.genes.massa_cabeca;
		if (percorre->biota.genes.limiar_reproducao > max_limiar_reproducao)
			max_limiar_reproducao = percorre->biota.genes.limiar_reproducao;
		if (percorre->biota.genes.distribuicao_energia > max_distribuicao_energetica)
			max_distribuicao_energetica = percorre->biota.genes.distribuicao_energia;
		if (percorre->biota.estado.idade > max_idade)
			max_idade = percorre->biota.estado.idade;
		if (percorre->biota.estado.energia > max_energia)
			max_energia = percorre->biota.estado.energia;
		if (percorre->biota.estado.geracao > max_geracao)
			max_geracao = percorre->biota.estado.geracao;
		if (percorre->biota.estado.filhos > max_filhos)
			max_filhos = percorre->biota.estado.filhos;

		/* Percorre lista de lineage */
		struct NohLineage *percorre_lin;
		for (percorre_lin = &cabecaLineage;
		percorre_lin->proximo != NULL &&
		percorre_lin->proximo->lineage > percorre->biota.lineage;
		percorre_lin = percorre_lin->proximo)
		;

		/* se encontrou lineage */
		if (percorre_lin->proximo != NULL && (percorre_lin->proximo->lineage == percorre->biota.lineage))
		{
			percorre_lin->proximo->quantidade++;			percorre_lin->proximo->cor[0] += percorre->biota.genes.cor_cabeca[0];
			percorre_lin->proximo->cor[1] += percorre->biota.genes.cor_cabeca[1];
			percorre_lin->proximo->cor[2] += percorre->biota.genes.cor_cabeca[2];
		}
		else
		{
			/* nao existe lineage */
			struct NohLineage *novo = new NohLineage;

			novo->lineage = percorre->biota.lineage;
			novo->proximo = percorre_lin->proximo;
			novo->quantidade = 1;
			novo->cor[0] = percorre->biota.genes.cor_cabeca[0];
			novo->cor[1] = percorre->biota.genes.cor_cabeca[1];
			novo->cor[2] = percorre->biota.genes.cor_cabeca[2];

			percorre_lin->proximo = novo;	
		}
	}
	mutex->unlock();

	/* Divide valores para alcancar a media */
	if (estatisticas.numero_biotas > 0)
	{
		numero_segmentos /= estatisticas.numero_biotas;
		massa_cabeca /= estatisticas.numero_biotas;
		limiar_reproducao /= estatisticas.numero_biotas;
		distribuicao_energetica /= estatisticas.numero_biotas;

		idade /= estatisticas.numero_biotas;
		energia  /= estatisticas.numero_biotas;
		geracao /= estatisticas.numero_biotas;
		filhos /= estatisticas.numero_biotas;
	}

	/* Desenha grafico de cores */
	QPainter painterE(stat->imgEspecies->imagem);
	int altura = 0;

	
	/* Percorre lista de lineages */
	struct NohLineage *percorre_lin = cabecaLineage.proximo;
	while (percorre_lin != NULL)
	{
		/* Desenha */
		painterE.setPen(QPen(QColor(
		percorre_lin->cor[0]/percorre_lin->quantidade, 
		percorre_lin->cor[1]/percorre_lin->quantidade, 
		percorre_lin->cor[2]/percorre_lin->quantidade)));

		painterE.drawLine(
		stat->imgEspecies->imagem->width()-1,
		stat->imgEspecies->imagem->height() -
		(int)(altura * stat->imgEspecies->imagem->height() / (float)estatisticas.numero_biotas),
		stat->imgEspecies->imagem->width()-1,
		stat->imgEspecies->imagem->height() -
		(int)((altura + percorre_lin->quantidade) * stat->imgEspecies->imagem->height() / (float)estatisticas.numero_biotas));

		altura += percorre_lin->quantidade;

		/* Deleta e passa para o proximo */
		struct NohLineage *remove = percorre_lin;
		percorre_lin = percorre_lin->proximo;
		delete remove;
	}
	/* desloca imagem */
	painterE.drawImage(-1,0,*(stat->imgEspecies->imagem));

	/* Pinta grafico de biotas x Graos */
	static int max_numero_biotas_graos = 0;
	static int lastGraoY = -1;
	static int lastBiotaY = -1;

	/* Ajusta valores maximos */
	if (estatisticas.numero_biotas > (unsigned int)max_numero_biotas_graos)
		max_numero_biotas_graos = estatisticas.numero_biotas;
	if (estatisticas.numero_graos > (unsigned int)max_numero_biotas_graos)
		max_numero_biotas_graos = estatisticas.numero_graos;

	/* calcula posicoes */
	int biotaY = stat->imgBiotasGraos->height() -	(int)(estatisticas.numero_biotas * stat->imgBiotasGraos->height() / (float)max_numero_biotas_graos);

	int graoY = stat->imgBiotasGraos->height() -
	(int)(estatisticas.numero_graos * stat->imgBiotasGraos->height() / (float)max_numero_biotas_graos);

	/* Pinta linha branca */
	QPainter painterBG(stat->imgBiotasGraos->imagem);
	painterBG.setPen(QPen(Qt::white));
	painterBG.drawLine(stat->imgBiotasGraos->width()-1, 0, stat->imgBiotasGraos->width()-1, stat->imgBiotasGraos->height());

	/* Pinta linhas do grafico */
	painterBG.setPen(QPen(Qt::red));
	painterBG.drawLine(stat->imgBiotasGraos->width()-2, (lastBiotaY == -1)? biotaY : lastBiotaY, stat->imgBiotasGraos->width()-1, biotaY);
	painterBG.setPen(QPen(Qt::blue));
	painterBG.drawLine(stat->imgBiotasGraos->width()-2, (lastGraoY == -1)? graoY : lastGraoY, stat->imgBiotasGraos->width()-1, graoY);

	/* desloca imagem */
	painterBG.drawImage(-1,0,*(stat->imgBiotasGraos->imagem));

	lastBiotaY = biotaY;
	lastGraoY = graoY;

	if (stat->isVisible())
	{
		/* Escreve dados */
		/* medias */
		stat->labelEnergia->setText(QVariant(energia).toString() + tr(" wens"));
		stat->labelSegmentos->setText(QVariant(numero_segmentos).toString());
		stat->labelCabeca->setText(QVariant(massa_cabeca).toString());
		stat->labelLimiar->setText(QVariant(limiar_reproducao).toString() + tr(" wens"));
		stat->labelDistribuicao->setText(QVariant(distribuicao_energetica*100).toString() + tr(" %"));
		stat->labelGeracao->setText(QVariant(geracao).toString());
		stat->labelIdade->setText(QVariant(idade).toString());
		stat->labelFilhos->setText(QVariant(filhos).toString());
		/* maximos */
		stat->labelEnergia_2->setText(QVariant(max_energia).toString() + tr(" wens"));
		stat->labelSegmentos_2->setText(QVariant(max_numero_segmentos).toString());
		stat->labelCabeca_2->setText(QVariant(max_massa_cabeca).toString());
		stat->labelLimiar_2->setText(QVariant(max_limiar_reproducao).toString() + tr(" wens"));
		stat->labelDistribuicao_2->setText(QVariant(max_distribuicao_energetica*100).toString() + tr(" %"));
		stat->labelGeracao_2->setText(QVariant(max_geracao).toString());
		stat->labelIdade_2->setText(QVariant(max_idade).toString());
		stat->labelFilhos_2->setText(QVariant(max_filhos).toString());
		/* grafico biotas x graos */
		stat->labelGraosxBiotas->setText(QVariant((float)estatisticas.numero_graos/(float)estatisticas.numero_biotas).toString());

		/* Repinta graficos */
		stat->imgEspecies->repaint();
		stat->imgBiotasGraos->repaint();
	}
}

/******************************************************************************/

void MundoQT::mudaBarraStatus()
{
	if (selecionado)
		emit estadoModificado(
		QString::fromUtf8("Idade: ") + QVariant(selecionado->biota.estado.idade).toString() +
		QString::fromUtf8(", Geração: ") + QVariant((int)selecionado->biota.estado.geracao).toString() +
		QString::fromUtf8(", Energia: ") + QVariant((int)selecionado->biota.estado.energia).toString() +
		QString::fromUtf8(", Filhos: ") + QVariant((int)selecionado->biota.estado.filhos).toString());
	else if (grao_selecionado)
		emit estadoModificado(
		QString::fromUtf8("Energia do grão: ") + QVariant(propriedades.energia_grao).toString());
	else
		emit estadoModificado(
		QVariant((int)estatisticas.ciclos).toString() + QString::fromUtf8(" Ciclos, ") +
		QVariant((int)estatisticas.numero_biotas).toString() + QString::fromUtf8(" Biotas, ") +
		QVariant((int)estatisticas.numero_graos).toString() + QString::fromUtf8(" Grãos."));
}

/******************************************************************************/

void MundoQT::limpar()
{
	/* Limpa */

	/* Sincroniza */
	if (mutex != NULL)
		mutex->lock();	

	Mundo::destroy();

	/* Libera */
	if (mutex != NULL)
		mutex->unlock();

	/* redesenha */
	if (!executando && mutex != NULL)
		repaint();

	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::reiniciar()
{
	limpar();

	/* Sincroniza */
	if (mutex != NULL)
		mutex->lock();	

	/* Cria biotas aleatorios */
	int inicial = rand() % 2+((propriedades.tamanho_x*propriedades.tamanho_y)/250000);
	for (int c = 0; c < inicial; c++)
		inserirBiota(Biota((Mundo*)this));

	/* Libera */
	if (mutex != NULL)
		mutex->unlock();

	selecionado = NULL;	

	/* redesenha */
	if (!executando)
		repaint();

	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::posicionarParede(Vetor<float> pos)
{
	if (pontaSelecionada == PONTA_INICIO)
		parede_selecionada->inicio = (pos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);
	else if (pontaSelecionada == PONTA_FIM)
		parede_selecionada->fim = (pos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);
	else
	{
		Vetor<float> posicao = (pos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);

		parede_selecionada->inicio += posicao - ancora_parede;
		parede_selecionada->fim += posicao - ancora_parede;
		ancora_parede = posicao;
	}
}


/******************************************************************************/

bool MundoQT::selecionarParede(Vetor<float> pos)
{
	Vetor<float> posicao = (pos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);

	/* Percorre lista de paredes */
	for (struct NohParede *percorre = nohCabecaParedes.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (posicao.distancia(percorre->inicio) <= DISTANCIA_SELECAO_PAREDE)
		{
			parede_selecionada = percorre;
			pontaSelecionada = PONTA_INICIO;
			return true;
		}
		if (posicao.distancia(percorre->fim) <= DISTANCIA_SELECAO_PAREDE)
		{
			parede_selecionada = percorre;
			pontaSelecionada = PONTA_FIM;
			return true;
		}

		Vetor<float> u = posicao - percorre->inicio;
		Vetor<float> v = percorre->fim - percorre->inicio;
		Vetor<float> proj = (v.modulo() == 0)? posicao : (
		(v / v.modulo ()) *
		(((u.X * v.X)+(u.Y * v.Y)) / v.modulo()) );

		proj += percorre->inicio;

		/* Corrige limites da parede */
		if (proj.X > percorre->inicio.X && proj.X > percorre->fim.X)
			proj = (percorre->inicio.X > percorre->fim.X)? percorre->inicio : percorre->fim;
		else if (proj.X < percorre->inicio.X && proj.X < percorre->fim.X)
			proj = (percorre->inicio.X < percorre->fim.X)? percorre->inicio : percorre->fim;
		else if (proj.Y > percorre->inicio.Y && proj.Y > percorre->fim.Y)
			proj = (percorre->inicio.Y > percorre->fim.Y)? percorre->inicio : percorre->fim;
		else if (proj.Y < percorre->inicio.Y && proj.Y < percorre->fim.Y)
			proj = (percorre->inicio.Y < percorre->fim.Y)? percorre->inicio : percorre->fim;

		/* Testa distancia */
		if (posicao.distancia(proj) <=
		GROSSURA_PAREDE)
		{
			parede_selecionada = percorre;
			pontaSelecionada = MEIO;
			ancora_parede = proj;
			return true;
		}
	}
	return false;
}

/******************************************************************************/

void MundoQT::novaParede()
{
	mutex->lock();
	struct NohParede *novaParede = new struct NohParede;

	novaParede->inicio = (lastPos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);
	novaParede->fim = (lastPos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety);
	novaParede->proximo = nohCabecaParedes.proximo;
	nohCabecaParedes.proximo = novaParede;

	parede_selecionada = novaParede;
	pontaSelecionada = PONTA_FIM;
	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::removerParede()
{
	mutex->lock();
	/* Percorre lista de paredes */
	for (struct NohParede *percorre = &nohCabecaParedes;
	percorre->proximo != NULL;
	percorre = percorre->proximo)
		if (percorre->proximo == parede_selecionada)
		{
			struct NohParede *remover = percorre->proximo;
			percorre->proximo = percorre->proximo->proximo;
			delete remover;
			parede_selecionada = NULL;

			break;
		}
	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::abrirParedes()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Esquema de Paredes"),"",tr("Esquema de Paredes (*.wall)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "r");

	/* Remove todas as paredes */
	mutex->lock();
	Mundo::abrirParedes(arq);
	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();

	fclose(arq);
}

/******************************************************************************/

void MundoQT::salvarParedes()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Salvar Esquema de Paredes"),"",tr("Esquema de Paredes (*.wall)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "w");

	Mundo::salvarParedes(arq);
	fclose(arq);
}

/******************************************************************************/

void MundoQT::novoGrao()
{
	/* Insere novo grao na posicao */
	mutex->lock();
	inserirGrao((lastPos - (Vetor<float>(width(),height())/2.0f))/zm + Vetor<float>(offsetx, offsety));
	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::removerGrao()
{
	removerGraoSelecionado();
}

/******************************************************************************/

void MundoQT::salvarBiota()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Salvar Biota"),"",tr("Biota (*.biota)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "w");

	selecionado->biota.salvar(arq);
	fclose(arq);
}

/******************************************************************************/

void MundoQT::abrirBiota()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Biota"),"",tr("Biota (*.biota)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "r");

	mutex->lock();
	Biota biota = Biota((Mundo*)this, ((lastPos-Vetor<float>(width()/2,height()/2))/zm) + Vetor<float>(offsetx, offsety), arq);
	inserirBiota(biota);
	mutex->unlock();

	fclose(arq);

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::novoBiota()
{
	mutex->lock();

	Biota biota((Mundo*)this);
	biota.estado.posicao = ((lastPos - Vetor<float>(width()/2,height()/2))/zm) + Vetor<float>(offsetx, offsety);

	inserirBiota(biota);

	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::removerBiota()
{
	mutex->lock();

	/* Percorre a lista de biotas em busca do selecionado*/
	struct NohBiota *percorre;
	for (percorre = &nohCabecaBiotas;
	percorre->proximo != selecionado;
	percorre = percorre->proximo);

	/* Remove biota da lista */
	struct NohBiota *remover = percorre->proximo;
	percorre->proximo = percorre->proximo->proximo;
	/* Desceleciona */
	selecionado = NULL;
	/* Libera memoria */
	remover->biota.destroy();
	delete remover;
	/* Decrementa contador de biotas */
	estatisticas.numero_biotas--;

	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::mutacaoBiota()
{
	mutex->lock();
	selecionado->biota.mutacao();
	mutex->unlock();

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::setMenuBiota(QMenu *m)
{
	menuBiota = m;
}

/******************************************************************************/

void MundoQT::setMenuGrao(QMenu *m)
{
	menuGrao = m;
}

/******************************************************************************/

void MundoQT::setMenuVoid(QMenu *m)
{
	menuVoid = m;
}

/******************************************************************************/

void MundoQT::setMenuParede(QMenu *m)
{
	menuParede = m;
}

/******************************************************************************/

void MundoQT::maisVelho()
{
	NohBiota *maximo = NULL;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (maximo == NULL || percorre->biota.estado.idade > maximo->biota.estado.idade)
			maximo = percorre;
	}
	mutex->unlock();

	selecionado = maximo;

	/* track */
	offsetx = selecionado->biota.estado.posicao.X;
	offsety = selecionado->biota.estado.posicao.Y;

	/* emite sinais */
	emit changeHorizontalBar((int)offsetx);
	emit changeVerticalBar((int)offsety);
	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::maisEnergia()
{
	NohBiota *maximo = NULL;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (maximo == NULL || percorre->biota.estado.energia > maximo->biota.estado.energia)
			maximo = percorre;
	}
	mutex->unlock();

	selecionado = maximo;

	/* track */
	offsetx = selecionado->biota.estado.posicao.X;
	offsety = selecionado->biota.estado.posicao.Y;

	/* emite sinais */
	emit changeHorizontalBar((int)offsetx);
	emit changeVerticalBar((int)offsety);
	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::geracaoMaisNova()
{
	NohBiota *maximo = NULL;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (maximo == NULL || percorre->biota.estado.geracao > maximo->biota.estado.geracao)
			maximo = percorre;
	}
	mutex->unlock();

	selecionado = maximo;

	/* track */
	offsetx = selecionado->biota.estado.posicao.X;
	offsety = selecionado->biota.estado.posicao.Y;

	/* emite sinais */
	emit changeHorizontalBar((int)offsetx);
	emit changeVerticalBar((int)offsety);
	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::geracaoMaisAntiga()
{
	NohBiota *maximo = NULL;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (maximo == NULL || percorre->biota.estado.geracao < maximo->biota.estado.geracao)
			maximo = percorre;
	}
	mutex->unlock();

	selecionado = maximo;

	/* track */
	offsetx = selecionado->biota.estado.posicao.X;
	offsety = selecionado->biota.estado.posicao.Y;

	/* emite sinais */
	emit changeHorizontalBar((int)offsetx);
	emit changeVerticalBar((int)offsety);
	mudaBarraStatus();
}


/******************************************************************************/

void MundoQT::maisFilhos()
{
	NohBiota *maximo = NULL;

	/* Percorre a lista de biotas */
	mutex->lock();
	for (struct NohBiota *percorre = nohCabecaBiotas.proximo;
	percorre != NULL;
	percorre = percorre->proximo)
	{
		if (maximo == NULL || percorre->biota.estado.filhos > maximo->biota.estado.filhos)
			maximo = percorre;
	}
	mutex->unlock();

	selecionado = maximo;

	/* track */
	offsetx = selecionado->biota.estado.posicao.X;
	offsety = selecionado->biota.estado.posicao.Y;

	/* emite sinais */
	emit changeHorizontalBar((int)offsetx);
	emit changeVerticalBar((int)offsety);
	mudaBarraStatus();
}

/******************************************************************************/

void MundoQT::abrirSimulacao()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Abrir Simulação"),"",QString::fromUtf8("Simulação (*.simvida)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "r");

	mutex->lock();
	Mundo::abrirMundo(arq);
	mutex->unlock();

	fclose(arq);

	/* Redesenha */
	if (!executando)
		repaint();
}

/******************************************************************************/

void MundoQT::salvarSimulacao()
{
	QString fileName = QFileDialog::getSaveFileName(this, QString::fromUtf8("Salvar Simulação"),"",QString::fromUtf8("Simulação (*.simvida)"));

	if (fileName.isNull())
		return;

	FILE *arq;
	arq = fopen(fileName.toAscii().data(), "w");

	Mundo::salvarMundo(arq);
	fclose(arq);
}

/******************************************************************************/
