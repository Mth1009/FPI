#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QImage>
#include <QMainWindow>

#include <vector>

class QComboBox;
class QLabel;
class QPushButton;

class ImagePanel : public QWidget {
public:
    explicit ImagePanel(const QString &titulo, QWidget *parent = nullptr);
    void setImage(const QImage &imagem);
    void clearImage();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void atualizarImagem();

    QLabel *imagemLabel;
    QImage imagem;
};

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void montarInterface();
    void aplicarOperacaoSelecionada();
    void criarAtalhos();
    void abrirImagem();
    void salvarImagem();
    void espelharHorizontalmente();
    void espelharVerticalmente();
    void converterParaCinza();
    void quantizar();
    void calcularHistograma();
    void ajustarBrilho();
    void ajustarContraste();
    void aplicarNegativo();
    void reduzirImagem();
    void rotacionarHorario();
    void rotacionarAntiHorario();
    void ampliarImagem();
    void aplicarConvolucao();
    void equalizarHistograma();
    void corresponderHistograma();
    void restaurarOriginal();
    void atualizarResultado(const QString &mensagem);
    void atualizarControles(bool habilitados);
    QImage criarQImage(const std::vector<unsigned char> &dados) const;

    ImagePanel *painelOriginal;
    ImagePanel *painelResultado;
    QLabel *nomeArquivoLabel;
    QLabel *detalhesLabel;
    QPushButton *salvarButton;
    QComboBox *operacaoCombo;
    QPushButton *aplicarButton;
    QPushButton *restaurarButton;

    std::vector<unsigned char> dadosOriginais;
    std::vector<unsigned char> dadosResultado;
    int largura = 0;
    int altura = 0;
    int larguraOriginal = 0;
    int alturaOriginal = 0;
    QString caminhoAtual;
};

#endif // MAINWINDOW_H
