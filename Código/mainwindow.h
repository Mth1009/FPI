#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QImage>
#include <QMainWindow>

#include <vector>

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
    void criarAtalhos();
    void abrirImagem();
    void salvarImagem();
    void espelharHorizontalmente();
    void espelharVerticalmente();
    void converterParaCinza();
    void quantizar();
    void restaurarOriginal();
    void atualizarResultado(const QString &mensagem);
    void atualizarControles(bool habilitados);
    QImage criarQImage(const std::vector<unsigned char> &dados) const;

    ImagePanel *painelOriginal;
    ImagePanel *painelResultado;
    QLabel *nomeArquivoLabel;
    QLabel *detalhesLabel;
    QPushButton *salvarButton;
    QPushButton *horizontalButton;
    QPushButton *verticalButton;
    QPushButton *cinzaButton;
    QPushButton *quantizacaoButton;
    QPushButton *restaurarButton;

    std::vector<unsigned char> dadosOriginais;
    std::vector<unsigned char> dadosResultado;
    int largura = 0;
    int altura = 0;
    QString caminhoAtual;
};

#endif // MAINWINDOW_H
