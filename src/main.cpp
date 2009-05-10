/** \file
    Ponto de inicio do processo do sistema.
    Cria uma janela QT principal e inicia o mundo Simvida
    \author Rodrigo Setti
*/
#include <QApplication>
#include <QTranslator>

#include "gui/main-window.h"

/** \mainpage Simvida

    O que é vida? Será "vivo" somente os processos que conhecemos baseados em carbono? ou vida é uma definição mais ampla que abrangeria qualquer processo capaz de replicar-se, evoluir e persistir?

    Este projeto não foi criado para responder a estas perguntas filosóficas, mas sim para demonstrar de forma mais clara possível que o processo de evolução da vida a qual conhecemos - seleção natural(Darwin) - pode sim ser simulado no computador.Decerto que a complexidade e amplitude do sistema são limitados, devido a própria limitação computacional existente hoje, entretanto, mesmo assim é clara a evolução da "vida" diante dos seus olhos. Para tornar as coisas mais "rápidas" que o mundo real aonde os aperfeiçoamentos genéticos ocorrem em escalas de tempo milenares, as criaturas neste simulador - chamadas de "Biotas" - são proprietárias de um código genético relativamente pequeno, o que possibilita mutações em características mais evidêntes, além disso os ciclos de vida são curtos acelerando as gerações e conseqüentemente as evoluções genéticas adaptativas.

    Um Biota é um ser interessante. Ele é virtual, de fato, não passa de um conjunto de bytes - você pode chamar isso de algo "vivo"? - você pode chamar um simples conjunto de átomos de algo "vivo"?. Os Biotas tem apenas um objetivo na vida desde que nascem: reproduzir-se, em outras palavras, passar seus genes adiante. Isto deve ser feito com prioritária urgência já que seu tempo de vida é limitado, os Biotas vão ficando com crescente ineficiência com a idade até que não consigam mais sustentar sua vida e morrem.

    O único parâmetro de saúde de um Biota é sua quantidade de energia, quando este valor atinge zero ele morre, e em contrapartida, quando atinge um determinado limiar estipulado por seus genes ele cria um filho(reprodução assexuada).
    Ao seu filho o pai doa certa percentagem de sua energia, também determinada pelos genes.

    Os Biotas para atingir seu limiar de reprodução e passar seus genes adiante prescisam conseguir mais energia. Isso é feito correndo atrás de pequenos grãos azuis de energia que fornecem uma generosa quantidade de energia a quem comê-lo. Entretanto, os grãos são freqüentemente escassos e aparecem em locais aleatórios, na verdade na maioria das vezes existem mais Biotas do que grãos no sistema, isso significa que quando você encontrar um grão terá que competi-lo com outro Biota.

    Os Biotas, para conseguir grãos precisam nadar, e o nado consome energia, portanto é melhor que toda energia gasta nadando pelo grão compense pela energia que ele fornece. Existem infinitas configurações anatômicas e comportamentais possíveis para um Biota, algumas são extremamente ineficiêntes fazendo-o gastar quilos de energia para nadar poucos centímetros, outras configurações simplesmente não saem do lugar... Apenas uma pequena parcela destas permite à uma espécie conseguir nadar atrás de grãos, juntar energia, reproduzir e passar seus genes adiante para que seus filhos façam o mesmo e assim por diante. A cada reprodução existe uma probabilidade de ocorrer uma mutação genética aleatória em qualquer gene do filho, as mutações afetam desde o movimento do nado, pesos da cabeça e segmentos, comprimentos, limiar energético que ativa a reprodução e outros genes diversos.

    Uma mutação - entre os Biotas - não é muito bem vista, isto porquê frequentemente os mutantes são seres esquisitos e com variações que lhes atrapalha a vida.

    Não conseguem competir por comida e ter filhos e acabam morrendo, seus filhos - se tiver - também não terão um futuro muito melhor. Apesar disso, algumas mutações dão aos sortudos mutantes habilidades ligeiramente superiores que seus pais, garantido uma vantagem seja no gasto menor de energia para nadar, ou uma velocidade maior, ou um ajuste mais preciso dos parâmetros de comportamento.

    Esta característica lhe permitirá ter mais filhos, e estes também mais filhos, competindo contra outros pobres Biotas inferiores e subjulgando outras raças, a menos que aquelas também possuam seus "mutantes". Enfim, gerações e gerações se passam, e é possível notar um aumento crescente na eficiência e adaptação dos Biotas as características do ambiente.

    É possível salvar os genes de um biota num arquivo e carrega-lo depois em outra simulação para analizar o efeito da introdução da espécie. Paredes podem ser criadas no ambiente criando "isolamentos geográficos" entre os animais. Verdadeiras guerras biológicas são travadas entre espécies no dominio do nicho que compartilham. Você pode descobrir muita coisa com o SimVidA.

    Analizar tendências evolutivas relativas aos parâmetros do mundo que você estipulou, salvar padrões genéticos bem sucedidos e competi-los, clonar, mutar, matar a vontade sem peso na consciência. Arquitetar esquemas de paredes que funcionam como fazendas de Biotas, criar  espécies para fins específicos e o que mais sua imaginação permitir.

    \author Rodrigo Setti
*/
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(recursos);

    QApplication app(argc, argv);

	QTranslator translator;
	if (translator.load("simvida_" + QLocale::system().name()))
		app.installTranslator(&translator);
	else
		qWarning("Translation loading failed.");

	MainWindow *mainWindow = new MainWindow();
    mainWindow->showMaximized();
    return app.exec();
}
